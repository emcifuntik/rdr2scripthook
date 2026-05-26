#pragma once

// Centralised native bindings for an ejsc::Context backing a Mod:
//   - console.log / warn / error globals
//   - Hash.joaat global
//   - VK_* key code globals
//   - setTimeout / setInterval / clearTimeout / clearInterval (via TimerManager)
//   - Vector3 class binding (rdr2js::Vec3)
//   - Native global (Native.invoke / invokeFloat / invokeVector3)
//   - Global global (Global.getInt / setInt / getFloat / setFloat)
//   - _Core global (getGameTime, isKeyPressed, isKeyJustPressed)
//   - 'core' native ES module (addTickCallback, etc.)
//   - 'natives' native ES module (the auto-generated 7000+ game natives)
//
// Driven by Mod's ctor; not intended to be called from outside the js/ layer.

namespace rdr2js {

class Mod;

namespace bindings {

void InstallAll(Mod& mod);

// Called from CScriptManager before every JS frame; samples GetAsyncKeyState
// for the _Core.isKeyPressed / _Core.isKeyJustPressed helpers. These globals
// share state across all mods because they reflect Windows-level key state.
void PollKeyboard();

// Sets the game-time value returned by _Core.getGameTime. Optional; if you
// don't call it, _Core.getGameTime returns 0.
void SetGameTime(unsigned long timeMs);

} // namespace bindings
} // namespace rdr2js
