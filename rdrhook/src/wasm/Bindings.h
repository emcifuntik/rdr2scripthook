#pragma once

#include <cstdint>

struct wasmtime_linker;
typedef struct wasmtime_linker wasmtime_linker_t;

namespace rdr2wasm {

class Mod;

namespace bindings {

bool DefineAll(Mod& mod, wasmtime_linker_t* linker);
void PollKeyboard();
// Runs game-native UI input maintenance from the active WasmScriptThread.
void PumpGameUiInput();
// Releases reference-counted host resources still owned by a mod. Safe to
// call repeatedly and during partial-load cleanup.
void ReleaseOwnedResources(Mod& mod);
#ifdef RDR2_WASM_TEST
void SetKeyStateForTesting(uint32_t key, bool pressed);
#endif

} // namespace bindings
} // namespace rdr2wasm
