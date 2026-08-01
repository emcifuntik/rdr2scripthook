#include "wasm/Mod.h"
#include "wasm/Runtime.h"

#include <cstdint>
#include <filesystem>
#include <iterator>

namespace {

struct NativeContext {
    uint64_t* retVal;
    uint64_t argCount;
    uint64_t* stackPtr;
    uint64_t dataCount;
    uint64_t spaceForResults[24];
    uint64_t stack[24];
};

void __cdecl StubNative(NativeContext* context) {
    context->stack[0] = 0;
    context->stack[1] = 0;
    context->stack[2] = 0;
}

uintptr_t GetNativeAddress(uint64_t) {
    return reinterpret_cast<uintptr_t>(&StubNative);
}

void* GetGlobalPointer(uint32_t index) {
    static uint64_t globals[1024]{};
    return index < std::size(globals) ? &globals[index] : nullptr;
}

} // namespace

int main() {
    auto& runtime = rdr2wasm::GetRuntime();
    if (!runtime.IsValid()) return 1;

    rdr2wasm::InstallGameBridge(GetNativeAddress, GetGlobalPointer);
    rdr2wasm::ModManifest manifest{
        .name = "WASM smoke test",
        .version = "1.0.0",
        .author = "RDR2 Script Hook",
        .description = "Loads the compiled Rust example",
        .entrypoint = "main.wasm",
        .modPath = std::filesystem::path(WASM_EXAMPLE_MOD_PATH),
    };

    rdr2wasm::Mod mod(runtime, std::move(manifest));
    if (!mod.LoadEntrypoint()) return 2;

    for (int tick = 0; tick < 300; ++tick) mod.Tick();
    mod.OnKeyDown(0x73); // F4
    mod.OnKeyUp(0x73);
    return 0;
}
