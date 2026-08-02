#pragma once

#include <cstdint>
#include <string>

#include <wasmtime.h>

namespace rdr2wasm {

using GetNativeAddressFunc = uintptr_t(*)(uint64_t hash);
using GetGlobalPointerFunc = void*(*)(uint32_t globalVarId);

class Runtime {
public:
    Runtime();
    ~Runtime();

    Runtime(const Runtime&) = delete;
    Runtime& operator=(const Runtime&) = delete;

    wasm_engine_t* Engine() const noexcept { return m_engine; }
    bool IsValid() const noexcept { return m_engine != nullptr; }

private:
    wasm_engine_t* m_engine = nullptr;
};

Runtime& GetRuntime();

void InstallGameBridge(GetNativeAddressFunc getNativeAddr,
                       GetGlobalPointerFunc getGlobalPtr);
GetNativeAddressFunc GetNativeAddrFn();
GetGlobalPointerFunc GetGlobalPtrFn();

// Consume a Wasmtime-owned diagnostic and return it as a C++ string.
std::string TakeError(wasmtime_error_t* error);
std::string TakeTrap(wasm_trap_t* trap);

} // namespace rdr2wasm
