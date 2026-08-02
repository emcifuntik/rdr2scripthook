#include "stdafx.h"
#include "CMemory.h"

namespace {
  CMemory::Hook _PatchKeyboardHook([]() {
    constexpr CMemory::Pattern keyboardHookCallPattern("48 8B 05 ? ? ? ? 48 85 C0 74 ? FF D0 8A D8");
    CMemory keyboardHookCall = keyboardHookCallPattern.Search();

    if (keyboardHookCall.IsValid())
    {
        keyboardHookCall.Nop(16);
    }
    else
    {
        spdlog::warn("Failed to find keyboard hook pattern");
    }
  });
}
