#include "Mod.h"

#include "Bindings.h"
#include "Logger.h"

#include <fstream>
#include <iterator>
#include <cstring>
#include <vector>

namespace rdr2wasm {

namespace {

std::vector<uint8_t> ReadFile(const std::filesystem::path& path) {
    std::ifstream file(path, std::ios::binary);
    if (!file) return {};
    return { std::istreambuf_iterator<char>(file),
             std::istreambuf_iterator<char>() };
}

} // namespace

Mod::Mod(Runtime& runtime, ModManifest manifest)
    : m_runtime(runtime), m_manifest(std::move(manifest)) {}

Mod::~Mod() {
    if (m_loaded && m_hasShutdown) {
        Call("rdr2_shutdown", m_shutdown);
    }
    Reset();
}

void Mod::Reset() {
    m_loaded = false;
    if (m_store) {
        wasmtime_store_delete(m_store);
        m_store = nullptr;
        m_context = nullptr;
    }
}

bool Mod::LoadEntrypoint() {
    if (!m_runtime.IsValid()) {
        spdlog::error("[WASM:{}] Wasmtime runtime is unavailable", m_manifest.name);
        return false;
    }

    const auto entrypointPath = m_manifest.modPath / m_manifest.entrypoint;
    if (entrypointPath.extension() != ".wasm") {
        spdlog::error("[WASM:{}] Entrypoint must be a .wasm module: {}",
                      m_manifest.name, entrypointPath.string());
        return false;
    }

    auto bytes = ReadFile(entrypointPath);
    if (bytes.empty()) {
        spdlog::error("[WASM:{}] Entrypoint is missing or empty: {}",
                      m_manifest.name, entrypointPath.string());
        return false;
    }

    m_store = wasmtime_store_new(m_runtime.Engine(), this, nullptr);
    if (!m_store) {
        spdlog::error("[WASM:{}] Failed to create store", m_manifest.name);
        return false;
    }
    m_context = wasmtime_store_context(m_store);
    wasmtime_store_limiter(m_store, 64 * 1024 * 1024, 10'000, 1, 4, 2);
    if (auto* error = wasmtime_context_set_fuel(m_context, FuelPerCall)) {
        spdlog::error("[WASM:{}] Failed to set instantiation fuel: {}",
                      m_manifest.name, TakeError(error));
        Reset();
        return false;
    }

    wasmtime_module_t* module = nullptr;
    if (auto* error = wasmtime_module_new(m_runtime.Engine(), bytes.data(),
                                           bytes.size(), &module)) {
        spdlog::error("[WASM:{}] Failed to compile {}: {}", m_manifest.name,
                      entrypointPath.string(), TakeError(error));
        Reset();
        return false;
    }

    wasmtime_linker_t* linker = wasmtime_linker_new(m_runtime.Engine());
    if (!linker) {
        spdlog::error("[WASM:{}] Failed to create linker", m_manifest.name);
        wasmtime_module_delete(module);
        Reset();
        return false;
    }

    bool bindingsReady = bindings::DefineAll(*this, linker);
    wasm_trap_t* trap = nullptr;
    wasmtime_error_t* error = nullptr;
    if (bindingsReady) {
        error = wasmtime_linker_instantiate(linker, m_context, module,
                                             &m_instance, &trap);
    }
    wasmtime_linker_delete(linker);
    wasmtime_module_delete(module);

    if (!bindingsReady) {
        Reset();
        return false;
    }
    if (error || trap) {
        std::string message;
        if (error) message = TakeError(error);
        if (trap) {
            if (!message.empty()) message += "; ";
            message += TakeTrap(trap);
        }
        spdlog::error("[WASM:{}] Failed to instantiate module: {}",
                      m_manifest.name, message);
        Reset();
        return false;
    }

    bool found = false;
    if (!FindFunction("rdr2_abi_version", true, m_abiVersion, found) ||
        !FindFunction("rdr2_init", true, m_init, found) ||
        !FindFunction("rdr2_tick", false, m_tick, m_hasTick) ||
        !FindFunction("rdr2_key_down", false, m_keyDown, m_hasKeyDown) ||
        !FindFunction("rdr2_key_up", false, m_keyUp, m_hasKeyUp) ||
        !FindFunction("rdr2_shutdown", false, m_shutdown, m_hasShutdown) ||
        !CheckAbiVersion() || !Call("rdr2_init", m_init)) {
        Reset();
        return false;
    }

    m_loaded = true;
    return true;
}

bool Mod::FindFunction(const char* name, bool required, wasmtime_func_t& out,
                       bool& found) {
    wasmtime_extern_t item;
    found = wasmtime_instance_export_get(m_context, &m_instance, name,
                                          std::strlen(name), &item);
    if (!found) {
        if (required) {
            spdlog::error("[WASM:{}] Required export '{}' is missing",
                          m_manifest.name, name);
            return false;
        }
        return true;
    }
    if (item.kind != WASMTIME_EXTERN_FUNC) {
        spdlog::error("[WASM:{}] Export '{}' is not a function",
                      m_manifest.name, name);
        return false;
    }
    out = item.of.func;
    return true;
}

bool Mod::Call(const char* name, const wasmtime_func_t& function,
               const wasmtime_val_t* args, size_t argCount,
               wasmtime_val_t* results, size_t resultCount) {
    if (auto* error = wasmtime_context_set_fuel(m_context, FuelPerCall)) {
        spdlog::error("[WASM:{}] Failed to set execution fuel: {}",
                      m_manifest.name, TakeError(error));
        return false;
    }

    wasm_trap_t* trap = nullptr;
    auto* error = wasmtime_func_call(m_context, &function, args, argCount,
                                     results, resultCount, &trap);
    if (!error && !trap) return true;

    std::string message;
    if (error) message = TakeError(error);
    if (trap) {
        if (!message.empty()) message += "; ";
        message += TakeTrap(trap);
    }
    spdlog::error("[WASM:{}] Export '{}' failed: {}", m_manifest.name,
                  name, message);
    return false;
}

bool Mod::CheckAbiVersion() {
    wasmtime_val_t result{};
    if (!Call("rdr2_abi_version", m_abiVersion, nullptr, 0, &result, 1)) {
        return false;
    }
    if (result.kind != WASMTIME_I32 ||
        static_cast<uint32_t>(result.of.i32) != AbiVersion) {
        spdlog::error("[WASM:{}] Unsupported ABI version (expected {})",
                      m_manifest.name, AbiVersion);
        return false;
    }
    return true;
}

void Mod::Tick() {
    if (m_loaded && m_hasTick) Call("rdr2_tick", m_tick);
}

void Mod::OnKeyDown(uint32_t key) {
    if (!m_loaded || !m_hasKeyDown) return;
    wasmtime_val_t arg{};
    arg.kind = WASMTIME_I32;
    arg.of.i32 = static_cast<int32_t>(key);
    Call("rdr2_key_down", m_keyDown, &arg, 1);
}

void Mod::OnKeyUp(uint32_t key) {
    if (!m_loaded || !m_hasKeyUp) return;
    wasmtime_val_t arg{};
    arg.kind = WASMTIME_I32;
    arg.of.i32 = static_cast<int32_t>(key);
    Call("rdr2_key_up", m_keyUp, &arg, 1);
}

} // namespace rdr2wasm
