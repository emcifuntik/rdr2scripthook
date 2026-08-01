#pragma once

#include <cstdint>

struct wasmtime_linker;
typedef struct wasmtime_linker wasmtime_linker_t;

namespace rdr2wasm {

class Mod;

namespace bindings {

bool DefineAll(Mod& mod, wasmtime_linker_t* linker);
void PollKeyboard();

} // namespace bindings
} // namespace rdr2wasm
