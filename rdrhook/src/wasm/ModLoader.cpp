#include "ModLoader.h"

#include "Logger.h"

#include <toml.hpp>

namespace rdr2wasm {

ModLoader& GetModLoader() {
    static ModLoader loader;
    return loader;
}

ModLoader::~ModLoader() {
    UnloadAllMods();
}

bool ModLoader::Initialize(const std::wstring& basePath) {
    if (m_initialized) return true;

    m_basePath = std::filesystem::path(basePath);
    m_modsPath = m_basePath / L"mods";
    try {
        std::filesystem::create_directories(m_modsPath);
    } catch (const std::exception& exception) {
        spdlog::error("[WASM] Failed to create mods directory '{}': {}",
                      m_modsPath.string(), exception.what());
        return false;
    }

    m_initialized = true;
    spdlog::info("[WASM] Mod directory: {}", m_modsPath.string());
    return true;
}

bool ModLoader::ParseManifest(const std::filesystem::path& path,
                              ModManifest& out) {
    try {
        const auto data = toml::parse(path.string());
        if (data.contains("mod")) {
            const auto& mod = toml::find(data, "mod");
            if (mod.contains("name"))
                out.name = toml::find<std::string>(mod, "name");
            if (mod.contains("version"))
                out.version = toml::find<std::string>(mod, "version");
            if (mod.contains("author"))
                out.author = toml::find<std::string>(mod, "author");
            if (mod.contains("description"))
                out.description = toml::find<std::string>(mod, "description");
            if (mod.contains("entrypoint"))
                out.entrypoint = toml::find<std::string>(mod, "entrypoint");
            if (mod.contains("runtime"))
                out.runtime = toml::find<std::string>(mod, "runtime");
        }
        out.modPath = path.parent_path();

        if (!out.IsValid()) {
            spdlog::error("[WASM] Invalid manifest (name and entrypoint are required): {}",
                          path.string());
            return false;
        }
        if (out.runtime != "wasmtime" && out.runtime != "javy") {
            spdlog::error("[WASM] Invalid runtime '{}' in {}",
                          out.runtime, path.string());
            return false;
        }
        return true;
    } catch (const std::exception& exception) {
        spdlog::error("[WASM] Failed to parse '{}': {}", path.string(),
                      exception.what());
        return false;
    }
}

bool ModLoader::LoadOneMod(const std::filesystem::path& modDirectory) {
    ModManifest manifest;
    if (!ParseManifest(modDirectory / "mod.toml", manifest)) return false;

    spdlog::info("[WASM] Loading {} v{} by {}", manifest.name,
                 manifest.version, manifest.author);
    auto mod = std::make_unique<Mod>(GetRuntime(), std::move(manifest));
    if (!mod->LoadEntrypoint()) return false;

    spdlog::info("[WASM] Loaded {}", mod->Manifest().name);
    m_mods.push_back(std::move(mod));
    return true;
}

int ModLoader::LoadAllMods() {
    if (!m_initialized) {
        spdlog::error("[WASM] Mod loader is not initialized");
        return 0;
    }

    int loaded = 0;
    try {
        for (const auto& entry : std::filesystem::directory_iterator(m_modsPath)) {
            if (!entry.is_directory()) continue;
            if (!std::filesystem::exists(entry.path() / "mod.toml")) continue;
            if (LoadOneMod(entry.path())) ++loaded;
        }
    } catch (const std::exception& exception) {
        spdlog::error("[WASM] Failed to scan mods directory: {}", exception.what());
    }

    spdlog::info("[WASM] Loaded {} mod(s)", loaded);
    return loaded;
}

void ModLoader::TickAll() {
    for (auto& mod : m_mods) mod->Tick();
}

void ModLoader::OnKeyDownAll(uint32_t key) {
    for (auto& mod : m_mods) mod->OnKeyDown(key);
}

void ModLoader::OnKeyUpAll(uint32_t key) {
    for (auto& mod : m_mods) mod->OnKeyUp(key);
}

void ModLoader::UnloadAllMods() {
    for (const auto& mod : m_mods) {
        spdlog::info("[WASM] Unloading {}", mod->Manifest().name);
    }
    m_mods.clear();
}

} // namespace rdr2wasm
