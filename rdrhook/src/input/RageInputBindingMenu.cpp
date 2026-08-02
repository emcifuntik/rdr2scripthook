#include "stdafx.h"

#include "InputBindingManager.h"
#include "rage/CSysAllocator.h"

#include <MinHook.h>
#include <jitasm.h>

#include <algorithm>
#include <array>
#include <atomic>
#include <charconv>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <memory>
#include <mutex>
#include <shared_mutex>
#include <string>
#include <unordered_map>
#include <vector>

namespace rdr2::input {
namespace {

constexpr char CategoryName[] = "PM_PANE_RDRHOOK";
constexpr char CategoryLabel[] = "Script Bindings";
constexpr char ControlNamePrefix[] = "INPUT_RDRHOOK_";
constexpr std::uint32_t UnboundParameter = 0xFF000;
// RDR2 1.0.1491.50 emits Device=0 for keyboard mapping expressions and passes
// the same value through mapping lookup, capture, and commit.
constexpr std::uint8_t KeyboardDevice = 0x00;
constexpr std::size_t NativeMappingCaptureOffset = 3960;
constexpr std::size_t NativeMappingCaptureControlIdOffset = 3968;
constexpr std::size_t NativeMappingCaptureMappingIndexOffset = 4016;
constexpr std::size_t NativeMappingCaptureSubIndexOffset = 4018;
constexpr std::size_t NativeMappingCaptureDeviceOffset = 4020;
constexpr std::size_t AliasCount =
    InputBindingManager::NativeControlIdLast -
    InputBindingManager::NativeControlIdFirst + 1;

struct EngineArray {
    std::uint32_t* data = nullptr;
    std::uint16_t count = 0;
    std::uint16_t capacity = 0;
    std::uint32_t reserved = 0;
};
static_assert(sizeof(EngineArray) == 16);

using GetCategoriesFn = std::int64_t(__fastcall*)(EngineArray*);
using GetCategoryControlsFn =
    std::int64_t(__fastcall*)(const std::uint32_t*, EngineArray*);
using GetBindingFn = void**(__fastcall*)(void*, void**, std::uint32_t,
                                         std::uint8_t, std::uint16_t, bool);
using GetControlNameFn = const char*(__fastcall*)(void*, std::int32_t);
using GetControlIdFn = std::uint32_t(__fastcall*)(const char*);
using ApplyBindingFn = std::int64_t(__fastcall*)(
    void*, std::uint32_t, std::uint16_t, std::uint32_t,
    std::uint16_t, std::uint8_t);
using GetLocalizedTextFn = const char*(__fastcall*)(
    void*, std::uint32_t, const std::uint32_t*, std::int32_t*);
using HashStringFn = std::uint32_t(__fastcall*)(std::int32_t, const char*);
using BuildMappingConflictsFn = std::int64_t(__fastcall*)(void*);
using ClearMappingConflictsFn = std::int64_t(__fastcall*)(void*);
using RefreshMappingUiItemFn = char(__fastcall*)(void*, std::uint32_t,
                                                  void*);
using SetMappingColumnMappedFn = std::int64_t(__fastcall*)(void*, bool);
using SetUiPropertyFn = std::int64_t(__fastcall*)(
    void*, const void*, std::uint32_t, std::uint32_t, std::uint64_t);
using RenderMappingTokenFn = std::int64_t(__fastcall*)(
    std::uint32_t, void*, std::int32_t, std::int32_t, bool,
    std::uint8_t, std::uint16_t, bool);
using ResolveMappingGlyphFn = std::int32_t(__fastcall*)(std::uint32_t,
                                                        std::int32_t);
using StartMappingCaptureFn = char(__fastcall*)(
    void*, std::uint32_t, std::uint8_t, std::uint16_t, std::uint16_t);

GetCategoriesFn g_getCategories = nullptr;
GetCategoryControlsFn g_getCategoryControls = nullptr;
GetBindingFn g_getBinding = nullptr;
GetControlNameFn g_getControlName = nullptr;
GetControlIdFn g_getControlId = nullptr;
ApplyBindingFn g_applyBinding = nullptr;
GetLocalizedTextFn g_getLocalizedText = nullptr;
HashStringFn g_hashString = nullptr;
BuildMappingConflictsFn g_buildMappingConflicts = nullptr;
ClearMappingConflictsFn g_clearMappingConflicts = nullptr;
RefreshMappingUiItemFn g_refreshMappingUiItem = nullptr;
SetMappingColumnMappedFn g_setMappingColumnMapped = nullptr;
SetUiPropertyFn g_setUiProperty = nullptr;
RenderMappingTokenFn g_renderMappingToken = nullptr;
ResolveMappingGlyphFn g_resolveMappingGlyph = nullptr;
StartMappingCaptureFn g_startMappingCapture = nullptr;

std::shared_mutex g_textMutex;
std::unordered_map<std::uint32_t, std::shared_ptr<const std::string>>
    g_customText;
std::mutex g_controlNameMutex;
std::unordered_map<std::uint32_t,
                   std::unique_ptr<std::array<char, 32>>> g_controlNames;
std::once_flag g_categoryEnumerationLogged;
std::once_flag g_controlEnumerationLogged;
std::once_flag g_conflictBypassLogged;
std::mutex g_captureMutex;
std::atomic_uint32_t g_captureTemplateId = 0;

const char* ControlName(std::uint32_t controlId)
{
    if (!InputBindingManager::Instance().FindByControlId(controlId))
        return nullptr;

    std::scoped_lock lock(g_controlNameMutex);
    if (const auto existing = g_controlNames.find(controlId);
        existing != g_controlNames.end())
        return existing->second->data();

    auto name = std::make_unique<std::array<char, 32>>();
    std::snprintf(name->data(), name->size(), "%s%08X",
                  ControlNamePrefix, controlId);
    const char* result = name->data();
    g_controlNames.emplace(controlId, std::move(name));
    return result;
}

std::vector<BindingInfo> VisibleBindings()
{
    auto bindings = InputBindingManager::Instance().Snapshot();
    std::erase_if(bindings, [](const BindingInfo& binding) {
        return binding.controlId == 0;
    });
    return bindings;
}

std::uint32_t CategoryHash()
{
    return g_hashString ? g_hashString(0, CategoryName) : 0;
}

void SetCustomText(std::uint32_t hash, std::string value)
{
    if (hash)
        g_customText[hash] =
            std::make_shared<const std::string>(std::move(value));
}

void RefreshCustomText(const std::vector<BindingInfo>& bindings)
{
    if (!g_hashString) return;
    std::unique_lock lock(g_textMutex);
    SetCustomText(CategoryHash(), CategoryLabel);
    for (const BindingInfo& binding : bindings) {
        const char* name = ControlName(binding.controlId);
        if (!name) continue;
        std::string key = "KMS_";
        key += name;
        // The stock control row does not clip its label before the two input
        // columns. Keep the visible action name compact; ownership remains
        // available through the scripting API and the persisted identity.
        SetCustomText(g_hashString(0, key.c_str()), binding.description);
    }
}

bool Reserve(EngineArray& array, std::uint16_t required)
{
    if (required <= array.capacity) return true;
    if (array.count > array.capacity) return false;

    const std::uint32_t capacity = std::max<std::uint32_t>(
        required, std::max<std::uint32_t>(8, array.capacity * 2u));
    if (capacity > UINT16_MAX) return false;
    auto* replacement = static_cast<std::uint32_t*>(
        CSysAllocator::Instance().Alloc(capacity * sizeof(std::uint32_t)));
    if (!replacement) return false;
    if (array.data && array.count)
        std::memcpy(replacement, array.data,
                    array.count * sizeof(std::uint32_t));
    if (array.data) CSysAllocator::Instance().Dealloc(array.data);
    array.data = replacement;
    array.capacity = static_cast<std::uint16_t>(capacity);
    return true;
}

bool Append(EngineArray& array, std::uint32_t value)
{
    if (array.count &&
        std::find(array.data, array.data + array.count, value) !=
            array.data + array.count)
        return true;
    if (array.count == UINT16_MAX ||
        !Reserve(array, static_cast<std::uint16_t>(array.count + 1)))
        return false;
    array.data[array.count++] = value;
    return true;
}

bool AppendRaw(EngineArray& array, std::uint32_t value)
{
    if (array.count == UINT16_MAX ||
        !Reserve(array, static_cast<std::uint16_t>(array.count + 1)))
        return false;
    array.data[array.count++] = value;
    return true;
}

bool Prepend(EngineArray& array, std::uint32_t value)
{
    if (array.count) {
        auto* existing = std::find(array.data, array.data + array.count, value);
        if (existing != array.data + array.count) {
            if (existing != array.data) {
                const std::size_t preceding = existing - array.data;
                std::memmove(array.data + 1, array.data,
                             preceding * sizeof(std::uint32_t));
                array.data[0] = value;
            }
            return true;
        }
    }

    if (array.count == UINT16_MAX ||
        !Reserve(array, static_cast<std::uint16_t>(array.count + 1)))
        return false;
    if (array.count)
        std::memmove(array.data + 1, array.data,
                     array.count * sizeof(std::uint32_t));
    array.data[0] = value;
    ++array.count;
    return true;
}

std::int64_t __fastcall GetCategoriesHook(EngineArray* output)
{
    const std::int64_t result = g_getCategories(output);
    if (!output) return result;
    const auto bindings = VisibleBindings();
    RefreshCustomText(bindings);
    if (!Prepend(*output, CategoryHash()))
        spdlog::warn("[Input] Could not prepend Script Bindings category");
    else
        std::call_once(g_categoryEnumerationLogged, [&] {
            spdlog::info(
                "[Input] Script Bindings category published first ({} binding(s))",
                bindings.size());
        });
    return result;
}

std::int64_t __fastcall GetCategoryControlsHook(
    const std::uint32_t* category, EngineArray* output)
{
    const std::int64_t result = g_getCategoryControls(category, output);
    if (!category || !output || *category != CategoryHash()) return result;

    const auto bindings = VisibleBindings();
    RefreshCustomText(bindings);
    std::call_once(g_controlEnumerationLogged, [&] {
        spdlog::info("[Input] Publishing {} supplemental binding row(s)",
                     bindings.size());
    });
    for (const BindingInfo& binding : bindings) {
        if (!Append(*output, binding.controlId)) {
            spdlog::error("[Input] Could not append binding control {}",
                          binding.controlId);
            break;
        }
    }
    return result;
}

bool EnableSupplementalMappingColumn(void* item) noexcept
{
    __try {
        auto* bytes = static_cast<std::byte*>(item);
        // A supplemental row deliberately has no retail input object. Keep
        // both mapping columns interactive; their values live in the
        // supplemental registry instead of the retail fixed-size array.
        g_setMappingColumnMapped(bytes + 112, true);
        g_setMappingColumnMapped(bytes + 272, true);

        // The retail refresh marks a row as invalid when neither column owns
        // a retail CControl mapping. Supplemental rows deliberately keep their
        // mapping state outside that fixed registry, so clear the warning via
        // the same reactive-property setter used by the retail UI.
        constexpr std::uint64_t updateContext = 0x00000000FFFFFFFFull;
        if (*reinterpret_cast<const std::uint8_t*>(bytes + 464) != 0) {
            const bool warningVisible = false;
            g_setUiProperty(bytes + 432, &warningVisible, 1, 0,
                            updateContext);
        }
        return *reinterpret_cast<const std::uint8_t*>(bytes + 464) == 0;
    } __except (EXCEPTION_EXECUTE_HANDLER) {
        return false;
    }
}

char __fastcall RefreshMappingUiItemHook(void* item,
                                         std::uint32_t controlId,
                                         void* enabledMask)
{
    const char result =
        g_refreshMappingUiItem(item, controlId, enabledMask);
    if (!item) return result;

    const auto binding =
        InputBindingManager::Instance().FindByControlId(controlId);
    if (!binding) return result;

    const bool rendered = EnableSupplementalMappingColumn(item);
    if (rendered) {
        static std::once_flag logged;
        std::call_once(logged, [] {
            spdlog::info(
                "[Input] Supplemental binding columns enabled in Controls UI");
        });
    } else {
        static std::once_flag logged;
        std::call_once(logged, [] {
            spdlog::error(
                "[Input] Could not render a supplemental binding cell");
        });
    }
    return result;
}

std::int64_t __fastcall RenderMappingTokenCustom(
    std::uint32_t controlId, void* output, std::int32_t,
    std::int32_t, bool, std::uint8_t device,
    std::uint16_t mappingIndex, bool)
{
    const auto binding =
        InputBindingManager::Instance().FindByControlId(controlId);

    if (!output || device != KeyboardDevice || mappingIndex > 1) {
        spdlog::warn(
            "[Input] Rejected mapping render for control {} "
            "(device=0x{:02X}, mapping={})",
            controlId, device, mappingIndex);
        return 0;
    }
    if (!binding) {
        spdlog::warn("[Input] No binding exists for control {}", controlId);
        return 0;
    }
    const std::string& parameter =
        mappingIndex == 0 ? binding->parameter
                          : binding->alternateParameter;
    if (parameter == "UNBOUND") return 0;

    std::uint8_t virtualKey = 0;
    std::string canonical;
    if (!InputBindingManager::NormalizeKeyboardParameter(
            parameter, virtualKey, canonical))
    {
        spdlog::warn(
            "[Input] Could not normalize mapping parameter '{}'", parameter);
        return 0;
    }

    // Native Controls expressions and capture commits use Win32 virtual-key
    // parameters. RAGE's 256-byte runtime keyboard snapshot is the only side
    // of the bridge that uses DirectInput-compatible scan codes.
    std::int32_t glyph = g_resolveMappingGlyph(virtualKey, 0);
    // During the first Controls-page build the keyboard glyph context can
    // briefly report -1, for which the retail resolver returns its generic
    // unknown glyph (995). Keyboard glyph IDs in this branch are VK + 3000;
    // use that exact retail mapping so an early cached row does not keep ???.
    if (glyph == 995) {
        glyph = static_cast<std::int32_t>(virtualKey) + 3000;
    }
    if (glyph == -1) {
        spdlog::warn(
            "[Input] Glyph lookup failed for virtual key 0x{:02X}",
            virtualKey);
        return 0;
    }

    auto& glyphs = *static_cast<EngineArray*>(output);
    const std::uint16_t previousCount = glyphs.count;
    if (!AppendRaw(glyphs, static_cast<std::uint32_t>(glyph))) {
        spdlog::error("[Input] Could not append mapping glyph {}", glyph);
        return 0;
    }

    // The native renderer stores two validity bits for each emitted glyph
    // immediately after its small-vector fields. This is the simple
    // single-key branch from CControl_RenderMappingToken.
    const std::uint32_t firstBit =
        previousCount >= 2 ? (previousCount & ~1u)
                           : (2u * previousCount);
    auto* bytes = static_cast<std::uint8_t*>(output);
    bytes[33 + (firstBit >> 3)] |=
        static_cast<std::uint8_t>(1u << (firstBit & 7));
    const std::uint32_t secondBit = firstBit + 1;
    bytes[33 + (secondBit >> 3)] |=
        static_cast<std::uint8_t>(1u << (secondBit & 7));

    {
        static std::once_flag logged;
        std::call_once(logged, [] {
            spdlog::info(
                "[Input] Supplemental mappings resolved through the native "
                "keyboard-glyph pipeline");
        });
    }
    return glyphs.count;
}

bool SetSupplementalCaptureIdentity(
    void* control, std::uint32_t controlId, std::uint8_t device,
    std::uint16_t mappingIndex, std::uint16_t subIndex) noexcept
{
    if (!control) return false;
    __try {
        auto* bytes = static_cast<std::byte*>(control);
        *reinterpret_cast<std::uint32_t*>(
            bytes + NativeMappingCaptureControlIdOffset) = controlId;
        *reinterpret_cast<std::uint16_t*>(
            bytes + NativeMappingCaptureMappingIndexOffset) = mappingIndex;
        *reinterpret_cast<std::uint16_t*>(
            bytes + NativeMappingCaptureSubIndexOffset) = subIndex;
        *reinterpret_cast<std::uint8_t*>(
            bytes + NativeMappingCaptureDeviceOffset) = device;
        return true;
    } __except (EXCEPTION_EXECUTE_HANDLER) {
        return false;
    }
}

char __fastcall StartMappingCaptureCustom(
    void* control, std::uint32_t controlId, std::uint8_t device,
    std::uint16_t mappingIndex, std::uint16_t subIndex)
{
    const auto binding =
        InputBindingManager::Instance().FindByControlId(controlId);

    if (!control || device != KeyboardDevice || mappingIndex > 1 ||
        !binding) {
        spdlog::warn(
            "[Input] Rejected remap capture for control {} "
            "(device=0x{:02X}, mapping={})",
            controlId, device, mappingIndex);
        return 0;
    }

    std::scoped_lock lock(g_captureMutex);

    // RAGE's capture state owns a ref-counted retail input object and refuses
    // to start without one. Borrow any valid keyboard mapping as the capture
    // sentinel, then restore the supplemental identity before polling begins.
    // The commit hook routes the captured parameter to InputBindingManager, so
    // the retail sentinel itself is never modified.
    std::uint32_t templateId =
        g_captureTemplateId.load(std::memory_order_relaxed);
    bool started = templateId != 0 &&
                   g_startMappingCapture(control, templateId, device, 0, 0);
    if (!started) {
        templateId = 0;
        for (std::uint32_t candidate = 1;
             candidate < InputBindingManager::NativeControlIdFirst;
             ++candidate) {
            const bool candidateStarted =
                g_startMappingCapture(control, candidate, device, 0, 0);
            if (candidateStarted) {
                templateId = candidate;
                started = true;
                break;
            }
        }
    }
    if (!started) {
        spdlog::error(
            "[Input] Could not acquire a retail keyboard capture sentinel");
        return 0;
    }

    if (!SetSupplementalCaptureIdentity(
            control, controlId, device, mappingIndex, subIndex)) {
        spdlog::error(
            "[Input] Could not publish supplemental mapping capture state");
        return 0;
    }

    g_captureTemplateId.store(templateId, std::memory_order_relaxed);
    spdlog::info(
        "[Input] Supplemental remap capture started for {} with retail "
        "sentinel {}",
        controlId, templateId);
    return 1;
}

void** __fastcall GetBindingCustom(void*, void** output, std::uint32_t,
                                   std::uint8_t, std::uint16_t, bool)
{
    if (!output) return output;
    *output = nullptr;
    return output;
}

const char* __fastcall GetControlNameCustom(void*, std::int32_t controlId)
{
    return ControlName(static_cast<std::uint32_t>(controlId));
}

std::uint32_t __fastcall GetControlIdHook(const char* name)
{
    if (name && std::strncmp(name, ControlNamePrefix,
                             sizeof(ControlNamePrefix) - 1) == 0) {
        const char* first = name + sizeof(ControlNamePrefix) - 1;
        const char* last = first + 8;
        std::uint32_t controlId = 0;
        const auto parsed = std::from_chars(first, last, controlId, 16);
        const bool valid =
            parsed.ec == std::errc{} && parsed.ptr == last && *last == '\0' &&
            ControlName(controlId);
        if (valid)
            return controlId;
    }
    return g_getControlId(name);
}

std::int64_t __fastcall ApplyBindingCustom(
    void*, std::uint32_t controlId, std::uint16_t mappingIndex,
    std::uint32_t parameter, std::uint16_t, std::uint8_t device)
{
    const auto binding =
        InputBindingManager::Instance().FindByControlId(controlId);
    if (!binding) {
        spdlog::warn("[Input] No binding exists for control {}", controlId);
        return 0;
    }
    if (parameter > 0xFF && parameter != UnboundParameter) {
        spdlog::warn(
            "[Input] Rejected mapping parameter 0x{:X}", parameter);
        return 0;
    }
    if (device != KeyboardDevice || mappingIndex > 1) {
        spdlog::warn(
            "[Input] Rejected binding apply for device 0x{:02X}, mapping {}",
            device, mappingIndex);
        return 0;
    }

    const BindingStatus status =
        InputBindingManager::Instance().SetKeyboardVirtualKeyByControlId(
            controlId, mappingIndex, parameter);
    if (status != BindingStatus::Ok) {
        spdlog::error(
            "[Input] Could not persist binding {} mapping {}: status {}",
            controlId, mappingIndex, static_cast<std::int32_t>(status));
        return 0;
    }

    RefreshCustomText(VisibleBindings());
    if (parameter == UnboundParameter)
        spdlog::info("[Input] Binding {} mapping {} was cleared", controlId,
                     mappingIndex);
    else
        spdlog::info(
            "[Input] Binding {} mapping {} remapped to virtual key 0x{:02X}",
            controlId, mappingIndex, parameter);
    return 1;
}

std::int64_t __fastcall BuildMappingConflictsCustom(void* control)
{
    std::call_once(g_conflictBypassLogged, [] {
        spdlog::info(
            "[Input] Retail conflict list isolated from supplemental registry");
    });
    return control
               ? g_clearMappingConflicts(
                     static_cast<std::byte*>(control) +
                         NativeMappingCaptureOffset)
               : 0;
}

const char* __fastcall GetLocalizedTextHook(
    void* text, std::uint32_t language, const std::uint32_t* hash,
    std::int32_t* resolvedLanguage)
{
    if (hash) {
        thread_local std::shared_ptr<const std::string> retained;
        std::shared_lock lock(g_textMutex);
        if (const auto it = g_customText.find(*hash);
            it != g_customText.end()) {
            retained = it->second;
            if (resolvedLanguage) *resolvedLanguage = -1;
            return retained->c_str();
        }
    }
    return g_getLocalizedText(text, language, hash, resolvedLanguage);
}

class AbsoluteJumpStub final : public jitasm::Frontend {
public:
    void SetTarget(std::uintptr_t target) { m_target = target; }

private:
    void InternalMain() override
    {
        mov(rax, m_target);
        jmp(rax);
    }
    std::uintptr_t m_target = 0;
};

class GetBindingBridgeStub final : public jitasm::Frontend {
    void InternalMain() override
    {
        cmp(r8d, InputBindingManager::NativeControlIdFirst);
        jb("retail");
        cmp(r8d, InputBindingManager::NativeControlIdLast);
        ja("retail");
        mov(rax, reinterpret_cast<std::uintptr_t>(&GetBindingCustom));
        jmp(rax);
        L("retail");
        mov(rax, reinterpret_cast<std::uintptr_t>(&g_getBinding));
        mov(rax, qword_ptr[rax]);
        jmp(rax);
    }
};

class RenderMappingTokenBridgeStub final : public jitasm::Frontend {
    void InternalMain() override
    {
        cmp(ecx, InputBindingManager::NativeControlIdFirst);
        jb("retail");
        cmp(ecx, InputBindingManager::NativeControlIdLast);
        ja("retail");
        mov(rax,
            reinterpret_cast<std::uintptr_t>(&RenderMappingTokenCustom));
        jmp(rax);
        L("retail");
        mov(rax, reinterpret_cast<std::uintptr_t>(&g_renderMappingToken));
        mov(rax, qword_ptr[rax]);
        jmp(rax);
    }
};

class StartMappingCaptureBridgeStub final : public jitasm::Frontend {
    void InternalMain() override
    {
        cmp(edx, InputBindingManager::NativeControlIdFirst);
        jb("retail");
        cmp(edx, InputBindingManager::NativeControlIdLast);
        ja("retail");
        mov(rax,
            reinterpret_cast<std::uintptr_t>(&StartMappingCaptureCustom));
        jmp(rax);
        L("retail");
        mov(rax, reinterpret_cast<std::uintptr_t>(&g_startMappingCapture));
        mov(rax, qword_ptr[rax]);
        jmp(rax);
    }
};

class ControlNameBridgeStub final : public jitasm::Frontend {
    void InternalMain() override
    {
        cmp(edx, InputBindingManager::NativeControlIdFirst);
        jb("retail");
        cmp(edx, InputBindingManager::NativeControlIdLast);
        ja("retail");
        mov(rax, reinterpret_cast<std::uintptr_t>(&GetControlNameCustom));
        jmp(rax);
        L("retail");
        mov(rax, reinterpret_cast<std::uintptr_t>(&g_getControlName));
        mov(rax, qword_ptr[rax]);
        jmp(rax);
    }
};

class ApplyBindingBridgeStub final : public jitasm::Frontend {
    void InternalMain() override
    {
        cmp(edx, InputBindingManager::NativeControlIdFirst);
        jb("retail");
        cmp(edx, InputBindingManager::NativeControlIdLast);
        ja("retail");
        mov(rax, reinterpret_cast<std::uintptr_t>(&ApplyBindingCustom));
        jmp(rax);
        L("retail");
        mov(rax, reinterpret_cast<std::uintptr_t>(&g_applyBinding));
        mov(rax, qword_ptr[rax]);
        jmp(rax);
    }
};

class ConflictBridgeStub final : public jitasm::Frontend {
    void InternalMain() override
    {
        test(rcx, rcx);
        jz("retail");
        mov(eax, dword_ptr[
            rcx + NativeMappingCaptureControlIdOffset]);
        cmp(eax, InputBindingManager::NativeControlIdFirst);
        jb("retail");
        cmp(eax, InputBindingManager::NativeControlIdLast);
        ja("retail");
        mov(rax,
            reinterpret_cast<std::uintptr_t>(&BuildMappingConflictsCustom));
        jmp(rax);
        L("retail");
        mov(rax, reinterpret_cast<std::uintptr_t>(&g_buildMappingConflicts));
        mov(rax, qword_ptr[rax]);
        jmp(rax);
    }
};

class EnabledMaskBridgeStub final : public jitasm::Frontend {
public:
    void SetReturnAddress(std::uintptr_t address) { m_returnAddress = address; }

private:
    void InternalMain() override
    {
        cmp(edi, InputBindingManager::NativeControlIdFirst);
        jb("retail");
        cmp(edi, InputBindingManager::NativeControlIdLast);
        ja("retail");

        // The retail mask contains exactly 13 qwords (832 bits), but only the
        // 785 retail actions are initialized into it. Supplemental rows are
        // always valid/enabled and therefore take the retail mask-hit result.
        // This build canonicalizes both the result and the destructor flag
        // through EBX before leaving this block.
        mov(ebx, 1);
        mov(al, bl);
        jmp("done");

        L("retail");
        mov(rdx, qword_ptr[rbp + 0x60]);
        mov(eax, edi);
        and(eax, 0x3F);
        movzx(ecx, al);
        mov(rax, rdi);
        shr(rax, 6);
        mov(rax, qword_ptr[rdx + rax * 8]);
        bt(rax, rcx);
        jb("enabled");
        mov(eax, esi);
        and(eax, 0xFF);
        test(eax, eax);
        jnz("disabled");
        test(bl, bl);
        jz("enabled");
        L("disabled");
        mov(al, r15b);
        mov(ebx, 1);
        jmp("done");
        L("enabled");
        mov(ebx, 1);
        mov(al, bl);

        L("done");
        // AL is the warning-visible result consumed by the first instruction
        // after this replaced block. An absolute jump through RAX corrupts
        // that result with the low byte of the return address, which marks
        // every retail row as invalid. R11 is volatile and dead here.
        mov(r11, m_returnAddress);
        jmp(r11);
    }
    std::uintptr_t m_returnAddress = 0;
};

AbsoluteJumpStub g_categoriesStub;
AbsoluteJumpStub g_categoryControlsStub;
AbsoluteJumpStub g_controlIdStub;
AbsoluteJumpStub g_localizedTextStub;
AbsoluteJumpStub g_refreshUiItemStub;
GetBindingBridgeStub g_getBindingStub;
RenderMappingTokenBridgeStub g_renderMappingTokenStub;
StartMappingCaptureBridgeStub g_startMappingCaptureStub;
ControlNameBridgeStub g_controlNameStub;
ApplyBindingBridgeStub g_applyBindingStub;
ConflictBridgeStub g_conflictStub;
EnabledMaskBridgeStub g_enabledMaskStub;

struct HookSpec {
    const char* name;
    void* target;
    void* replacement;
    void** original;
};

template<std::size_t Count>
bool InstallHooks(const std::array<HookSpec, Count>& hooks)
{
    std::size_t created = 0;
    for (; created < hooks.size(); ++created) {
        const MH_STATUS status = MH_CreateHook(
            hooks[created].target, hooks[created].replacement,
            hooks[created].original);
        if (status != MH_OK) {
            spdlog::error(
                "[Input] Could not create Jitasm bridge #{} '{}' target={}: {}",
                created, hooks[created].name, fmt::ptr(hooks[created].target),
                static_cast<int>(status));
            break;
        }
    }
    if (created != hooks.size()) {
        while (created) MH_RemoveHook(hooks[--created].target);
        return false;
    }

    std::size_t enabled = 0;
    for (; enabled < hooks.size(); ++enabled) {
        const MH_STATUS status = MH_EnableHook(hooks[enabled].target);
        if (status != MH_OK) {
            spdlog::error(
                "[Input] Could not enable Jitasm bridge #{} '{}' target={}: {}",
                enabled, hooks[enabled].name,
                fmt::ptr(hooks[enabled].target), static_cast<int>(status));
            break;
        }
    }
    if (enabled != hooks.size()) {
        while (enabled) MH_DisableHook(hooks[--enabled].target);
        for (const HookSpec& hook : hooks) MH_RemoveHook(hook.target);
        return false;
    }
    return true;
}

CMemory::Hook g_installNativeBindingMenu([] {
    CMemory categories = CMemory::Pattern(
        "40 53 48 83 EC 20 48 8B D9 48 8B 09 E8 ? ? ? ? 33 C0 48 8B CB "
        "48 89 03 89 43 08 0F B7 15 ? ? ? ?").Search(false);
    CMemory categoryControls = CMemory::Pattern(
        "48 89 5C 24 08 57 48 83 EC 20 48 8B F9 48 8B DA 48 8B 0A E8 ? "
        "? ? ? 33 C0 48 89 03 89 43 08").Search(false);
    CMemory getBinding = CMemory::Pattern(
        "40 53 48 83 EC 40 8A 44 24 78 48 8B DA 88 44 24 28 48 8D 54 24 "
        "38 0F B7 44 24 70 66 89 44 24 20 E8 ? ? ? ?").Search(false);
    CMemory getControlName = CMemory::Pattern(
        "4C 8B 41 08 33 C0 4C 8B CA 4D 85 C0 74 27 66 3B 41 10 73 21 44 "
        "0F B7 51 10").Search(false);
    CMemory getControlId = CMemory::Pattern(
        "48 83 EC 28 48 8B D1 45 33 C9 48 8D 0D ? ? ? ? 45 33 C0 E8 ? ? "
        "? ? 48 83 F8 FF 75 02 33 C0").Search(false);
    CMemory applyBinding = CMemory::Pattern(
        "48 8B C4 48 89 58 08 48 89 68 10 48 89 70 18 57 48 83 EC 40 C6 "
        "40 E0 00 41 8B E9 44 8A 4C 24 78 8B F2 66 44 89 40 D8").Search(false);
    CMemory getLocalizedText = CMemory::Pattern(
        "89 54 24 10 53 56 57 48 83 EC 20 41 83 09 FF 49 8B F9 0F B7 81 "
        "E8 00 00 00 4C 8D 4C 24 40").Search(false);
    CMemory hashString = CMemory::Pattern(
        "48 63 C1 48 8B CA 4C 8D 04 80 4D 03 C0 48 8D 05 ? ? ? ? 4A FF "
        "24 C0").Search(false);
    CMemory buildConflicts = CMemory::Pattern(
        "40 53 48 81 EC A0 00 00 00 65 48 8B 14 25 58 00 00 00 48 8B "
        "D9 8B 05 ? ? ? ?").Search(false);
    CMemory clearConflicts = CMemory::Pattern(
        "48 89 5C 24 08 57 48 83 EC 20 48 8B D9 48 8B 49 10 E8 ? ? ? "
        "? 33 FF 48 89 7B 10").Search(false);
    CMemory enabledMaskRead = CMemory::Pattern(
        "48 8B 55 60 8B C7 83 E0 3F 0F B6 C8 48 8B C7 48 C1 E8 06 "
        "48 8B 04 C2 48 0F A3 C8 72 13 40 84 F6 75 04 84 DB 74 0A "
        "41 8A C7 BB 01 00 00 00 EB 07 BB 01 00 00 00 8A C3").Search(false);
    CMemory refreshUiItem = CMemory::Pattern(
        "4C 89 44 24 18 55 53 56 57 41 54 41 55 41 56 41 57 48 8D "
        "6C 24 F8 48 81 EC 08 01 00 00 48 8B D9 8B FA").Search(false);
    CMemory setMappingColumnMapped = CMemory::Pattern(
        "40 53 48 83 EC 30 48 8D 59 78 38 53 20 74").Search(false);
    CMemory setUiProperty = CMemory::Pattern(
        "40 53 48 83 EC 30 48 8B 44 24 60 48 8B D9 4C 8B 11 48 89 "
        "44 24 20 41 FF 92 B0 00 00 00 84 C0 74").Search(false);
    CMemory renderMappingToken = CMemory::Pattern(
        "4C 8B DC 49 89 5B 08 49 89 6B 10 49 89 73 18 49 89 7B 20 "
        "41 56 48 83 EC 60 8A 84 24 A8 00 00 00 41 8B F0 88 44 24 "
        "28 45 8B F1").Search(false);
    CMemory resolveMappingGlyph = CMemory::Pattern(
        "48 83 EC 28 41 83 C8 FF 44 8B C9 41 81 E1 00 F0 0F 00 74 "
        "79 41 8D 81 00 F0 FF FF A9 FF EF FF FF 74 64 41 81 F9 00 "
        "30 00").Search(false);
    CMemory startMappingCapture = CMemory::Pattern(
        "48 89 5C 24 10 48 89 6C 24 18 56 57 41 56 48 83 EC 40 4C "
        "8D B1 78 0F 00 00 48 8B D9 49 8B CE 41 0F B7 F9 41 8A F0 "
        "8B EA").Search(false);
    std::array<CMemory, 17> targets = {
        categories, categoryControls, getBinding, getControlName,
        getControlId, applyBinding, getLocalizedText, hashString,
        buildConflicts, clearConflicts, enabledMaskRead, refreshUiItem,
        setMappingColumnMapped, setUiProperty, renderMappingToken,
        resolveMappingGlyph, startMappingCapture};
    constexpr std::array<const char*, 17> targetNames = {
        "category enumeration", "category-control enumeration",
        "binding lookup", "control-name lookup", "control-id lookup",
        "binding apply", "localized-text lookup", "string hash",
        "conflict builder", "conflict clear", "UI enabled-mask read",
        "mapping-row refresh", "mapping-column mapped setter",
        "UI reactive-property setter", "mapping-token renderer",
        "mapping-glyph resolver", "mapping-capture start"};
    bool missingTarget = false;
    for (std::size_t index = 0; index < targets.size(); ++index) {
        if (targets[index].IsValid()) continue;
        missingTarget = true;
        spdlog::error("[Input] Could not resolve {}", targetNames[index]);
    }
    if (missingTarget) {
        spdlog::error(
            "[Input] Supplemental binding integration is unavailable");
        return;
    }

    g_hashString = hashString.Get<HashStringFn>();
    g_clearMappingConflicts = clearConflicts.Get<ClearMappingConflictsFn>();
    g_setMappingColumnMapped =
        setMappingColumnMapped.Get<SetMappingColumnMappedFn>();
    g_setUiProperty = setUiProperty.Get<SetUiPropertyFn>();
    g_resolveMappingGlyph =
        resolveMappingGlyph.Get<ResolveMappingGlyphFn>();

    g_categoriesStub.SetTarget(
        reinterpret_cast<std::uintptr_t>(&GetCategoriesHook));
    g_categoryControlsStub.SetTarget(
        reinterpret_cast<std::uintptr_t>(&GetCategoryControlsHook));
    g_controlIdStub.SetTarget(
        reinterpret_cast<std::uintptr_t>(&GetControlIdHook));
    g_localizedTextStub.SetTarget(
        reinterpret_cast<std::uintptr_t>(&GetLocalizedTextHook));
    g_refreshUiItemStub.SetTarget(
        reinterpret_cast<std::uintptr_t>(&RefreshMappingUiItemHook));

    const std::array<HookSpec, 11> hooks = {{
        {"categories", categories.Get<void*>(), g_categoriesStub.GetCode(),
         reinterpret_cast<void**>(&g_getCategories)},
        {"category-controls", categoryControls.Get<void*>(),
         g_categoryControlsStub.GetCode(),
         reinterpret_cast<void**>(&g_getCategoryControls)},
        {"binding-lookup", getBinding.Get<void*>(),
         g_getBindingStub.GetCode(),
         reinterpret_cast<void**>(&g_getBinding)},
        {"control-name", getControlName.Get<void*>(),
         g_controlNameStub.GetCode(),
         reinterpret_cast<void**>(&g_getControlName)},
        {"control-id", getControlId.Get<void*>(),
         g_controlIdStub.GetCode(),
         reinterpret_cast<void**>(&g_getControlId)},
        {"binding-apply", applyBinding.Get<void*>(),
         g_applyBindingStub.GetCode(),
         reinterpret_cast<void**>(&g_applyBinding)},
        {"localized-text", getLocalizedText.Get<void*>(),
         g_localizedTextStub.GetCode(),
         reinterpret_cast<void**>(&g_getLocalizedText)},
        {"conflict-builder", buildConflicts.Get<void*>(),
         g_conflictStub.GetCode(),
         reinterpret_cast<void**>(&g_buildMappingConflicts)},
        {"row-refresh", refreshUiItem.Get<void*>(),
         g_refreshUiItemStub.GetCode(),
         reinterpret_cast<void**>(&g_refreshMappingUiItem)},
        {"mapping-renderer", renderMappingToken.Get<void*>(),
         g_renderMappingTokenStub.GetCode(),
         reinterpret_cast<void**>(&g_renderMappingToken)},
        {"mapping-capture", startMappingCapture.Get<void*>(),
         g_startMappingCaptureStub.GetCode(),
         reinterpret_cast<void**>(&g_startMappingCapture)},
    }};
    if (!InstallHooks(hooks)) return;

    constexpr std::size_t ReplacedMaskBytes = 55;
    g_enabledMaskStub.SetReturnAddress(
        reinterpret_cast<std::uintptr_t>(
            enabledMaskRead.Get<std::byte*>() + ReplacedMaskBytes));
    void* enabledMaskBridge = g_enabledMaskStub.GetCode();
    enabledMaskRead.Nop(ReplacedMaskBytes);
    enabledMaskRead.FarJump(enabledMaskBridge);

    spdlog::info(
        "[Input] Jitasm supplemental binding UI initialized ({} slots, "
        "control IDs {}..{})",
        AliasCount, InputBindingManager::NativeControlIdFirst,
        InputBindingManager::NativeControlIdLast);
});

} // namespace
} // namespace rdr2::input
