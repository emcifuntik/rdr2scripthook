#pragma once

#include "Mod.h"

#include <cstdint>
#include <filesystem>
#include <memory>
#include <string>
#include <vector>

namespace rdr2wasm {

class ModLoader {
public:
    ModLoader() = default;
    ~ModLoader();

    bool Initialize(const std::wstring& basePath);
    int LoadAllMods();

    void TickAll();
    void OnKeyDownAll(uint32_t key);
    void OnKeyUpAll(uint32_t key);
    void UnloadAllMods();

    const std::vector<std::unique_ptr<Mod>>& Mods() const { return m_mods; }
    const std::filesystem::path& ModsPath() const { return m_modsPath; }

private:
    bool ParseManifest(const std::filesystem::path& path, ModManifest& out);
    bool LoadOneMod(const std::filesystem::path& modDirectory);

    std::filesystem::path m_basePath;
    std::filesystem::path m_modsPath;
    std::vector<std::unique_ptr<Mod>> m_mods;
    bool m_initialized = false;
};

ModLoader& GetModLoader();

} // namespace rdr2wasm
