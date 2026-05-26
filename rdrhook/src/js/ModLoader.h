#pragma once

#include "Mod.h"

#include <cstdint>
#include <filesystem>
#include <memory>
#include <string>
#include <vector>

namespace rdr2js {

// Scans the mods/ folder under the given base path for `<mod>/mod.toml`
// manifests, loads each one's entrypoint as an ES module, and provides
// per-frame dispatch.
class ModLoader {
public:
    ModLoader();
    ~ModLoader();

    bool Initialize(const std::wstring& basePath);

    // Walk m_modsPath looking for directories containing mod.toml. Returns
    // the number of mods loaded successfully.
    int LoadAllMods();

    void TickAll();
    void OnKeyDownAll(uint32_t key);
    void OnKeyUpAll(uint32_t key);

    void UnloadAllMods();

    const std::vector<std::unique_ptr<Mod>>& Mods() const { return m_mods; }
    const std::filesystem::path& ModsPath() const { return m_modsPath; }

private:
    bool ParseManifest(const std::filesystem::path& manifestPath, ModManifest& out);
    bool LoadOneMod(const std::filesystem::path& modDir);

    std::filesystem::path m_basePath;
    std::filesystem::path m_modsPath;
    std::vector<std::unique_ptr<Mod>> m_mods;
    bool m_initialized = false;
};

// Singleton accessor.
ModLoader& GetModLoader();

} // namespace rdr2js
