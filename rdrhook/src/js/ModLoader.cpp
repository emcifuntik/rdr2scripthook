#include "stdafx.h"
#include "ModLoader.h"
#include "Logger.h"

#include <toml.hpp>

namespace rdr2js {

namespace {
std::unique_ptr<ModLoader> g_loader;
}

ModLoader& GetModLoader() {
    if (!g_loader) g_loader = std::make_unique<ModLoader>();
    return *g_loader;
}

ModLoader::ModLoader() = default;
ModLoader::~ModLoader() { UnloadAllMods(); }

bool ModLoader::Initialize(const std::wstring& basePath) {
    if (m_initialized) return true;

    m_basePath = std::filesystem::path(basePath);
    m_modsPath = m_basePath / L"mods";

    if (!std::filesystem::exists(m_modsPath)) {
        try {
            std::filesystem::create_directories(m_modsPath);
            spdlog::info("[ModLoader] Created mods directory: {}", m_modsPath.string());
        } catch (const std::exception& e) {
            spdlog::error("[ModLoader] Failed to create mods directory: {}", e.what());
            return false;
        }
    }

    m_initialized = true;
    spdlog::info("[ModLoader] Initialized with mods path: {}", m_modsPath.string());
    return true;
}

bool ModLoader::ParseManifest(const std::filesystem::path& manifestPath, ModManifest& out) {
    try {
        auto data = toml::parse(manifestPath.string());

        if (data.contains("mod")) {
            const auto& s = toml::find(data, "mod");
            if (s.contains("name"))        out.name        = toml::find<std::string>(s, "name");
            if (s.contains("version"))     out.version     = toml::find<std::string>(s, "version");
            if (s.contains("author"))      out.author      = toml::find<std::string>(s, "author");
            if (s.contains("description")) out.description = toml::find<std::string>(s, "description");
            if (s.contains("entrypoint"))  out.entrypoint  = toml::find<std::string>(s, "entrypoint");
        }

        out.modPath = manifestPath.parent_path();

        if (!out.IsValid()) {
            spdlog::error("[ModLoader] Invalid manifest: name and entrypoint are required ({})",
                          manifestPath.string());
            return false;
        }
        return true;
    } catch (const toml::syntax_error& e) {
        spdlog::error("[ModLoader] TOML syntax error in {}: {}", manifestPath.string(), e.what());
        return false;
    } catch (const std::exception& e) {
        spdlog::error("[ModLoader] Error parsing manifest {}: {}", manifestPath.string(), e.what());
        return false;
    }
}

bool ModLoader::LoadOneMod(const std::filesystem::path& modDir) {
    ModManifest manifest;
    if (!ParseManifest(modDir / "mod.toml", manifest)) return false;

    spdlog::info("[ModLoader] Loading mod: {} v{} by {}",
                 manifest.name, manifest.version, manifest.author);

    auto mod = std::make_unique<Mod>(GetRuntime(), std::move(manifest));
    if (!mod->LoadEntrypoint()) {
        spdlog::error("[ModLoader] Failed to load entrypoint for: {}", mod->Manifest().name);
        return false;
    }

    spdlog::info("[ModLoader] Loaded: {}", mod->Manifest().name);
    m_mods.push_back(std::move(mod));
    return true;
}

int ModLoader::LoadAllMods() {
    if (!m_initialized) {
        spdlog::error("[ModLoader] Not initialized");
        return 0;
    }

    int loaded = 0;
    try {
        for (const auto& entry : std::filesystem::directory_iterator(m_modsPath)) {
            if (!entry.is_directory()) continue;
            auto manifestPath = entry.path() / "mod.toml";
            if (!std::filesystem::exists(manifestPath)) continue;
            if (LoadOneMod(entry.path())) ++loaded;
        }
    } catch (const std::exception& e) {
        spdlog::error("[ModLoader] Error scanning mods directory: {}", e.what());
    }

    spdlog::info("[ModLoader] Loaded {} JavaScript mod(s)", loaded);
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
    for (auto& mod : m_mods) {
        spdlog::info("[ModLoader] Unloading mod: {}", mod->Manifest().name);
    }
    m_mods.clear();
}

} // namespace rdr2js
