#pragma once

#include "GuestProfile.h"
#include "Runtime.h"

#include <cstdint>
#include <filesystem>
#include <string>

namespace rdr2wasm {

struct ModManifest {
    std::string name;
    std::string version;
    std::string author;
    std::string description;
    std::string entrypoint;
    std::string runtime = "wasmtime";
    std::filesystem::path modPath;

    bool IsValid() const { return !name.empty() && !entrypoint.empty(); }
};

class Mod {
public:
    Mod(Runtime& runtime, ModManifest manifest, const GuestProfile& profile);
    ~Mod();

    Mod(const Mod&) = delete;
    Mod& operator=(const Mod&) = delete;

    bool LoadEntrypoint();

    bool Tick();
    void OnKeyDown(uint32_t key);
    void OnKeyUp(uint32_t key);

    const ModManifest& Manifest() const noexcept { return m_manifest; }
    bool IsLoaded() const noexcept { return m_loaded; }

private:
    bool FindFunction(const char* name, bool required, wasmtime_func_t& out,
                      bool& found);
    bool Call(const char* name, const wasmtime_func_t& function,
              const wasmtime_val_t* args = nullptr, size_t argCount = 0,
              wasmtime_val_t* results = nullptr, size_t resultCount = 0);
    bool CheckAbiVersion();
    bool ValidateModuleImports(const wasmtime_module_t* module) const;
    bool ConfigureRestrictedWasi();
    void Reset();

    static constexpr uint32_t AbiVersion = 1;

    Runtime& m_runtime;
    ModManifest m_manifest;
    const GuestProfile& m_profile;
    wasmtime_store_t* m_store = nullptr;
    wasmtime_context_t* m_context = nullptr;
    wasmtime_instance_t m_instance{};
    wasmtime_func_t m_init{};
    wasmtime_func_t m_tick{};
    wasmtime_func_t m_keyDown{};
    wasmtime_func_t m_keyUp{};
    wasmtime_func_t m_shutdown{};
    wasmtime_func_t m_abiVersion{};
    bool m_hasTick = false;
    bool m_hasKeyDown = false;
    bool m_hasKeyUp = false;
    bool m_hasShutdown = false;
    bool m_loaded = false;
};

} // namespace rdr2wasm
