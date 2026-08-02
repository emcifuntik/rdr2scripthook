#include "stdafx.h"

#include "RageUiRenderer.h"

#include "PostFrontendRenderer.h"
#include "rage/CSysAllocator.h"
#include "webview/WebViewHost.h"

#include <d3d12.h>
#include <wrl/client.h>

#define VK_USE_PLATFORM_WIN32_KHR
#define VK_NO_PROTOTYPES
#include <vulkan/vulkan.h>

#include <atomic>
#include <bit>
#include <chrono>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <limits>
#include <vector>

namespace rdr2::graphics {
namespace {

struct RageTexture {
    virtual ~RageTexture() = default;
};

// RDR3's TextureVK stores this separately allocated object at byte offset 72.
// Its destructor owns both handles and releases the block with the game
// allocator, so imported images must use the same layout and allocator.
struct VulkanImageData {
    VkDeviceMemory memory = VK_NULL_HANDLE;
    VkImage image = VK_NULL_HANDLE;
    std::uint32_t padding[24]{};
};
static_assert(sizeof(VulkanImageData) == 112);

enum class BufferFormat : std::uint8_t {
    B8G8R8A8Unorm = 87,
};

struct ImageParams {
    std::uint16_t width = 0;
    std::uint16_t height = 0;
    std::uint16_t depth = 1;
    std::uint8_t dimension = 1;
    BufferFormat bufferFormat = BufferFormat::B8G8R8A8Unorm;
    std::uint8_t tileMode = 0xFF;
    std::uint8_t antiAliasType = 0;
    std::uint8_t levels = 1;
    bool unknownFlag = false;
    std::uint8_t unknown12 = 0;
    std::uint8_t unknown13 = 0;
};
static_assert(sizeof(ImageParams) == 14);

enum class BufferType : int {
    Image = 2,
};

struct TextureViewDesc {
    BufferType bufferType = BufferType::Image;
    int mostDetailedMip = 0;
    int mipLevels = 1;
    int arrayStart = 0;
    int arraySize = 1;
    float minLodClamp = 0.0f;
    bool unknownSingleChannel = false;
    std::uint8_t dimension = 4;
    std::uint8_t formatOverride = 0;
};
static_assert(sizeof(TextureViewDesc) == 28);

struct RageViewport {
    std::byte storage[1752];
};

using CreateTexture = RageTexture* (*)(
    const char* name, const ImageParams& params, int bufferType,
    std::uint32_t flags1, void* memoryInfo, std::uint32_t flags2,
    int cpuAccessType, void* clearValue, const void* conversionInfo,
    RageTexture* other);
using DrawMatrixConstructor = void (*)(RageViewport* viewport);
using ActivateMatrix = void (*)(RageViewport* viewport, bool active);
using SetScreenSpaceMatrix = void (*)(void*, void*, bool);
using GetShaderFunction = int (*)(std::intptr_t shader, const char* name);
using BeginShaderDraw = void (*)(std::intptr_t shader, int, int,
                                 std::int32_t function);
using BeginTechnique = void (*)(std::intptr_t shader, void* graphicsContext,
                                int technique);
using PopShader = void (*)();
using SetImParameters = void (*)(const float*, const float*);
using SetTexture = void (*)(RageTexture* texture);
using BeginVertices = void (*)(int drawMode, int count, int unknown);
using AddVertex = void (*)(float x, float y, float z, float normalX,
                           float normalY, float normalZ, std::uint32_t color,
                           float u, float v);
using DrawVertices = void (*)();

std::atomic_bool g_ready = false;
CreateTexture g_createTexture = nullptr;
DrawMatrixConstructor g_drawMatrixConstructor = nullptr;
ActivateMatrix g_activateMatrix = nullptr;
SetScreenSpaceMatrix g_setScreenSpaceMatrix = nullptr;
GetShaderFunction g_getShaderFunction = nullptr;
BeginShaderDraw g_beginShaderDraw = nullptr;
BeginTechnique g_beginTechnique = nullptr;
PopShader g_popShader = nullptr;
SetImParameters g_setImParameters = nullptr;
SetTexture g_setTexture = nullptr;
BeginVertices g_beginVertices = nullptr;
AddVertex g_addVertex = nullptr;
DrawVertices g_drawVertices = nullptr;

std::intptr_t* g_imShader = nullptr;
std::uint64_t** g_sgaDriver = nullptr;
std::uint32_t g_rasterStateOffset = 0;
std::uint32_t g_stateDirtyBitsOffset = 0;
std::uint32_t g_defaultStateOffset = 0;
std::uint16_t* g_noCullingState = nullptr;
std::uint16_t* g_defaultBlendState = nullptr;
std::uint16_t* g_noDepthState = nullptr;

RageViewport* g_imViewport = nullptr;
struct ImportedSurface {
    RageTexture* texture = nullptr;
    HANDLE sharedHandle = nullptr;
};
std::vector<ImportedSurface> g_importedSurfaces;
std::uint64_t g_importedGeneration = 0;
bool g_importFailureLogged = false;

Microsoft::WRL::ComPtr<ID3D12Fence> g_producerFence;
Microsoft::WRL::ComPtr<ID3D12Fence> g_consumerFence;
VkSemaphore g_vulkanProducerSemaphore = VK_NULL_HANDLE;
VkSemaphore g_vulkanConsumerSemaphore = VK_NULL_HANDLE;
HANDLE g_producerFenceHandle = nullptr;
HANDLE g_consumerFenceHandle = nullptr;
std::uint64_t g_lastProducerWait = 0;
std::uint64_t g_nextConsumerSignal = 0;
std::shared_ptr<const webview::CapturedFrame> g_pendingFrame;
std::vector<std::shared_ptr<const webview::CapturedFrame>> g_failedFrameHolds;
bool g_syncFailureLogged = false;
bool g_syncOpenedLogged = false;
bool g_producerWaitLogged = false;
bool g_surfaceImportedLogged = false;
bool g_drawRecordedLogged = false;
bool g_consumerSignalLogged = false;

std::uint64_t g_renderCalls = 0;
std::uint64_t g_freshFrames = 0;
std::uint64_t g_repeatedFrames = 0;
std::uint64_t g_lastRenderedSequence = 0;
auto g_renderStatsStarted = std::chrono::steady_clock::now();

struct VulkanFunctions {
    VkDevice device = VK_NULL_HANDLE;
    PFN_vkGetDeviceProcAddr getDeviceProcAddr = nullptr;
    PFN_vkCreateImage createImage = nullptr;
    PFN_vkDestroyImage destroyImage = nullptr;
    PFN_vkGetImageMemoryRequirements getImageMemoryRequirements = nullptr;
    PFN_vkAllocateMemory allocateMemory = nullptr;
    PFN_vkFreeMemory freeMemory = nullptr;
    PFN_vkBindImageMemory2 bindImageMemory2 = nullptr;
    PFN_vkGetMemoryWin32HandlePropertiesKHR
        getMemoryWin32HandleProperties = nullptr;
    PFN_vkCreateSemaphore createSemaphore = nullptr;
    PFN_vkDestroySemaphore destroySemaphore = nullptr;
    PFN_vkImportSemaphoreWin32HandleKHR importSemaphoreWin32Handle = nullptr;
    PFN_vkQueueWaitIdle queueWaitIdle = nullptr;
};
VulkanFunctions g_vulkan;
const char* g_vulkanFailureStage = "not attempted";
VkResult g_vulkanFailureResult = VK_SUCCESS;

void RecordVulkanFailure(const char* stage, VkResult result)
{
    g_vulkanFailureStage = stage;
    g_vulkanFailureResult = result;
}

template <typename Function>
Function VulkanDeviceFunction(VkDevice device, const char* name)
{
    return reinterpret_cast<Function>(
        g_vulkan.getDeviceProcAddr(device, name));
}

bool EnsureVulkanFunctions()
{
    const auto device = reinterpret_cast<VkDevice>(GetVulkanDevice());
    if (device == VK_NULL_HANDLE) return false;
    if (g_vulkan.device == device && g_vulkan.queueWaitIdle) return true;

    g_vulkan = {};
    HMODULE loader = GetModuleHandleW(L"vulkan-1.dll");
    if (!loader) {
        RecordVulkanFailure("load vulkan-1.dll", VK_ERROR_INITIALIZATION_FAILED);
        return false;
    }
    g_vulkan.getDeviceProcAddr = reinterpret_cast<PFN_vkGetDeviceProcAddr>(
        GetProcAddress(loader, "vkGetDeviceProcAddr"));
    if (!g_vulkan.getDeviceProcAddr) {
        RecordVulkanFailure("resolve vkGetDeviceProcAddr",
                            VK_ERROR_INITIALIZATION_FAILED);
        return false;
    }

    g_vulkan.device = device;
    g_vulkan.createImage = VulkanDeviceFunction<PFN_vkCreateImage>(
        device, "vkCreateImage");
    g_vulkan.destroyImage = VulkanDeviceFunction<PFN_vkDestroyImage>(
        device, "vkDestroyImage");
    g_vulkan.getImageMemoryRequirements =
        VulkanDeviceFunction<PFN_vkGetImageMemoryRequirements>(
            device, "vkGetImageMemoryRequirements");
    g_vulkan.allocateMemory = VulkanDeviceFunction<PFN_vkAllocateMemory>(
        device, "vkAllocateMemory");
    g_vulkan.freeMemory = VulkanDeviceFunction<PFN_vkFreeMemory>(
        device, "vkFreeMemory");
    g_vulkan.bindImageMemory2 = VulkanDeviceFunction<PFN_vkBindImageMemory2>(
        device, "vkBindImageMemory2");
    g_vulkan.getMemoryWin32HandleProperties = VulkanDeviceFunction<
        PFN_vkGetMemoryWin32HandlePropertiesKHR>(
        device, "vkGetMemoryWin32HandlePropertiesKHR");
    g_vulkan.createSemaphore = VulkanDeviceFunction<PFN_vkCreateSemaphore>(
        device, "vkCreateSemaphore");
    g_vulkan.destroySemaphore =
        VulkanDeviceFunction<PFN_vkDestroySemaphore>(
            device, "vkDestroySemaphore");
    g_vulkan.importSemaphoreWin32Handle = VulkanDeviceFunction<
        PFN_vkImportSemaphoreWin32HandleKHR>(
        device, "vkImportSemaphoreWin32HandleKHR");
    g_vulkan.queueWaitIdle = VulkanDeviceFunction<PFN_vkQueueWaitIdle>(
        device, "vkQueueWaitIdle");

    const bool complete =
        g_vulkan.createImage && g_vulkan.destroyImage &&
        g_vulkan.getImageMemoryRequirements && g_vulkan.allocateMemory &&
        g_vulkan.freeMemory && g_vulkan.bindImageMemory2 &&
        g_vulkan.createSemaphore && g_vulkan.destroySemaphore &&
        g_vulkan.importSemaphoreWin32Handle && g_vulkan.queueWaitIdle;
    if (!complete) {
        RecordVulkanFailure("resolve Vulkan external-sharing functions",
                            VK_ERROR_EXTENSION_NOT_PRESENT);
    }
    return complete;
}

template <typename Function>
Function DriverFunction(std::size_t byteOffset)
{
    if (!g_sgaDriver || !*g_sgaDriver) return nullptr;
    auto* vtable = *reinterpret_cast<std::uintptr_t**>(*g_sgaDriver);
    if (!vtable) return nullptr;
    return reinterpret_cast<Function>(
        *reinterpret_cast<std::uintptr_t*>(
            reinterpret_cast<std::uint8_t*>(vtable) + byteOffset));
}

void DestroyTextures()
{
    if (!g_importedSurfaces.empty() &&
        GetGraphicsBackend() == GraphicsBackend::Vulkan &&
        EnsureVulkanFunctions()) {
        const auto queue = reinterpret_cast<VkQueue>(GetVulkanQueue());
        if (queue != VK_NULL_HANDLE) g_vulkan.queueWaitIdle(queue);
    }
    for (auto& surface : g_importedSurfaces)
        delete surface.texture;
    g_importedSurfaces.clear();
    g_importedGeneration = 0;
}

void ResetSynchronization()
{
    if ((g_vulkanProducerSemaphore != VK_NULL_HANDLE ||
         g_vulkanConsumerSemaphore != VK_NULL_HANDLE) &&
        EnsureVulkanFunctions()) {
        const auto queue = reinterpret_cast<VkQueue>(GetVulkanQueue());
        if (queue != VK_NULL_HANDLE) g_vulkan.queueWaitIdle(queue);
        if (g_vulkanProducerSemaphore != VK_NULL_HANDLE) {
            g_vulkan.destroySemaphore(
                g_vulkan.device, g_vulkanProducerSemaphore, nullptr);
        }
        if (g_vulkanConsumerSemaphore != VK_NULL_HANDLE) {
            g_vulkan.destroySemaphore(
                g_vulkan.device, g_vulkanConsumerSemaphore, nullptr);
        }
    }
    g_vulkanProducerSemaphore = VK_NULL_HANDLE;
    g_vulkanConsumerSemaphore = VK_NULL_HANDLE;
    g_pendingFrame.reset();
    g_failedFrameHolds.clear();
    g_producerFence.Reset();
    g_consumerFence.Reset();
    g_producerFenceHandle = nullptr;
    g_consumerFenceHandle = nullptr;
    g_lastProducerWait = 0;
    g_nextConsumerSignal = 0;
    g_syncFailureLogged = false;
}

bool EnsureD3D12Synchronization(
    const std::shared_ptr<const webview::CapturedFrame>& frame)
{
    if (!frame || !frame->ProducerFenceHandle() ||
        !frame->ConsumerFenceHandle()) {
        return false;
    }
    if (g_producerFence && g_consumerFence &&
        g_producerFenceHandle == frame->ProducerFenceHandle() &&
        g_consumerFenceHandle == frame->ConsumerFenceHandle()) {
        return true;
    }

    ResetSynchronization();
    ID3D12Device* device = GetD3D12Device();
    if (!device) return false;

    HRESULT result = device->OpenSharedHandle(
        frame->ProducerFenceHandle(), IID_PPV_ARGS(&g_producerFence));
    if (FAILED(result)) return false;
    result = device->OpenSharedHandle(
        frame->ConsumerFenceHandle(), IID_PPV_ARGS(&g_consumerFence));
    if (FAILED(result)) {
        g_producerFence.Reset();
        return false;
    }

    g_producerFenceHandle = frame->ProducerFenceHandle();
    g_consumerFenceHandle = frame->ConsumerFenceHandle();
    const std::uint64_t completedValue =
        g_consumerFence->GetCompletedValue();
    if (completedValue == UINT64_MAX) {
        ResetSynchronization();
        return false;
    }
    g_nextConsumerSignal = completedValue;
    if (!g_syncOpenedLogged) {
        g_syncOpenedLogged = true;
        spdlog::info("[Graphics] WebView shared fences opened in D3D12");
    }
    return true;
}

bool CreateImportedVulkanSemaphore(HANDLE sharedHandle,
                                   VkSemaphore& semaphore)
{
    VkSemaphoreCreateInfo createInfo{
        VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO};
    VkResult result = g_vulkan.createSemaphore(
        g_vulkan.device, &createInfo, nullptr, &semaphore);
    if (result != VK_SUCCESS) {
        RecordVulkanFailure("vkCreateSemaphore", result);
        return false;
    }

    VkImportSemaphoreWin32HandleInfoKHR importInfo{
        VK_STRUCTURE_TYPE_IMPORT_SEMAPHORE_WIN32_HANDLE_INFO_KHR};
    importInfo.semaphore = semaphore;
    importInfo.handleType =
        VK_EXTERNAL_SEMAPHORE_HANDLE_TYPE_D3D11_FENCE_BIT;
    importInfo.handle = sharedHandle;
    result = g_vulkan.importSemaphoreWin32Handle(
        g_vulkan.device, &importInfo);
    if (result == VK_SUCCESS) return true;

    RecordVulkanFailure("vkImportSemaphoreWin32HandleKHR", result);
    g_vulkan.destroySemaphore(g_vulkan.device, semaphore, nullptr);
    semaphore = VK_NULL_HANDLE;
    return false;
}

bool EnsureVulkanSynchronization(
    const std::shared_ptr<const webview::CapturedFrame>& frame)
{
    if (!frame || !frame->ProducerFenceHandle() ||
        !frame->ConsumerFenceHandle() || !EnsureVulkanFunctions()) {
        return false;
    }
    if (g_vulkanProducerSemaphore != VK_NULL_HANDLE &&
        g_vulkanConsumerSemaphore != VK_NULL_HANDLE &&
        g_producerFenceHandle == frame->ProducerFenceHandle() &&
        g_consumerFenceHandle == frame->ConsumerFenceHandle()) {
        return true;
    }

    ResetSynchronization();
    if (!CreateImportedVulkanSemaphore(frame->ProducerFenceHandle(),
                                       g_vulkanProducerSemaphore) ||
        !CreateImportedVulkanSemaphore(frame->ConsumerFenceHandle(),
                                       g_vulkanConsumerSemaphore)) {
        ResetSynchronization();
        return false;
    }

    g_producerFenceHandle = frame->ProducerFenceHandle();
    g_consumerFenceHandle = frame->ConsumerFenceHandle();
    if (!g_syncOpenedLogged) {
        g_syncOpenedLogged = true;
        spdlog::info(
            "[Graphics] WebView shared fences imported into Vulkan");
    }
    return true;
}

bool EnsureSynchronization(
    const std::shared_ptr<const webview::CapturedFrame>& frame)
{
    switch (GetGraphicsBackend()) {
        case GraphicsBackend::D3D12:
            return EnsureD3D12Synchronization(frame);
        case GraphicsBackend::Vulkan:
            return EnsureVulkanSynchronization(frame);
        default:
            return false;
    }
}

bool EnqueueProducerWait(
    const std::shared_ptr<const webview::CapturedFrame>& frame)
{
    if (frame->ProducerFenceValue() <= g_lastProducerWait) return true;

    bool succeeded = false;
    VkResult vulkanResult = VK_ERROR_INITIALIZATION_FAILED;
    switch (GetGraphicsBackend()) {
        case GraphicsBackend::D3D12: {
            ID3D12CommandQueue* queue = GetD3D12CommandQueue();
            succeeded = queue && g_producerFence &&
                        SUCCEEDED(queue->Wait(
                            g_producerFence.Get(),
                            frame->ProducerFenceValue()));
            break;
        }
        case GraphicsBackend::Vulkan: {
            succeeded = StageVulkanProducerWait(
                reinterpret_cast<void*>(g_vulkanProducerSemaphore),
                frame->ProducerFenceValue());
            vulkanResult = succeeded ? VK_SUCCESS
                                     : VK_ERROR_INITIALIZATION_FAILED;
            break;
        }
        default:
            break;
    }
    if (!succeeded) {
        if (GetGraphicsBackend() == GraphicsBackend::Vulkan)
            RecordVulkanFailure("vkQueueSubmit(producer wait)",
                                vulkanResult);
        return false;
    }

    g_lastProducerWait = frame->ProducerFenceValue();
    if (!g_producerWaitLogged) {
        g_producerWaitLogged = true;
        spdlog::info(
            "[Graphics] WebView producer-fence wait enqueued ({})",
            GetGraphicsBackend() == GraphicsBackend::Vulkan ? "Vulkan"
                                                             : "D3D12");
    }
    return true;
}

void ReportRenderPacing(std::uint64_t sequence)
{
    const auto now = std::chrono::steady_clock::now();
    if (g_renderCalls == 0) g_renderStatsStarted = now;
    ++g_renderCalls;
    if (sequence != g_lastRenderedSequence) {
        ++g_freshFrames;
        g_lastRenderedSequence = sequence;
    } else {
        ++g_repeatedFrames;
    }

    const double seconds =
        std::chrono::duration<double>(now - g_renderStatsStarted).count();
    if (seconds < 5.0) return;

    spdlog::debug(
        "[Graphics] WebView pacing: {:.1f} draws/s, {:.1f} fresh/s, "
        "{} repeated",
        static_cast<double>(g_renderCalls) / seconds,
        static_cast<double>(g_freshFrames) / seconds, g_repeatedFrames);
    g_renderCalls = 0;
    g_freshFrames = 0;
    g_repeatedFrames = 0;
    g_renderStatsStarted = now;
}

RageTexture* CreateEmptyRageTexture(
    const std::shared_ptr<const webview::CapturedFrame>& frame)
{
    ImageParams params;
    params.width = static_cast<std::uint16_t>(frame->Width());
    params.height = static_cast<std::uint16_t>(frame->Height());
    RageTexture* texture = g_createTexture(
        "rdr2_webview", params, 0, 2, nullptr, 8, 0, nullptr, nullptr,
        nullptr);
    if (!texture) return nullptr;

    using DestroyDriverTexture = void (*)(std::uint64_t*, RageTexture*);
    const auto destroyDriverTexture =
        DriverFunction<DestroyDriverTexture>(440);
    if (!destroyDriverTexture) {
        delete texture;
        return nullptr;
    }

    destroyDriverTexture(*g_sgaDriver, texture);
    // Both TextureD3D12::resource and TextureVK::image occupy this slot.
    *reinterpret_cast<void**>(
        reinterpret_cast<std::byte*>(texture) + 72) = nullptr;
    return texture;
}

void CreateTextureShaderResourceView(RageTexture* texture)
{
    using CreateShaderResourceView = void (*)(
        std::uint64_t*, void*, RageTexture*, const TextureViewDesc*);
    const auto createShaderResourceView =
        DriverFunction<CreateShaderResourceView>(256);
    if (!createShaderResourceView) return;

    TextureViewDesc view;
    void* driverTexture = *reinterpret_cast<void**>(
        reinterpret_cast<std::byte*>(texture) + 48);
    createShaderResourceView(
        *g_sgaDriver, driverTexture, texture, &view);
}

RageTexture* ImportD3D12Texture(
    const std::shared_ptr<const webview::CapturedFrame>& frame)
{
    ID3D12Device* device = GetD3D12Device();
    if (!device) return nullptr;

    ID3D12Resource* resource = nullptr;
    const HRESULT result = device->OpenSharedHandle(
        frame->SharedHandle(), IID_PPV_ARGS(&resource));
    if (FAILED(result) || !resource) return nullptr;

    RageTexture* texture = CreateEmptyRageTexture(frame);
    if (!texture) {
        resource->Release();
        return nullptr;
    }

    // TextureD3D12 is a virtual Texture followed by 64 bytes of SGA state.
    // The native resource field is therefore at byte offset 72.
    *reinterpret_cast<ID3D12Resource**>(
        reinterpret_cast<std::byte*>(texture) + 72) = resource;
    CreateTextureShaderResourceView(texture);
    return texture;
}

RageTexture* ImportVulkanTexture(
    const std::shared_ptr<const webview::CapturedFrame>& frame)
{
    if (!EnsureVulkanFunctions()) return nullptr;

    VkExternalMemoryImageCreateInfo externalInfo{
        VK_STRUCTURE_TYPE_EXTERNAL_MEMORY_IMAGE_CREATE_INFO};
    externalInfo.handleTypes =
        VK_EXTERNAL_MEMORY_HANDLE_TYPE_D3D11_TEXTURE_BIT;

    VkImageCreateInfo imageInfo{VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO};
    imageInfo.pNext = &externalInfo;
    imageInfo.imageType = VK_IMAGE_TYPE_2D;
    imageInfo.format = VK_FORMAT_B8G8R8A8_UNORM;
    imageInfo.extent = {frame->Width(), frame->Height(), 1};
    imageInfo.mipLevels = 1;
    imageInfo.arrayLayers = 1;
    imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
    imageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
    imageInfo.usage = VK_IMAGE_USAGE_SAMPLED_BIT;
    imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;

    VkImage image = VK_NULL_HANDLE;
    VkResult result = g_vulkan.createImage(
        g_vulkan.device, &imageInfo, nullptr, &image);
    if (result != VK_SUCCESS) {
        RecordVulkanFailure("vkCreateImage", result);
        return nullptr;
    }

    VkMemoryRequirements requirements{};
    g_vulkan.getImageMemoryRequirements(
        g_vulkan.device, image, &requirements);
    std::uint32_t compatibleMemoryTypes = requirements.memoryTypeBits;
    if (g_vulkan.getMemoryWin32HandleProperties) {
        VkMemoryWin32HandlePropertiesKHR handleProperties{
            VK_STRUCTURE_TYPE_MEMORY_WIN32_HANDLE_PROPERTIES_KHR};
        if (g_vulkan.getMemoryWin32HandleProperties(
                g_vulkan.device,
                VK_EXTERNAL_MEMORY_HANDLE_TYPE_D3D11_TEXTURE_BIT,
                frame->SharedHandle(), &handleProperties) == VK_SUCCESS) {
            compatibleMemoryTypes &= handleProperties.memoryTypeBits;
        }
    }
    if (compatibleMemoryTypes == 0) {
        RecordVulkanFailure("find imported image memory type",
                            VK_ERROR_INVALID_EXTERNAL_HANDLE);
        g_vulkan.destroyImage(g_vulkan.device, image, nullptr);
        return nullptr;
    }

    VkMemoryDedicatedAllocateInfo dedicatedInfo{
        VK_STRUCTURE_TYPE_MEMORY_DEDICATED_ALLOCATE_INFO};
    dedicatedInfo.image = image;
    VkImportMemoryWin32HandleInfoKHR importInfo{
        VK_STRUCTURE_TYPE_IMPORT_MEMORY_WIN32_HANDLE_INFO_KHR};
    importInfo.pNext = &dedicatedInfo;
    importInfo.handleType =
        VK_EXTERNAL_MEMORY_HANDLE_TYPE_D3D11_TEXTURE_BIT;
    importInfo.handle = frame->SharedHandle();
    VkMemoryAllocateInfo allocationInfo{
        VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO};
    allocationInfo.pNext = &importInfo;
    allocationInfo.allocationSize = requirements.size;
    allocationInfo.memoryTypeIndex =
        std::countr_zero(compatibleMemoryTypes);

    VkDeviceMemory memory = VK_NULL_HANDLE;
    result = g_vulkan.allocateMemory(
        g_vulkan.device, &allocationInfo, nullptr, &memory);
    if (result != VK_SUCCESS) {
        RecordVulkanFailure("vkAllocateMemory(import D3D11 texture)", result);
        g_vulkan.destroyImage(g_vulkan.device, image, nullptr);
        return nullptr;
    }

    VkBindImageMemoryInfo bindInfo{
        VK_STRUCTURE_TYPE_BIND_IMAGE_MEMORY_INFO};
    bindInfo.image = image;
    bindInfo.memory = memory;
    result = g_vulkan.bindImageMemory2(
        g_vulkan.device, 1, &bindInfo);
    if (result != VK_SUCCESS) {
        RecordVulkanFailure("vkBindImageMemory2", result);
        g_vulkan.freeMemory(g_vulkan.device, memory, nullptr);
        g_vulkan.destroyImage(g_vulkan.device, image, nullptr);
        return nullptr;
    }

    RageTexture* texture = CreateEmptyRageTexture(frame);
    if (!texture) {
        g_vulkan.destroyImage(g_vulkan.device, image, nullptr);
        g_vulkan.freeMemory(g_vulkan.device, memory, nullptr);
        return nullptr;
    }

    auto* imageData = static_cast<VulkanImageData*>(
        CSysAllocator::Instance().Alloc(sizeof(VulkanImageData)));
    if (!imageData) {
        delete texture;
        g_vulkan.destroyImage(g_vulkan.device, image, nullptr);
        g_vulkan.freeMemory(g_vulkan.device, memory, nullptr);
        return nullptr;
    }
    std::memset(imageData, 0, sizeof(*imageData));
    imageData->memory = memory;
    imageData->image = image;
    *reinterpret_cast<VulkanImageData**>(
        reinterpret_cast<std::byte*>(texture) + 72) = imageData;
    CreateTextureShaderResourceView(texture);
    return texture;
}

RageTexture* ImportTexture(
    const std::shared_ptr<const webview::CapturedFrame>& frame)
{
    if (!frame || !frame->SharedHandle()) return nullptr;
    if (frame->Width() > std::numeric_limits<std::uint16_t>::max() ||
        frame->Height() > std::numeric_limits<std::uint16_t>::max()) {
        return nullptr;
    }

    RageTexture* texture = nullptr;
    switch (GetGraphicsBackend()) {
        case GraphicsBackend::D3D12:
            texture = ImportD3D12Texture(frame);
            break;
        case GraphicsBackend::Vulkan:
            texture = ImportVulkanTexture(frame);
            break;
        default:
            return nullptr;
    }
    if (!texture) return nullptr;

    g_importFailureLogged = false;
    if (!g_surfaceImportedLogged) {
        g_surfaceImportedLogged = true;
        spdlog::info(
            "[Graphics] WebView shared surface imported into RAGE ({})",
            GetGraphicsBackend() == GraphicsBackend::Vulkan ? "Vulkan"
                                                             : "D3D12");
    }
    return texture;
}

std::uint32_t GetRasterizerState(void* context)
{
    return *reinterpret_cast<std::uint8_t*>(
        static_cast<std::byte*>(context) + g_rasterStateOffset);
}

std::uint32_t GetBlendState(void* context)
{
    return *reinterpret_cast<std::uint16_t*>(
        static_cast<std::byte*>(context) + g_rasterStateOffset + 12);
}

std::uint32_t GetDepthState(void* context)
{
    return *reinterpret_cast<std::uint16_t*>(
        static_cast<std::byte*>(context) + g_rasterStateOffset + 2);
}

void SetRasterizerState(void* context, std::uint32_t state)
{
    if (GetRasterizerState(context) == static_cast<std::uint8_t>(state))
        return;
    auto* bytes = static_cast<std::byte*>(context);
    auto& dirty = *reinterpret_cast<std::uint32_t*>(
        bytes + g_stateDirtyBitsOffset);
    if (*reinterpret_cast<std::uint8_t*>(
            bytes + g_defaultStateOffset) ==
        static_cast<std::uint8_t>(state)) {
        dirty &= ~4u;
    } else {
        dirty |= 4u;
    }
    *reinterpret_cast<std::uint8_t*>(bytes + g_rasterStateOffset) =
        static_cast<std::uint8_t>(state);
}

void SetBlendState(void* context, std::uint32_t state)
{
    if (GetBlendState(context) == state) return;
    auto* bytes = static_cast<std::byte*>(context);
    *reinterpret_cast<std::uint32_t*>(bytes + g_stateDirtyBitsOffset) |= 16u;
    *reinterpret_cast<std::uint8_t*>(
        bytes + g_rasterStateOffset + 12) = static_cast<std::uint8_t>(state);
}

void SetDepthState(void* context, std::uint32_t state)
{
    if (GetDepthState(context) == state) return;
    auto* bytes = static_cast<std::byte*>(context);
    auto& dirty = *reinterpret_cast<std::uint32_t*>(
        bytes + g_stateDirtyBitsOffset);
    if (*reinterpret_cast<std::uint8_t*>(
            bytes + g_defaultStateOffset + 2) ==
        static_cast<std::uint8_t>(state)) {
        dirty &= ~8u;
    } else {
        dirty |= 8u;
    }
    *reinterpret_cast<std::uint8_t*>(
        bytes + g_rasterStateOffset + 2) = static_cast<std::uint8_t>(state);
}

bool PushUiShader(void* graphicsContext)
{
    if (!g_imShader || !*g_imShader) return false;
    if (!g_imViewport) {
        g_imViewport = new RageViewport{};
        g_drawMatrixConstructor(g_imViewport);
    }

    g_activateMatrix(g_imViewport, true);
    g_setScreenSpaceMatrix(nullptr, nullptr, true);
    const int shaderFunction =
        g_getShaderFunction(*g_imShader, "rage_unlit_draw");
    g_beginShaderDraw(*g_imShader, 0, 0, shaderFunction);
    g_beginTechnique(*g_imShader, graphicsContext, 0);
    return true;
}

void BindTexture(RageTexture* texture)
{
    alignas(16) constexpr float scale[4] = {1.0f, 1.0f, 1.0f, 1.0f};
    alignas(16) constexpr float offset[4] = {0.0f, 0.0f, 0.0f, 0.0f};
    g_setImParameters(scale, offset);
    g_setTexture(texture);
}

CMemory::Hook g_resolveUiRenderer([] {
    constexpr CMemory::Pattern createTexturePattern(
        "8B 45 ? 89 44 24 28 48 8B 45 ? 48 89 44 24 20 E8");
    constexpr CMemory::Pattern drawMatrixPattern(
        "40 53 48 83 EC 30 66 C7 81 70 06 00 00 01 00 0F 57 C0");
    constexpr CMemory::Pattern activateMatrixPattern(
        "48 85 DB 74 ? 8A 83 71 06 00 00");
    constexpr CMemory::Pattern screenSpacePattern(
        "75 16 65 48 8B 0C 25 58 00 00 00 BB ? ? 00 00");
    constexpr CMemory::Pattern shaderFunctionPattern(
        "E8 ? ? ? ? 48 8B 0B 8B D0 48 83 C4 20");
    constexpr CMemory::Pattern beginShaderPattern(
        "41 8A D8 4C 8B D1 41 80 F9 FF");
    constexpr CMemory::Pattern beginTechniquePattern(
        "57 41 56 41 57 48 83 EC 20 4C 8B 19");
    constexpr CMemory::Pattern popShaderPattern(
        "48 83 EC 38 65 48 8B 04 25 58");
    constexpr CMemory::Pattern setImParametersPattern(
        "41 B9 10 00 00 00 F3 0F 7F 44 24 20");
    constexpr CMemory::Pattern setTexturePattern(
        "40 53 48 83 EC 20 8B 15 ? ? ? ? 48 8B D9 4C 8B C1 "
        "48 8B 0D ? ? ? ? E8 ? ? ? ? 48 8B CB E8");
    constexpr CMemory::Pattern imShaderPattern(
        "41 B9 10 00 00 00 48 8B 0D ? ? ? ? F3 0F 7F 44 24 20 E8");
    constexpr CMemory::Pattern beginVerticesPattern(
        "4C 39 92 ? ? 00 00 74 2E");
    constexpr CMemory::Pattern addVertexPattern(
        "48 83 EC 78 8B 05 ? ? ? ? 65 48");
    constexpr CMemory::Pattern drawVerticesPattern(
        "F3 44 0F 11 44 24 20 E8 ? ? ? ? E8");
    constexpr CMemory::Pattern stateOffsetsPattern(
        "3A D1 74 22 8B 83 ? ? ? ? 88 8B");
    constexpr CMemory::Pattern stockStatesPattern(
        "48 8D 4D BF 88 05 ? ? ? ? C6 45 BF 02");
    constexpr CMemory::Pattern sgaDriverPattern(
        "C6 82 ? ? 00 00 01 C6 82 ? ? 00 00 01 48 8B 0D");

    CMemory createTexture = createTexturePattern.Search();
    CMemory drawMatrix = drawMatrixPattern.Search();
    CMemory activateMatrix = activateMatrixPattern.Search();
    CMemory screenSpace = screenSpacePattern.Search();
    CMemory shaderFunction = shaderFunctionPattern.Search();
    CMemory beginShader = beginShaderPattern.Search();
    CMemory beginTechnique = beginTechniquePattern.Search();
    CMemory popShader = popShaderPattern.Search();
    CMemory setImParameters = setImParametersPattern.Search();
    CMemory setTexture = setTexturePattern.Search();
    CMemory imShader = imShaderPattern.Search();
    CMemory beginVertices = beginVerticesPattern.Search();
    CMemory addVertex = addVertexPattern.Search();
    CMemory drawVertices = drawVerticesPattern.Search();
    CMemory stateOffsets = stateOffsetsPattern.Search();
    CMemory stockStates = stockStatesPattern.Search();
    CMemory sgaDriver = sgaDriverPattern.Search();

    if (!createTexture.IsValid() || !drawMatrix.IsValid() ||
        !activateMatrix.IsValid() || !screenSpace.IsValid() ||
        !shaderFunction.IsValid() || !beginShader.IsValid() ||
        !beginTechnique.IsValid() || !popShader.IsValid() ||
        !setImParameters.IsValid() || !setTexture.IsValid() ||
        !imShader.IsValid() || !beginVertices.IsValid() ||
        !addVertex.IsValid() || !drawVertices.IsValid() ||
        !stateOffsets.IsValid() || !stockStates.IsValid() ||
        !sgaDriver.IsValid()) {
        spdlog::error("[Graphics] Could not resolve the RAGE UI renderer");
        return;
    }

    g_createTexture = (createTexture + 16).GetCall<CreateTexture>();
    g_drawMatrixConstructor = drawMatrix.Get<DrawMatrixConstructor>();
    g_activateMatrix = (activateMatrix - 0x32).Get<ActivateMatrix>();
    g_setScreenSpaceMatrix =
        (screenSpace - 0x1C).Get<SetScreenSpaceMatrix>();
    g_getShaderFunction =
        (shaderFunction - 0x11).Get<GetShaderFunction>();
    g_beginShaderDraw = (beginShader - 0xC).Get<BeginShaderDraw>();
    g_beginTechnique =
        (beginTechnique - 0xF).Get<BeginTechnique>();
    g_popShader = popShader.Get<PopShader>();
    g_setImParameters =
        (setImParameters - 0x1E).Get<SetImParameters>();
    g_setTexture = setTexture.Get<SetTexture>();
    g_imShader = (imShader + 6).GetOffset().Get<std::intptr_t*>();
    g_beginVertices =
        (beginVertices - 0x49).Get<BeginVertices>();
    g_addVertex = addVertex.Get<AddVertex>();
    g_drawVertices = (drawVertices + 12).GetCall<DrawVertices>();

    g_rasterStateOffset = *reinterpret_cast<std::uint32_t*>(
        (stateOffsets - 4).Get<std::uint8_t*>());
    g_stateDirtyBitsOffset = *reinterpret_cast<std::uint32_t*>(
        (stateOffsets + 6).Get<std::uint8_t*>());
    g_defaultStateOffset = *reinterpret_cast<std::uint32_t*>(
        (stateOffsets + 18).Get<std::uint8_t*>());
    g_noCullingState =
        (stockStates + 6).GetOffset(0).Get<std::uint16_t*>();
    g_noDepthState =
        (stockStates - 186).GetOffset(0).Get<std::uint16_t*>();
    g_defaultBlendState =
        (stockStates + 167).GetOffset(0).Get<std::uint16_t*>();
    g_sgaDriver =
        (sgaDriver + 14).GetOffset().Get<std::uint64_t**>();

    g_ready.store(true, std::memory_order_release);
    spdlog::info("[Graphics] RAGE WebView UI renderer resolved");
});

} // namespace

bool RageUiRenderer::IsReady()
{
    return g_ready.load(std::memory_order_acquire);
}

void RageUiRenderer::Render(
    void* graphicsContext,
    const std::shared_ptr<const webview::CapturedFrame>& frame)
{
    if (!IsReady() || !graphicsContext) return;
    if (!frame) {
        Reset();
        return;
    }

    if (g_importedGeneration != frame->Generation()) {
        DestroyTextures();
        // Resizing replaces the shared texture ring but not the two fence
        // objects. Keep their imported payloads and monotonically increasing
        // consumer value alive across capture generations.
        g_importedGeneration = frame->Generation();
    }

    if (!EnsureSynchronization(frame)) {
        if (!g_syncFailureLogged) {
            g_syncFailureLogged = true;
            if (GetGraphicsBackend() == GraphicsBackend::Vulkan) {
                spdlog::error(
                    "[Graphics] WebView Vulkan synchronization failed at "
                    "{} (VkResult={})",
                    g_vulkanFailureStage,
                    static_cast<int>(g_vulkanFailureResult));
            } else {
                spdlog::error(
                    "[Graphics] WebView shared-surface synchronization is "
                    "unavailable");
            }
        }
        return;
    }

    if (!EnqueueProducerWait(frame)) {
        if (!g_syncFailureLogged) {
            g_syncFailureLogged = true;
            spdlog::error(
                "[Graphics] Failed to enqueue the WebView producer wait "
                "for frame {}",
                frame->Sequence());
        }
        return;
    }
    g_syncFailureLogged = false;

    RageTexture* texture = nullptr;
    for (const auto& surface : g_importedSurfaces) {
        if (surface.sharedHandle == frame->SharedHandle()) {
            texture = surface.texture;
            break;
        }
    }
    if (!texture) {
        texture = ImportTexture(frame);
        if (!texture) {
            if (!g_importFailureLogged) {
                g_importFailureLogged = true;
                if (GetGraphicsBackend() == GraphicsBackend::Vulkan) {
                    spdlog::error(
                        "[Graphics] Failed to import WebView2's Vulkan "
                        "surface at {} (VkResult={})",
                        g_vulkanFailureStage,
                        static_cast<int>(g_vulkanFailureResult));
                } else {
                    spdlog::error(
                        "[Graphics] Failed to import WebView2's shared "
                        "surface");
                }
            }
            return;
        }
        g_importedSurfaces.push_back({texture, frame->SharedHandle()});
    }

    const std::uint32_t oldRaster = GetRasterizerState(graphicsContext);
    const std::uint32_t oldBlend = GetBlendState(graphicsContext);
    const std::uint32_t oldDepth = GetDepthState(graphicsContext);

    BindTexture(texture);
    SetRasterizerState(graphicsContext, *g_noCullingState);
    SetBlendState(graphicsContext, *g_defaultBlendState);
    SetDepthState(graphicsContext, *g_noDepthState);

    bool drewFrame = false;
    if (PushUiShader(graphicsContext)) {
        const float width = static_cast<float>(frame->Width());
        const float height = static_cast<float>(frame->Height());
        constexpr std::uint32_t color = 0xFFFFFFFF;
        g_beginVertices(4, 4, 0);
        g_addVertex(0.0f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f,
                    color, 0.0f, 0.0f);
        g_addVertex(width, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f,
                    color, 1.0f, 0.0f);
        g_addVertex(0.0f, height, 0.0f, 0.0f, 0.0f, -1.0f,
                    color, 0.0f, 1.0f);
        g_addVertex(width, height, 0.0f, 0.0f, 0.0f, -1.0f,
                    color, 1.0f, 1.0f);
        g_drawVertices();
        g_popShader();
        drewFrame = true;
    }

    SetRasterizerState(graphicsContext, oldRaster);
    SetBlendState(graphicsContext, oldBlend);
    SetDepthState(graphicsContext, oldDepth);

    if (drewFrame) {
        g_pendingFrame = frame;
        if (!g_drawRecordedLogged) {
            g_drawRecordedLogged = true;
            spdlog::info("[Graphics] First synchronized WebView draw recorded");
        }
        ReportRenderPacing(frame->Sequence());
    }
}

void RageUiRenderer::OnSubmitted()
{
    if (!g_pendingFrame) return;

    const std::uint64_t signalValue = ++g_nextConsumerSignal;
    bool succeeded = false;
    VkResult vulkanResult = VK_ERROR_INITIALIZATION_FAILED;
    switch (GetGraphicsBackend()) {
        case GraphicsBackend::D3D12: {
            ID3D12CommandQueue* queue = GetD3D12CommandQueue();
            succeeded = queue && g_consumerFence &&
                        SUCCEEDED(queue->Signal(
                            g_consumerFence.Get(), signalValue));
            break;
        }
        case GraphicsBackend::Vulkan: {
            succeeded = StageVulkanConsumerSignal(
                reinterpret_cast<void*>(g_vulkanConsumerSemaphore),
                signalValue);
            vulkanResult = succeeded ? VK_SUCCESS
                                     : VK_ERROR_INITIALIZATION_FAILED;
            break;
        }
        default:
            break;
    }

    if (!succeeded) {
        if (GetGraphicsBackend() == GraphicsBackend::Vulkan) {
            RecordVulkanFailure("vkQueueSubmit(consumer signal)",
                                vulkanResult);
            spdlog::error(
                "[Graphics] Failed to enqueue the Vulkan WebView consumer "
                "signal (VkResult={})",
                static_cast<int>(vulkanResult));
        } else {
            spdlog::error(
                "[Graphics] Failed to enqueue the WebView consumer-fence "
                "signal");
        }
        // Keep the descriptor alive so the capture thread cannot recycle a
        // texture whose GPU read completion is unknown.
        g_failedFrameHolds.push_back(std::move(g_pendingFrame));
        return;
    }

    g_pendingFrame->MarkSubmitted(signalValue);
    g_pendingFrame.reset();
    if (!g_consumerSignalLogged) {
        g_consumerSignalLogged = true;
        spdlog::info(
            "[Graphics] First WebView consumer-fence signal submitted ({})",
            GetGraphicsBackend() == GraphicsBackend::Vulkan ? "Vulkan"
                                                             : "D3D12");
    }
}

void RageUiRenderer::Reset()
{
    DestroyTextures();
    ResetSynchronization();
    g_importFailureLogged = false;
    g_renderCalls = 0;
    g_freshFrames = 0;
    g_repeatedFrames = 0;
    g_lastRenderedSequence = 0;
    g_renderStatsStarted = std::chrono::steady_clock::now();
}

} // namespace rdr2::graphics
