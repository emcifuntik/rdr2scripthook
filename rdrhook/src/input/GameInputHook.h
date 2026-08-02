#pragma once

#include <cstdint>
#include <span>

namespace rdr2::input {

class GameInputHook final {
public:
    static void SetBlocked(bool blocked);
    static bool IsBlocked();

    // Cursor visibility is owned independently from input focus. Calls remain
    // reference-counted; the script runtime translates active ownership into
    // RDR2's SET_MOUSE_CURSOR_THIS_FRAME native on every game-script tick.
    static std::uint32_t AcquireCursorVisibility();
    static std::uint32_t ReleaseCursorVisibility();
    static std::uint32_t CursorVisibilityRequests();

    // Copies the keyboard snapshot most recently published by rage::ioInput.
    // Returns false on unsupported game builds so callers can retain a
    // version-tolerant fallback.
    static bool CopyKeyboardState(
        std::span<std::uint8_t, 256> destination);
};

} // namespace rdr2::input
