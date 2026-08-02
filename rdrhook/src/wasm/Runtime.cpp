#include "Runtime.h"

#include "Logger.h"

namespace rdr2wasm {

namespace {
GetNativeAddressFunc g_getNativeAddr = nullptr;
GetGlobalPointerFunc g_getGlobalPtr = nullptr;
}

Runtime::Runtime() {
    wasm_config_t* config = wasm_config_new();
    if (!config) {
        spdlog::error("[WASM] Failed to create Wasmtime configuration");
        return;
    }

    wasmtime_config_consume_fuel_set(config, true);
    wasmtime_config_cranelift_opt_level_set(config, WASMTIME_OPT_LEVEL_SPEED);
    m_engine = wasm_engine_new_with_config(config);
    if (!m_engine) {
        spdlog::error("[WASM] Failed to create Wasmtime engine");
    }
}

Runtime::~Runtime() {
    if (m_engine) wasm_engine_delete(m_engine);
}

Runtime& GetRuntime() {
    static Runtime runtime;
    return runtime;
}

void InstallGameBridge(GetNativeAddressFunc getNativeAddr,
                       GetGlobalPointerFunc getGlobalPtr) {
    g_getNativeAddr = getNativeAddr;
    g_getGlobalPtr = getGlobalPtr;
}

GetNativeAddressFunc GetNativeAddrFn() { return g_getNativeAddr; }
GetGlobalPointerFunc GetGlobalPtrFn() { return g_getGlobalPtr; }

std::string TakeError(wasmtime_error_t* error) {
    if (!error) return {};

    wasm_byte_vec_t message;
    wasmtime_error_message(error, &message);
    std::string result(message.data, message.size);
    wasm_byte_vec_delete(&message);
    wasmtime_error_delete(error);
    return result;
}

std::string TakeTrap(wasm_trap_t* trap) {
    if (!trap) return {};

    wasm_byte_vec_t message;
    wasm_trap_message(trap, &message);
    std::string result(message.data, message.size);
    wasm_byte_vec_delete(&message);
    wasm_trap_delete(trap);
    return result;
}

} // namespace rdr2wasm
