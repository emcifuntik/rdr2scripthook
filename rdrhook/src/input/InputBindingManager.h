#pragma once

#include <array>
#include <cstdint>
#include <filesystem>

// A project utility header has the same case-insensitive name as the CRT
// process header. clang-cl therefore needs these declarations before <mutex>
// includes <thread>.
#if defined(_WIN32) && defined(__clang__)
extern "C" {
    uintptr_t __cdecl _beginthreadex(
        void*, unsigned, unsigned (__stdcall*)(void*), void*, unsigned,
        unsigned*);
    void __cdecl _endthreadex(unsigned);
}
#endif

#include <mutex>
#include <optional>
#include <span>
#include <string>
#include <string_view>
#include <tuple>
#include <unordered_map>
#include <utility>
#include <vector>

namespace rdr2::input {

using BindingHandle = std::int32_t;

enum class BindingEvent : std::int32_t {
    None = 0,
    Down = 1,
    Up = 2,
};

enum class BindingStatus : std::int32_t {
    Ok = 0,
    InvalidHandle = -1,
    InvalidArgument = -2,
    UnsupportedMapper = -3,
    UnsupportedParameter = -4,
    AlreadyRegistered = -5,
    CapacityExceeded = -6,
};

struct BindingRegistration {
    BindingHandle handle = 0;
    BindingStatus status = BindingStatus::InvalidArgument;
};

struct BindingInfo {
    BindingHandle handle = 0;
    std::uint32_t controlId = 0;
    std::string owner;
    std::string id;
    std::string description;
    std::string mapper;
    std::string parameter;
    std::string alternateParameter;
    std::string defaultMapper;
    std::string defaultParameter;
    bool alternateMapped = false;
    bool down = false;
};

// Script-owned input actions. The manager consumes the engine keyboard
// snapshot and provides stable action handles, edge events, rebinding, and
// persistence. Manifest declarations reserve native settings slots before the
// frontend caches its controls; the runtime registration later adopts them.
class InputBindingManager final {
public:
    // RDR2 1.0.1491.50 owns IDs 0..784. Jitasm intercepts the following 2048
    // supplemental IDs before retail code can index its fixed arrays.
    static constexpr std::uint32_t NativeControlIdFirst = 785;
    static constexpr std::uint32_t NativeBindingCapacity = 2048;
    static constexpr std::uint32_t NativeControlIdLast =
        NativeControlIdFirst + NativeBindingCapacity - 1;

    static InputBindingManager& Instance();

    void SetStoragePath(std::filesystem::path path);

    BindingRegistration Predeclare(std::string ownerName, std::string id,
                                   std::string description,
                                   std::string mapper,
                                   std::string defaultParameter);
    BindingRegistration Register(void* owner, std::string ownerName,
                                 std::string id, std::string description,
                                 std::string mapper,
                                 std::string defaultParameter);
    BindingStatus Unregister(void* owner, BindingHandle handle);
    void ReleaseOwner(void* owner);

    BindingEvent PollEvent(void* owner, BindingHandle handle);
    std::int32_t IsDown(void* owner, BindingHandle handle) const;

    BindingStatus GetMapping(void* owner, BindingHandle handle,
                             std::string& mapper,
                             std::string& parameter) const;
    BindingStatus SetMapping(void* owner, BindingHandle handle,
                             std::string mapper, std::string parameter);
    BindingStatus ResetMapping(void* owner, BindingHandle handle);
    BindingStatus SetKeyboardVirtualKeyByControlId(std::uint32_t controlId,
                                                   std::uint32_t virtualKey);
    BindingStatus SetKeyboardVirtualKeyByControlId(
        std::uint32_t controlId, std::uint16_t mappingIndex,
        std::uint32_t virtualKey);

    std::vector<BindingInfo> Snapshot() const;
    std::optional<BindingInfo> FindByControlId(std::uint32_t controlId) const;
    void UpdateKeyboardState(std::span<const std::uint8_t, 256> state);

    static bool NormalizeKeyboardParameter(std::string_view parameter,
                                           std::uint8_t& virtualKey,
                                           std::string& canonicalName);
    static bool VirtualKeyToRageKey(std::uint8_t virtualKey,
                                    std::uint8_t& rageKey);

#ifdef RDR2_WASM_TEST
    void ResetForTesting();
#endif

private:
    InputBindingManager() = default;
    struct State;
    struct SavedMapping {
        std::string mapper;
        std::string parameter;
        std::string alternateParameter = "UNBOUND";
    };

    State* FindOwnedLocked(void* owner, BindingHandle handle);
    const State* FindOwnedLocked(void* owner, BindingHandle handle) const;
    BindingRegistration InsertLocked(void* owner, bool predeclared,
                                     std::string ownerName, std::string id,
                                     std::string description,
                                     std::string canonicalMapper,
                                     std::string canonicalParameter,
                                     std::uint8_t virtualKey);
    std::uint32_t AllocateControlIdLocked(std::string_view identity) const;
    void PromoteNativeSlotsLocked();
    void LoadLocked();
    void SaveLocked() const;

    mutable std::mutex m_mutex;
    std::filesystem::path m_storagePath;
    std::unordered_map<BindingHandle, State> m_bindings;
    std::unordered_map<std::string, SavedMapping> m_savedMappings;
    BindingHandle m_nextHandle = 1;
    bool m_loaded = false;
};

} // namespace rdr2::input
