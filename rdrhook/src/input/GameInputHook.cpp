#include "stdafx.h"

#include "GameInputHook.h"

#include <atomic>
#include <limits>

namespace rdr2::input {
namespace {

std::atomic_bool g_blocked = false;
std::atomic_uint32_t g_cursorVisibilityRequests = 0;

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

} // namespace rdr2::input
