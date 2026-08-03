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

ptrdiff_t LogWasiStdout(void* data, const unsigned char* bytes, size_t length) {
    auto* mod = static_cast<Mod*>(data);
    std::string message(reinterpret_cast<const char*>(bytes), length);
    while (!message.empty() && (message.back() == '\r' || message.back() == '\n'))
        message.pop_back();
    if (!message.empty())
        spdlog::info("[WASM:{}] {}", mod->Manifest().name, message);
    return static_cast<ptrdiff_t>(length);
}

ptrdiff_t LogWasiStderr(void* data, const unsigned char* bytes, size_t length) {
    auto* mod = static_cast<Mod*>(data);
    std::string message(reinterpret_cast<const char*>(bytes), length);
    while (!message.empty() && (message.back() == '\r' || message.back() == '\n'))
        message.pop_back();
    if (!message.empty())
        spdlog::error("[WASM:{}] {}", mod->Manifest().name, message);
    return static_cast<ptrdiff_t>(length);
}

} // namespace

Mod::Mod(Runtime& runtime, ModManifest manifest, const GuestProfile& profile)
    : m_runtime(runtime), m_manifest(std::move(manifest)), m_profile(profile) {}

Mod::~Mod() {
    if (m_loaded && m_hasShutdown) {
        Call(m_profile.shutdownExport.data(), m_shutdown);
    }
    Reset();
}

void Mod::Reset() {
    bindings::ReleaseOwnedResources(*this);
    m_loaded = false;
    if (m_store) {
        wasmtime_store_delete(m_store);
        m_store = nullptr;
        m_context = nullptr;
    }
}

bool Mod::ConfigureRestrictedWasi() {
    auto* wasi = wasi_config_new();
    if (!wasi) {
        spdlog::error("[WASM:{}] Failed to create restricted WASI configuration",
                      m_manifest.name);
        return false;
    }

    // Deliberately do not inherit argv, environment variables, sockets, or
    // preopened directories. The only ambient handles are empty stdin and
    // host-owned log streams.
    wasm_byte_vec_t stdinBytes;
    wasm_byte_vec_new_empty(&stdinBytes);
    wasi_config_set_stdin_bytes(wasi, &stdinBytes);
    wasi_config_set_stdout_custom(wasi, LogWasiStdout, this, nullptr);
    wasi_config_set_stderr_custom(wasi, LogWasiStderr, this, nullptr);
    if (auto* error = wasmtime_context_set_wasi(m_context, wasi)) {
        spdlog::error("[WASM:{}] Failed to configure restricted WASI: {}",
                      m_manifest.name, TakeError(error));
        return false;
    }
    return true;
}

bool Mod::ValidateModuleImports(const wasmtime_module_t* module) const {
    wasm_importtype_vec_t imports;
    wasmtime_module_imports(module, &imports);

    bool valid = true;
    for (size_t index = 0; index < imports.size; ++index) {
        const auto* moduleName = wasm_importtype_module(imports.data[index]);
        const auto* itemName = wasm_importtype_name(imports.data[index]);
        const std::string_view moduleView(
            moduleName && moduleName->data ? moduleName->data : "",
            moduleName ? moduleName->size : 0);
        const std::string_view itemView(
            itemName && itemName->data ? itemName->data : "",
            itemName ? itemName->size : 0);
        if (m_profile.AllowsImportModule(moduleView)) continue;

        spdlog::error(
            "[WASM:{}] Rejected import '{}::{}' for runtime profile '{}'",
            m_manifest.name, moduleView, itemView, m_profile.id);
        valid = false;
    }

    wasm_importtype_vec_delete(&imports);
    return valid;
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
    if (auto* error = wasmtime_context_set_fuel(m_context,
                                                 m_profile.fuelPerCall)) {
        spdlog::error("[WASM:{}] Failed to set instantiation fuel: {}",
                      m_manifest.name, TakeError(error));
        Reset();
        return false;
    }

    if (m_profile.wasi == WasiPolicy::Restricted &&
        !ConfigureRestrictedWasi()) {
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

    if (!ValidateModuleImports(module)) {
        wasmtime_module_delete(module);
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
    if (bindingsReady && m_profile.wasi == WasiPolicy::Restricted) {
        if (auto* error = wasmtime_linker_define_wasi(linker)) {
            spdlog::error("[WASM:{}] Failed to define WASI imports: {}",
                          m_manifest.name, TakeError(error));
            bindingsReady = false;
        }
    }
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
    wasmtime_func_t startup{};
    bool hasStartup = false;
    if (!FindFunction(m_profile.startupExport.data(), false,
                      startup, hasStartup) ||
        (hasStartup && !Call(m_profile.startupExport.data(), startup))) {
        Reset();
        return false;
    }
    if (m_profile.RequiresAbiVersion() &&
        !FindFunction(m_profile.abiVersionExport.data(), true,
                      m_abiVersion, found)) {
        Reset();
        return false;
    }
    if (!FindFunction(m_profile.initExport.data(), true, m_init, found) ||
        !FindFunction(m_profile.tickExport.data(), false, m_tick, m_hasTick) ||
        !FindFunction(m_profile.keyDownExport.data(), false,
                      m_keyDown, m_hasKeyDown) ||
        !FindFunction(m_profile.keyUpExport.data(), false,
                      m_keyUp, m_hasKeyUp) ||
        !FindFunction(m_profile.shutdownExport.data(), false,
                      m_shutdown, m_hasShutdown) ||
        (m_profile.RequiresAbiVersion() && !CheckAbiVersion()) ||
        !Call(m_profile.initExport.data(), m_init)) {
        Reset();
        return false;
    }

    m_loaded = true;
    return true;
}

bool Mod::FindFunction(const char* name, bool required, wasmtime_func_t& out,
                       bool& found) {
    if (!name || *name == '\0') {
        found = false;
        return true;
    }
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
    if (auto* error = wasmtime_context_set_fuel(m_context,
                                                 m_profile.fuelPerCall)) {
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
    if (!Call(m_profile.abiVersionExport.data(), m_abiVersion, nullptr, 0,
              &result, 1)) {
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

bool Mod::Tick() {
    if (!m_loaded) return false;
    if (!m_hasTick) return true;
    if (Call(m_profile.tickExport.data(), m_tick)) return true;

    // A trapped Wasm function cannot be resumed reliably. Disable only the
    // failing tick callback so the mod can still receive shutdown handling.
    m_hasTick = false;
    return false;
}

void Mod::OnKeyDown(uint32_t key) {
    if (!m_loaded || !m_hasKeyDown) return;
    wasmtime_val_t arg{};
    arg.kind = WASMTIME_I32;
    arg.of.i32 = static_cast<int32_t>(key);
    Call(m_profile.keyDownExport.data(), m_keyDown, &arg, 1);
}

void Mod::OnKeyUp(uint32_t key) {
    if (!m_loaded || !m_hasKeyUp) return;
    wasmtime_val_t arg{};
    arg.kind = WASMTIME_I32;
    arg.of.i32 = static_cast<int32_t>(key);
    Call(m_profile.keyUpExport.data(), m_keyUp, &arg, 1);
}

} // namespace rdr2wasm
