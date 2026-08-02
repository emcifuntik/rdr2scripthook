#include "stdafx.h"

#include "PostFrontendRenderer.h"

#include <d3d12.h>
#include <dxgi1_6.h>
#include <wrl/client.h>

#define VK_USE_PLATFORM_WIN32_KHR
#define VK_NO_PROTOTYPES
#include <vulkan/vulkan.h>

#include <algorithm>
#include <atomic>
#include <cstdint>
#include <cstring>
#include <mutex>
#include <utility>
#include <vector>

namespace rdr2::graphics {
namespace {

using EndDraw = void (*)(void* context);
using SetRenderTargets = void (*)(void* context, int count, void** targets,
                                  bool updateViewport);
using SetDepthStencil = void (*)(void* context, void* depthStencil,
                                 uint8_t stencilRef, uint8_t unknown);

std::atomic<PostFrontendCallback> g_postFrontendCallback = nullptr;
std::atomic<PostFrontendSubmittedCallback> g_postFrontendSubmittedCallback =
    nullptr;
std::atomic_bool g_postFrontendReady = false;
EndDraw g_originalEndDraw = nullptr;
SetRenderTargets g_setRenderTargets = nullptr;
SetDepthStencil g_setDepthStencil = nullptr;

// Both values point at globals owned by RDR2. The first global contains the
// active SGA driver object and the second contains the D3D12 device.
std::uint64_t** g_sgaDriver = nullptr;
ID3D12Device** g_d3d12Device = nullptr;
ID3D12CommandQueue** g_d3d12CommandQueue = nullptr;
void** g_vulkanDevice = nullptr;
void** g_vulkanQueue = nullptr;
std::atomic<ID3D12CommandQueue*> g_validatedD3D12CommandQueue = nullptr;
std::atomic_bool g_invalidD3D12CommandQueueLogged = false;
std::uint32_t g_swapchainBackbufferVtableOffset = 0;

PFN_vkCreateDevice g_originalVkCreateDevice = nullptr;
PFN_vkEnumerateDeviceExtensionProperties
    g_vkEnumerateDeviceExtensionProperties = nullptr;
PFN_vkGetPhysicalDeviceProperties2 g_vkGetPhysicalDeviceProperties2 = nullptr;
PFN_vkGetPhysicalDeviceQueueFamilyProperties
    g_vkGetPhysicalDeviceQueueFamilyProperties = nullptr;
PFN_vkGetDeviceQueue g_vkGetDeviceQueue = nullptr;
PFN_vkGetDeviceProcAddr g_vkGetDeviceProcAddr = nullptr;
PFN_vkQueueSubmit g_originalVkQueueSubmit = nullptr;
std::atomic<VkQueue> g_interceptedVulkanQueue = VK_NULL_HANDLE;
struct PendingVulkanFenceOperation {
    VkSemaphore semaphore = VK_NULL_HANDLE;
    std::uint64_t value = 0;
    std::uint32_t submissionsToSkip = 0;
};
std::mutex g_vulkanSubmitMutex;
PendingVulkanFenceOperation g_pendingVulkanProducerWait;
PendingVulkanFenceOperation g_pendingVulkanConsumerSignal;
std::atomic_bool g_vulkanSubmitHookLogged = false;
struct VulkanDeviceRecord {
    VkDevice device = VK_NULL_HANDLE;
    VkQueue graphicsQueue = VK_NULL_HANDLE;
    LUID adapterLuid{};
    bool hasAdapterLuid = false;
};
std::mutex g_vulkanDeviceRecordsMutex;
std::vector<VulkanDeviceRecord> g_vulkanDeviceRecords;

bool IsInsideGameImage(std::uintptr_t address, std::size_t size)
{
    const std::uintptr_t base = CMemory::Base();
    const std::size_t imageSize = CMemory::GetSize();
    return size <= imageSize && address >= base &&
           address <= base + imageSize - size;
}

ID3D12Device** ResolveD3D12DeviceGlobal()
{
    // This form is ideal when available because it begins exactly at the
    // RIP-relative MOV and can be decoded directly by CMemory::GetOffset.
    constexpr CMemory::Pattern extendedPattern(
        "48 8B 0D ? ? ? ? 8B 54 24 ? 48 8B 01 FF 50 78 48 8B 0B 48 8D");
    CMemory extended = extendedPattern.Search();
    if (extended.IsValid())
        return extended.GetOffset().Get<ID3D12Device**>();

    // The stack-load between the device MOV and the virtual call varies
    // between retail builds. The virtual-call sequence itself is stable, so
    // find it and decode the closest preceding `mov rcx, [rip+disp32]`.
    constexpr CMemory::Pattern stableCallPattern(
        "48 8B 01 FF 50 78 48 8B 0B 48 8D");
    CMemory stableCall = stableCallPattern.Search();
    if (!stableCall.IsValid()) return nullptr;

    const auto* call = stableCall.Get<std::uint8_t*>();
    for (std::size_t distance = 1; distance <= 48; ++distance) {
        const auto* instruction = call - distance;
        if (instruction[0] != 0x48 || instruction[1] != 0x8B ||
            instruction[2] != 0x0D) {
            continue;
        }

        std::int32_t displacement = 0;
        std::memcpy(&displacement, instruction + 3,
                    sizeof(displacement));
        const auto target = reinterpret_cast<std::uintptr_t>(instruction + 7) +
                            displacement;
        if (!IsInsideGameImage(target, sizeof(ID3D12Device*))) continue;

        spdlog::warn(
            "[Graphics] Resolved the RDR3 D3D12 device-global address "
            "through the version-tolerant fallback");
        return reinterpret_cast<ID3D12Device**>(target);
    }

    return nullptr;
}

ID3D12CommandQueue** ResolveD3D12CommandQueueGlobal()
{
    // This sequence is in RAGE's live per-frame submission path: load the
    // direct queue, ExecuteCommandLists, then Signal its frame fence. Unlike
    // the device initializer it is already unpacked when this DLL starts.
    constexpr CMemory::Pattern submitDirectQueuePattern(
        "48 8B 0D ? ? ? ? 4C 8B C3 BA 01 00 00 00 "
        "48 8B 01 FF 50 50");
    CMemory submitDirectQueue = submitDirectQueuePattern.Search();
    if (submitDirectQueue.IsValid()) {
        spdlog::info(
            "[Graphics] Resolved the RDR3 D3D12 queue-global address "
            "through the live submission path");
        return submitDirectQueue.GetOffset().Get<ID3D12CommandQueue**>();
    }

    // grcDeviceD3D12 creates its direct queue through
    // ID3D12Device::CreateCommandQueue (vtable offset 0x40). The RIP-relative
    // LEA immediately before the call supplies the address of the game's queue
    // global as the fourth argument.
    constexpr CMemory::Pattern createDirectQueuePattern(
        "4C 8D 0D ? ? ? ? 48 89 7D ? 4C 8D 05 ? ? ? ? "
        "89 7D ? 48 8D 55 ? 44 89 6D ? 48 8B 01 FF 50 40");
    CMemory createDirectQueue = createDirectQueuePattern.Search();
    if (createDirectQueue.IsValid())
        return createDirectQueue.GetOffset().Get<ID3D12CommandQueue**>();

    return nullptr;
}

void ResolveVulkanGlobals()
{
    // grcDeviceVK clears its dispatch table and then loads the VkDevice from
    // this RIP-relative global. The surrounding dispatch-table initialization
    // keeps the signature specific to the Vulkan device setup path.
    constexpr CMemory::Pattern devicePattern(
        "8D 50 41 8B CA 44 8B C2 F3 48 AB 48 8B 0D");
    CMemory device = devicePattern.Search();
    if (device.IsValid())
        g_vulkanDevice = (device + 14).GetOffset().Get<void**>();

    // vkGetDeviceQueue(device, graphicsFamily, 0, &primaryQueue).
    constexpr CMemory::Pattern queuePattern(
        "4C 8B 0D ? ? ? ? 45 33 C0 48 8B 0D ? ? ? ? "
        "41 8B D6 66 89 1D ? ? ? ? FF 15");
    CMemory queue = queuePattern.Search();
    if (queue.IsValid())
        g_vulkanQueue = queue.GetOffset().Get<void**>();
}

bool ContainsExtension(const std::vector<const char*>& extensions,
                       const char* name)
{
    return std::ranges::any_of(extensions, [name](const char* extension) {
        return extension && std::strcmp(extension, name) == 0;
    });
}

bool SupportsExtension(
    const std::vector<VkExtensionProperties>& extensions,
    const char* name)
{
    return std::ranges::any_of(extensions, [name](const auto& extension) {
        return std::strcmp(extension.extensionName, name) == 0;
    });
}

VkResult SubmitExternalFenceWait(
    VkQueue queue, const PendingVulkanFenceOperation& operation)
{
    VkD3D12FenceSubmitInfoKHR fenceValues{
        VK_STRUCTURE_TYPE_D3D12_FENCE_SUBMIT_INFO_KHR};
    fenceValues.waitSemaphoreValuesCount = 1;
    fenceValues.pWaitSemaphoreValues = &operation.value;
    constexpr VkPipelineStageFlags waitStage =
        VK_PIPELINE_STAGE_ALL_COMMANDS_BIT;
    VkSubmitInfo submit{VK_STRUCTURE_TYPE_SUBMIT_INFO};
    submit.pNext = &fenceValues;
    submit.waitSemaphoreCount = 1;
    submit.pWaitSemaphores = &operation.semaphore;
    submit.pWaitDstStageMask = &waitStage;
    return g_originalVkQueueSubmit(queue, 1, &submit, VK_NULL_HANDLE);
}

VkResult SubmitExternalFenceSignal(
    VkQueue queue, const PendingVulkanFenceOperation& operation)
{
    VkD3D12FenceSubmitInfoKHR fenceValues{
        VK_STRUCTURE_TYPE_D3D12_FENCE_SUBMIT_INFO_KHR};
    fenceValues.signalSemaphoreValuesCount = 1;
    fenceValues.pSignalSemaphoreValues = &operation.value;
    VkSubmitInfo submit{VK_STRUCTURE_TYPE_SUBMIT_INFO};
    submit.pNext = &fenceValues;
    submit.signalSemaphoreCount = 1;
    submit.pSignalSemaphores = &operation.semaphore;
    return g_originalVkQueueSubmit(queue, 1, &submit, VK_NULL_HANDLE);
}

VkResult VKAPI_CALL VulkanQueueSubmitHook(
    VkQueue queue, std::uint32_t submitCount,
    const VkSubmitInfo* submits, VkFence fence)
{
    const bool isGraphicsQueue =
        queue == g_interceptedVulkanQueue.load(std::memory_order_acquire);
    PendingVulkanFenceOperation producerWait;
    if (isGraphicsQueue) {
        std::scoped_lock lock(g_vulkanSubmitMutex);
        producerWait = std::exchange(
            g_pendingVulkanProducerWait, {});
    }

    if (producerWait.semaphore != VK_NULL_HANDLE) {
        const VkResult waitResult =
            SubmitExternalFenceWait(queue, producerWait);
        if (waitResult != VK_SUCCESS) {
            spdlog::error(
                "[Graphics] RAGE-thread Vulkan producer wait failed "
                "(VkResult={})",
                static_cast<int>(waitResult));
            return waitResult;
        }
    }

    const VkResult result =
        g_originalVkQueueSubmit(queue, submitCount, submits, fence);
    if (result != VK_SUCCESS || !isGraphicsQueue) return result;

    PendingVulkanFenceOperation consumerSignal;
    {
        std::scoped_lock lock(g_vulkanSubmitMutex);
        if (g_pendingVulkanConsumerSignal.semaphore != VK_NULL_HANDLE &&
            g_pendingVulkanConsumerSignal.submissionsToSkip != 0) {
            --g_pendingVulkanConsumerSignal.submissionsToSkip;
        } else {
            consumerSignal = std::exchange(
                g_pendingVulkanConsumerSignal, {});
        }
    }
    if (consumerSignal.semaphore != VK_NULL_HANDLE) {
        const VkResult signalResult =
            SubmitExternalFenceSignal(queue, consumerSignal);
        if (signalResult != VK_SUCCESS) {
            spdlog::error(
                "[Graphics] RAGE-thread Vulkan consumer signal failed "
                "(VkResult={})",
                static_cast<int>(signalResult));
        }
    }

    if (!g_vulkanSubmitHookLogged.exchange(true,
                                           std::memory_order_acq_rel)) {
        spdlog::info(
            "[Graphics] WebView synchronization integrated with the RAGE "
            "Vulkan submission thread");
    }
    return result;
}

void InstallVulkanQueueSubmitHook(VkDevice device, VkQueue graphicsQueue)
{
    if (!g_vkGetDeviceProcAddr || graphicsQueue == VK_NULL_HANDLE) return;
    g_interceptedVulkanQueue.store(graphicsQueue,
                                   std::memory_order_release);
    if (g_originalVkQueueSubmit) return;

    auto queueSubmit = reinterpret_cast<PFN_vkQueueSubmit>(
        g_vkGetDeviceProcAddr(device, "vkQueueSubmit"));
    if (!queueSubmit ||
        !CMemory(reinterpret_cast<void*>(queueSubmit))
             .Detour(VulkanQueueSubmitHook, &g_originalVkQueueSubmit)) {
        spdlog::error(
            "[Graphics] Could not install the Vulkan queue-submit hook");
    }
}

VkResult VKAPI_CALL CreateVulkanDeviceHook(
    VkPhysicalDevice physicalDevice,
    const VkDeviceCreateInfo* createInfo,
    const VkAllocationCallbacks* allocator, VkDevice* device)
{
    VkDeviceCreateInfo modified = *createInfo;
    std::vector<const char*> enabled;
    enabled.reserve(createInfo->enabledExtensionCount + 7);
    for (std::uint32_t index = 0;
         index < createInfo->enabledExtensionCount; ++index) {
        enabled.push_back(createInfo->ppEnabledExtensionNames[index]);
    }

    std::uint32_t propertyCount = 0;
    std::vector<VkExtensionProperties> available;
    if (g_vkEnumerateDeviceExtensionProperties &&
        g_vkEnumerateDeviceExtensionProperties(
            physicalDevice, nullptr, &propertyCount, nullptr) == VK_SUCCESS) {
        available.resize(propertyCount);
        if (propertyCount != 0) {
            g_vkEnumerateDeviceExtensionProperties(
                physicalDevice, nullptr, &propertyCount, available.data());
            available.resize(propertyCount);
        }
    }

    constexpr const char* requiredExtensions[] = {
        VK_KHR_EXTERNAL_MEMORY_EXTENSION_NAME,
        VK_KHR_EXTERNAL_MEMORY_WIN32_EXTENSION_NAME,
        VK_KHR_EXTERNAL_SEMAPHORE_EXTENSION_NAME,
        VK_KHR_EXTERNAL_SEMAPHORE_WIN32_EXTENSION_NAME,
        VK_KHR_GET_MEMORY_REQUIREMENTS_2_EXTENSION_NAME,
        VK_KHR_DEDICATED_ALLOCATION_EXTENSION_NAME,
        VK_KHR_BIND_MEMORY_2_EXTENSION_NAME,
    };
    std::uint32_t added = 0;
    for (const char* extension : requiredExtensions) {
        if (!ContainsExtension(enabled, extension) &&
            SupportsExtension(available, extension)) {
            enabled.push_back(extension);
            ++added;
        }
    }
    modified.enabledExtensionCount =
        static_cast<std::uint32_t>(enabled.size());
    modified.ppEnabledExtensionNames = enabled.data();

    LUID adapterLuid{};
    bool hasAdapterLuid = false;
    if (g_vkGetPhysicalDeviceProperties2) {
        VkPhysicalDeviceIDProperties idProperties{
            VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_ID_PROPERTIES};
        VkPhysicalDeviceProperties2 properties{
            VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2};
        properties.pNext = &idProperties;
        g_vkGetPhysicalDeviceProperties2(physicalDevice, &properties);
        if (idProperties.deviceLUIDValid) {
            static_assert(sizeof(adapterLuid) == VK_LUID_SIZE);
            std::memcpy(&adapterLuid, idProperties.deviceLUID,
                        sizeof(adapterLuid));
            hasAdapterLuid = true;
        }
    }

    std::uint32_t graphicsQueueFamily = UINT32_MAX;
    if (g_vkGetPhysicalDeviceQueueFamilyProperties) {
        std::uint32_t queueFamilyCount = 0;
        g_vkGetPhysicalDeviceQueueFamilyProperties(
            physicalDevice, &queueFamilyCount, nullptr);
        std::vector<VkQueueFamilyProperties> queueFamilies(
            queueFamilyCount);
        if (queueFamilyCount != 0) {
            g_vkGetPhysicalDeviceQueueFamilyProperties(
                physicalDevice, &queueFamilyCount, queueFamilies.data());
            queueFamilies.resize(queueFamilyCount);
        }
        for (std::uint32_t index = 0;
             index < createInfo->queueCreateInfoCount; ++index) {
            const auto family =
                createInfo->pQueueCreateInfos[index].queueFamilyIndex;
            if (family < queueFamilies.size() &&
                (queueFamilies[family].queueFlags & VK_QUEUE_GRAPHICS_BIT) !=
                    0) {
                graphicsQueueFamily = family;
                break;
            }
        }
    }
    if (graphicsQueueFamily == UINT32_MAX &&
        createInfo->queueCreateInfoCount != 0) {
        graphicsQueueFamily =
            createInfo->pQueueCreateInfos[0].queueFamilyIndex;
    }

    const VkResult result = g_originalVkCreateDevice(
        physicalDevice, &modified, allocator, device);
    VkQueue graphicsQueue = VK_NULL_HANDLE;
    if (result == VK_SUCCESS && device && *device) {
        if (g_vkGetDeviceQueue && graphicsQueueFamily != UINT32_MAX) {
            g_vkGetDeviceQueue(
                *device, graphicsQueueFamily, 0, &graphicsQueue);
        }
        std::scoped_lock lock(g_vulkanDeviceRecordsMutex);
        g_vulkanDeviceRecords.push_back(
            {*device, graphicsQueue, adapterLuid, hasAdapterLuid});
        spdlog::info(
            "[Graphics] Captured Vulkan device{} from vkCreateDevice "
            "(adapter LUID {})",
            graphicsQueue != VK_NULL_HANDLE ? " and graphics queue" : "",
            hasAdapterLuid ? "available" : "unavailable");
    }
    if (result == VK_SUCCESS && device && *device)
        InstallVulkanQueueSubmitHook(*device, graphicsQueue);
    if (result == VK_SUCCESS && added != 0) {
        spdlog::info(
            "[Graphics] Enabled {} Vulkan external-sharing extension(s)",
            added);
    }
    return result;
}

void InstallVulkanDeviceHook()
{
    HMODULE loader = GetModuleHandleW(L"vulkan-1.dll");
    if (!loader) return;

    auto createDevice = reinterpret_cast<PFN_vkCreateDevice>(
        GetProcAddress(loader, "vkCreateDevice"));
    g_vkEnumerateDeviceExtensionProperties =
        reinterpret_cast<PFN_vkEnumerateDeviceExtensionProperties>(
            GetProcAddress(loader,
                           "vkEnumerateDeviceExtensionProperties"));
    g_vkGetPhysicalDeviceProperties2 =
        reinterpret_cast<PFN_vkGetPhysicalDeviceProperties2>(
            GetProcAddress(loader, "vkGetPhysicalDeviceProperties2"));
    g_vkGetPhysicalDeviceQueueFamilyProperties = reinterpret_cast<
        PFN_vkGetPhysicalDeviceQueueFamilyProperties>(
        GetProcAddress(loader, "vkGetPhysicalDeviceQueueFamilyProperties"));
    g_vkGetDeviceQueue = reinterpret_cast<PFN_vkGetDeviceQueue>(
        GetProcAddress(loader, "vkGetDeviceQueue"));
    g_vkGetDeviceProcAddr = reinterpret_cast<PFN_vkGetDeviceProcAddr>(
        GetProcAddress(loader, "vkGetDeviceProcAddr"));
    if (!createDevice || !g_vkEnumerateDeviceExtensionProperties) return;

    if (!CMemory(reinterpret_cast<void*>(createDevice))
             .Detour(CreateVulkanDeviceHook, &g_originalVkCreateDevice)) {
        spdlog::warn(
            "[Graphics] Could not install the Vulkan device-creation hook");
    }
}

template <typename Function>
Function DriverVtableFunction(std::uint64_t* driver, std::size_t byteOffset)
{
    if (!driver) return nullptr;
    auto* vtable = *reinterpret_cast<std::uintptr_t**>(driver);
    if (!vtable) return nullptr;
    return reinterpret_cast<Function>(
        *reinterpret_cast<std::uintptr_t*>(
            reinterpret_cast<std::uint8_t*>(vtable) + byteOffset));
}

void InvokePostFrontend(void* context)
{
    const auto callback =
        g_postFrontendCallback.load(std::memory_order_acquire);
    if (!callback || !g_sgaDriver || !*g_sgaDriver ||
        !g_setRenderTargets || !g_setDepthStencil ||
        g_swapchainBackbufferVtableOffset == 0) {
        g_originalEndDraw(context);
        return;
    }

    auto* driver = *g_sgaDriver;

    using EndDriverDraw = void (*)(std::uint64_t*, void*);
    using GetSwapchainBackbuffer = void* (*)(std::uint64_t*);
    using SetDriverRenderTargets = void (*)(std::uint64_t*, void*,
                                            std::uint64_t, std::uint64_t,
                                            std::uint64_t, char, char);

    const auto endDriverDraw =
        DriverVtableFunction<EndDriverDraw>(driver, 0x328);
    const auto getBackbuffer = DriverVtableFunction<GetSwapchainBackbuffer>(
        driver, g_swapchainBackbufferVtableOffset);
    const auto setDriverRenderTargets =
        DriverVtableFunction<SetDriverRenderTargets>(driver, 0x318);

    if (!endDriverDraw || !getBackbuffer || !setDriverRenderTargets) {
        g_originalEndDraw(context);
        return;
    }

    // Finish the engine draw, bind the swap-chain backbuffer, run our
    // post-frontend callbacks, and then restore the original render targets.
    endDriverDraw(driver, context);

    void* renderTargets[] = {getBackbuffer(driver)};
    if (renderTargets[0]) {
        g_setRenderTargets(context, 1, renderTargets, true);
        g_setDepthStencil(context, nullptr, 0, 0);
        setDriverRenderTargets(driver, context, 0, 0, 0, 1, 0);

        callback(context);

        endDriverDraw(driver, context);
        const auto submittedCallback =
            g_postFrontendSubmittedCallback.load(std::memory_order_acquire);
        if (submittedCallback) submittedCallback();
    }

    g_originalEndDraw(context);
}

CMemory::Hook g_postFrontendHook([] {
    InstallVulkanDeviceHook();

    constexpr CMemory::Pattern endDrawPattern(
        "48 8B CB E8 ? ? ? ? 48 8B 0D ? ? ? ? 0F 57 ED");
    constexpr CMemory::Pattern setDepthStencilPattern(
        "41 B0 01 48 8B D3 48 8B CF E8 ? ? ? ? 48 83");
    constexpr CMemory::Pattern setRenderTargetsPattern(
        "41 56 48 83 EC 20 33 DB 41 8A E9");
    constexpr CMemory::Pattern swapchainBackbufferPattern(
        "41 B1 01 BA 01 00 00 00 48 8B 0C D8");
    constexpr CMemory::Pattern sgaDriverPattern(
        "C6 82 ? ? 00 00 01 C6 82 ? ? 00 00 01 48 8B 0D");

    CMemory endDraw = endDrawPattern.Search();
    CMemory setDepthStencil = setDepthStencilPattern.Search();
    CMemory setRenderTargets = setRenderTargetsPattern.Search();
    CMemory swapchainBackbuffer = swapchainBackbufferPattern.Search();
    CMemory sgaDriver = sgaDriverPattern.Search();

    // Device discovery is deliberately independent of hook discovery. A
    // version-specific D3D instruction must not prevent the render hook from
    // being installed, and vice versa.
    g_d3d12Device = ResolveD3D12DeviceGlobal();
    if (!g_d3d12Device) {
        spdlog::error(
            "[Graphics] Could not resolve the RDR3 D3D12 device global");
    }
    g_d3d12CommandQueue = ResolveD3D12CommandQueueGlobal();
    if (!g_d3d12CommandQueue) {
        spdlog::error(
            "[Graphics] Could not resolve the RDR3 D3D12 direct queue global");
    }
    ResolveVulkanGlobals();
    if (!g_vulkanDevice || !g_vulkanQueue) {
        spdlog::debug(
            "[Graphics] Vulkan globals are unavailable; the vkCreateDevice "
            "capture path remains active");
    }

    if (!endDraw.IsValid() || !setDepthStencil.IsValid() ||
        !setRenderTargets.IsValid() || !swapchainBackbuffer.IsValid() ||
        !sgaDriver.IsValid()) {
        spdlog::error(
            "[Graphics] Could not resolve the RDR3 post-frontend render path "
            "(endDraw={}, setDepthStencil={}, setRenderTargets={}, "
            "swapchainBackbuffer={}, sgaDriver={})",
            endDraw.IsValid(), setDepthStencil.IsValid(),
            setRenderTargets.IsValid(), swapchainBackbuffer.IsValid(),
            sgaDriver.IsValid());
        return;
    }

    g_setDepthStencil = (setDepthStencil + 9).GetCall<SetDepthStencil>();
    g_setRenderTargets =
        (setRenderTargets - 0x13).Get<SetRenderTargets>();
    g_swapchainBackbufferVtableOffset =
        *reinterpret_cast<std::uint32_t*>(
            swapchainBackbuffer.Get<std::uint8_t*>() - 24);
    g_sgaDriver =
        (sgaDriver + 14).GetOffset().Get<std::uint64_t**>();

    const bool installed =
        (endDraw - 0x1D).Detour(InvokePostFrontend, &g_originalEndDraw);
    if (!installed) {
        spdlog::error("[Graphics] Failed to install the post-frontend hook");
        return;
    }

    g_postFrontendReady.store(true, std::memory_order_release);
    spdlog::info("[Graphics] Post-frontend render layer initialized");
});

} // namespace

void SetPostFrontendCallback(PostFrontendCallback callback)
{
    g_postFrontendCallback.store(callback, std::memory_order_release);
}

void SetPostFrontendSubmittedCallback(
    PostFrontendSubmittedCallback callback)
{
    g_postFrontendSubmittedCallback.store(callback,
                                           std::memory_order_release);
}

bool IsPostFrontendReady()
{
    return g_postFrontendReady.load(std::memory_order_acquire);
}

ID3D12Device* GetD3D12Device()
{
    return g_d3d12Device ? *g_d3d12Device : nullptr;
}

ID3D12CommandQueue* GetD3D12CommandQueue()
{
    if (!g_d3d12CommandQueue || !*g_d3d12CommandQueue) return nullptr;

    ID3D12CommandQueue* candidate = *g_d3d12CommandQueue;
    ID3D12CommandQueue* validated =
        g_validatedD3D12CommandQueue.load(std::memory_order_acquire);
    if (validated == candidate) return validated;

    Microsoft::WRL::ComPtr<ID3D12CommandQueue> queriedQueue;
    const HRESULT result = candidate->QueryInterface(IID_PPV_ARGS(&queriedQueue));
    if (FAILED(result) ||
        queriedQueue->GetDesc().Type != D3D12_COMMAND_LIST_TYPE_DIRECT) {
        if (!g_invalidD3D12CommandQueueLogged.exchange(
                true, std::memory_order_acq_rel)) {
            spdlog::error(
                "[Graphics] Resolved RDR3 graphics object is not a D3D12 "
                "direct command queue (0x{:08X})",
                static_cast<unsigned int>(result));
        }
        return nullptr;
    }

    // Retain one reference for the process lifetime. The game owns the queue,
    // but keeping our validated interface stable avoids racing shutdown.
    validated = queriedQueue.Detach();
    g_validatedD3D12CommandQueue.store(validated,
                                        std::memory_order_release);
    g_invalidD3D12CommandQueueLogged.store(false,
                                            std::memory_order_release);
    return validated;
}

GraphicsBackend GetGraphicsBackend()
{
    if (GetVulkanDevice() && GetVulkanQueue())
        return GraphicsBackend::Vulkan;
    if (GetD3D12Device() && GetD3D12CommandQueue())
        return GraphicsBackend::D3D12;
    return GraphicsBackend::Unknown;
}

void* GetVulkanDevice()
{
    {
        std::scoped_lock lock(g_vulkanDeviceRecordsMutex);
        if (!g_vulkanDeviceRecords.empty())
            return g_vulkanDeviceRecords.back().device;
    }
    return g_vulkanDevice && *g_vulkanDevice ? *g_vulkanDevice : nullptr;
}

void* GetVulkanQueue()
{
    {
        std::scoped_lock lock(g_vulkanDeviceRecordsMutex);
        for (auto record = g_vulkanDeviceRecords.rbegin();
             record != g_vulkanDeviceRecords.rend(); ++record) {
            if (record->graphicsQueue != VK_NULL_HANDLE)
                return record->graphicsQueue;
        }
    }
    return g_vulkanQueue && *g_vulkanQueue ? *g_vulkanQueue : nullptr;
}

bool GetGraphicsAdapterLuid(LUID& adapterLuid)
{
    if (const auto currentDevice =
            reinterpret_cast<VkDevice>(GetVulkanDevice())) {
        std::scoped_lock lock(g_vulkanDeviceRecordsMutex);
        const auto record = std::ranges::find_if(
            g_vulkanDeviceRecords,
            [currentDevice](const VulkanDeviceRecord& candidate) {
                return candidate.device == currentDevice;
            });
        if (record != g_vulkanDeviceRecords.end() &&
            record->hasAdapterLuid) {
            adapterLuid = record->adapterLuid;
            return true;
        }
    }

    if (!GetVulkanDevice()) {
        if (ID3D12Device* device = GetD3D12Device()) {
            adapterLuid = device->GetAdapterLuid();
            return true;
        }
    }

    Microsoft::WRL::ComPtr<IDXGIFactory6> factory;
    if (FAILED(CreateDXGIFactory1(IID_PPV_ARGS(&factory)))) return false;

    for (UINT index = 0;; ++index) {
        Microsoft::WRL::ComPtr<IDXGIAdapter1> adapter;
        const HRESULT result = factory->EnumAdapterByGpuPreference(
            index, DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE,
            IID_PPV_ARGS(&adapter));
        if (result == DXGI_ERROR_NOT_FOUND) break;
        if (FAILED(result)) return false;

        DXGI_ADAPTER_DESC1 description{};
        if (FAILED(adapter->GetDesc1(&description)) ||
            (description.Flags & DXGI_ADAPTER_FLAG_SOFTWARE) != 0) {
            continue;
        }
        adapterLuid = description.AdapterLuid;
        return true;
    }
    return false;
}

bool StageVulkanProducerWait(void* semaphore, std::uint64_t value)
{
    if (!g_originalVkQueueSubmit || !semaphore || value == 0) return false;
    std::scoped_lock lock(g_vulkanSubmitMutex);
    g_pendingVulkanProducerWait = {
        reinterpret_cast<VkSemaphore>(semaphore), value};
    return true;
}

bool StageVulkanConsumerSignal(void* semaphore, std::uint64_t value)
{
    if (!g_originalVkQueueSubmit || !semaphore || value == 0) return false;
    std::scoped_lock lock(g_vulkanSubmitMutex);
    const auto vkSemaphore = reinterpret_cast<VkSemaphore>(semaphore);
    if (g_pendingVulkanConsumerSignal.semaphore == VK_NULL_HANDLE) {
        // Never allow a submit which was already in flight when the render
        // thread staged this signal to release the capture surface. Skipping
        // one intercepted RAGE submit makes the signal conservatively trail
        // the WebView draw even when submission and rendering overlap.
        g_pendingVulkanConsumerSignal = {vkSemaphore, value, 1};
    } else {
        g_pendingVulkanConsumerSignal.semaphore = vkSemaphore;
        g_pendingVulkanConsumerSignal.value =
            (std::max)(g_pendingVulkanConsumerSignal.value, value);
    }
    return true;
}

} // namespace rdr2::graphics
