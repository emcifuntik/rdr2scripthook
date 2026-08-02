#include "stdafx.h"

#include "GameInputHook.h"

#include "CMemory.h"

#include <atomic>
#include <cstring>
#include <limits>

namespace rdr2::input {
namespace {

std::atomic_bool g_blocked = false;
std::atomic_uint32_t g_cursorVisibilityRequests = 0;
const std::int32_t* g_keyboardSnapshotIndex = nullptr;
const std::uint8_t* g_keyboardSnapshots = nullptr;

CMemory::Hook g_resolveKeyboardState([] {
    // rage::ioKeyboard::Update selects the inactive 256-byte buffer before
    // publishing it. Both RIP-relative globals are resolved from that stable
    // sequence instead of relying on executable-version RVAs.
    CMemory match = CMemory::Pattern(
        "48 63 05 ? ? ? ? 4C 8D 35 ? ? ? ? 48 83 F0 01 48 C1 E0 08")
                        .Search(false);
    if (!match.IsValid()) {
        spdlog::warn("[Input] Could not resolve the RAGE keyboard snapshot; "
                     "using the Win32 compatibility fallback");
        return;
    }

    g_keyboardSnapshotIndex = match.GetOffset(3).Get<const std::int32_t*>();
    g_keyboardSnapshots = (match + 7).GetOffset(3).Get<const std::uint8_t*>();
    spdlog::info("[Input] RAGE keyboard snapshot resolved");
});

} // namespace

void GameInputHook::SetBlocked(bool blocked)
{
    g_blocked.store(blocked, std::memory_order_release);
}

bool GameInputHook::IsBlocked()
{
    return g_blocked.load(std::memory_order_acquire);
}

std::uint32_t GameInputHook::AcquireCursorVisibility()
{
    std::uint32_t current =
        g_cursorVisibilityRequests.load(std::memory_order_relaxed);
    while (current != std::numeric_limits<std::uint32_t>::max() &&
           !g_cursorVisibilityRequests.compare_exchange_weak(
               current, current + 1, std::memory_order_acq_rel,
               std::memory_order_relaxed)) {
    }
    const std::uint32_t result =
        current == std::numeric_limits<std::uint32_t>::max()
            ? current
            : current + 1;
    return result;
}

std::uint32_t GameInputHook::ReleaseCursorVisibility()
{
    std::uint32_t current =
        g_cursorVisibilityRequests.load(std::memory_order_relaxed);
    while (current != 0 &&
           !g_cursorVisibilityRequests.compare_exchange_weak(
               current, current - 1, std::memory_order_acq_rel,
               std::memory_order_relaxed)) {
    }
    const std::uint32_t result = current == 0 ? 0 : current - 1;
    return result;
}

std::uint32_t GameInputHook::CursorVisibilityRequests()
{
    return g_cursorVisibilityRequests.load(std::memory_order_acquire);
}

bool GameInputHook::CopyKeyboardState(
    std::span<std::uint8_t, 256> destination)
{
    if (!g_keyboardSnapshotIndex || !g_keyboardSnapshots) return false;

    __try {
        std::int32_t index = *g_keyboardSnapshotIndex;
        if (index < 0 || index > 1) return false;
        std::memcpy(destination.data(),
                    g_keyboardSnapshots + static_cast<std::size_t>(index) * 256,
                    destination.size());

        // The input thread can publish while this thread copies. One retry is
        // enough to guarantee we do not return a split old/new snapshot.
        const std::int32_t published = *g_keyboardSnapshotIndex;
        if (published != index) {
            if (published < 0 || published > 1) return false;
            std::memcpy(destination.data(),
                        g_keyboardSnapshots +
                            static_cast<std::size_t>(published) * 256,
                        destination.size());
        }
        return true;
    } __except (EXCEPTION_EXECUTE_HANDLER) {
        return false;
    }
}

} // namespace rdr2::input
