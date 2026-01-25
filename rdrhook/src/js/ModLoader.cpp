#include "stdafx.h"
#include "ModLoader.h"
#include "JSRuntime.h"
#include "JSCoreModule.h"
#include "Logger.h"

#include <JavaScriptCore/JavaScript.h>
#include <toml.hpp>
#include <fstream>
#include <sstream>

namespace rdr2js {

// Global mod loader instance
static std::unique_ptr<ModLoader> g_modLoader;

ModLoader& GetModLoader() {
    if (!g_modLoader) {
        g_modLoader = std::make_unique<ModLoader>();
    }
    return *g_modLoader;
}

ModLoader::ModLoader() = default;
ModLoader::~ModLoader() {
    UnloadAllMods();
}

bool ModLoader::Initialize(const std::wstring& basePath, JSRuntime* runtime) {
    if (m_initialized) {
        return true;
    }

    m_runtime = runtime;
    m_basePath = std::filesystem::path(basePath);
    m_modsPath = m_basePath / L"mods";

    // Create mods directory if it doesn't exist
    if (!std::filesystem::exists(m_modsPath)) {
        try {
            std::filesystem::create_directories(m_modsPath);
            spdlog::info("[ModLoader] Created mods directory: {}", m_modsPath.string());
        } catch (const std::exception& e) {
            spdlog::error("[ModLoader] Failed to create mods directory: {}", e.what());
            return false;
        }
    }

    // Built-in modules (natives, core) are registered via internal globals
    // (__natives__, __core__) in JSNativeBindings and accessed via ES6 imports

    m_initialized = true;
    spdlog::info("[ModLoader] Initialized with mods path: {}", m_modsPath.string());
    return true;
}

int ModLoader::LoadAllMods() {
    if (!m_initialized) {
        spdlog::error("[ModLoader] Not initialized");
        return 0;
    }

    int loadedCount = 0;

    try {
        for (const auto& entry : std::filesystem::directory_iterator(m_modsPath)) {
            if (entry.is_directory()) {
                // Check if mod.toml exists in this directory
                auto manifestPath = entry.path() / "mod.toml";
                if (std::filesystem::exists(manifestPath)) {
                    if (LoadMod(entry.path())) {
                        loadedCount++;
                    }
                }
            }
        }
    } catch (const std::exception& e) {
        spdlog::error("[ModLoader] Error scanning mods directory: {}", e.what());
    }

    spdlog::info("[ModLoader] Loaded {} JavaScript mod(s)", loadedCount);
    return loadedCount;
}

bool ModLoader::ParseManifest(const std::filesystem::path& manifestPath, ModManifest& manifest) {
    try {
        auto data = toml::parse(manifestPath.string());

        // Parse [mod] section
        if (data.contains("mod")) {
            const auto& modSection = toml::find(data, "mod");

            if (modSection.contains("name")) {
                manifest.name = toml::find<std::string>(modSection, "name");
            }
            if (modSection.contains("version")) {
                manifest.version = toml::find<std::string>(modSection, "version");
            }
            if (modSection.contains("author")) {
                manifest.author = toml::find<std::string>(modSection, "author");
            }
            if (modSection.contains("description")) {
                manifest.description = toml::find<std::string>(modSection, "description");
            }
            if (modSection.contains("entrypoint")) {
                manifest.entrypoint = toml::find<std::string>(modSection, "entrypoint");
            }
        }

        manifest.modPath = manifestPath.parent_path();

        if (!manifest.IsValid()) {
            spdlog::error("[ModLoader] Invalid manifest: name and entrypoint are required");
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

bool ModLoader::LoadMod(const std::filesystem::path& modDir) {
    ModManifest manifest;
    auto manifestPath = modDir / "mod.toml";

    if (!ParseManifest(manifestPath, manifest)) {
        return false;
    }

    spdlog::info("[ModLoader] Loading mod: {} v{} by {}", manifest.name, manifest.version, manifest.author);

    // Read the entrypoint script
    auto entrypointPath = modDir / manifest.entrypoint;
    if (!std::filesystem::exists(entrypointPath)) {
        spdlog::error("[ModLoader] Entrypoint not found: {}", entrypointPath.string());
        return false;
    }

    std::string scriptCode = ReadFileContents(entrypointPath);
    if (scriptCode.empty()) {
        spdlog::error("[ModLoader] Failed to read entrypoint script");
        return false;
    }

    // Create JavaScript context for this mod
    auto jsMod = m_runtime->CreateModContext(manifest.name);
    if (!jsMod) {
        spdlog::error("[ModLoader] Failed to create JS context for mod: {}", manifest.name);
        return false;
    }

    // Inject mod info as global variables (accessible from modules)
    JSGlobalContextRef ctx = jsMod->GetContext();
    JSObjectRef globalObj = JSContextGetGlobalObject(ctx);

    auto setGlobalString = [&](const char* name, const std::string& value) {
        JSStringRef jsName = JSStringCreateWithUTF8CString(name);
        JSStringRef jsValue = JSStringCreateWithUTF8CString(value.c_str());
        JSObjectSetProperty(ctx, globalObj, jsName, JSValueMakeString(ctx, jsValue),
            kJSPropertyAttributeReadOnly | kJSPropertyAttributeDontDelete, nullptr);
        JSStringRelease(jsName);
        JSStringRelease(jsValue);
    };

    setGlobalString("__MOD_NAME__", manifest.name);
    setGlobalString("__MOD_VERSION__", manifest.version);
    setGlobalString("__MOD_AUTHOR__", manifest.author);
    setGlobalString("__MOD_PATH__", modDir.string());

    // Built-in modules (natives, core) are now handled by JSCoreModule and JSNativesGenerated
    // They're registered as global objects and have synthetic module wrappers for ES6 imports
    // No need to provide them here - the module loader intercepts these requests

    // Execute the main script as an ES6 module (supports import/export)
    // Callbacks are registered dynamically via core.addTickCallback() etc.
    if (!jsMod->LoadModule(scriptCode, modDir.string() + "/" + manifest.entrypoint)) {
        spdlog::error("[ModLoader] Failed to execute mod script: {}", manifest.name);
        return false;
    }

    // Create loaded mod entry
    // With dynamic callbacks, we just store the mod context
    // Callbacks are registered by the mod itself using core.addTickCallback() etc.
    LoadedMod loadedMod;
    loadedMod.manifest = std::move(manifest);
    loadedMod.jsMod = std::move(jsMod);

    spdlog::debug("[ModLoader] Mod loaded with dynamic callback system (tick callbacks: {})",
        GetTickCallbackCount());

    m_loadedMods.push_back(std::move(loadedMod));

    spdlog::info("[ModLoader] Successfully loaded: {}", m_loadedMods.back().manifest.name);
    return true;
}

std::string ModLoader::ReadFileContents(const std::filesystem::path& path) {
    try {
        std::ifstream file(path, std::ios::binary);
        if (!file.is_open()) {
            return "";
        }

        std::ostringstream ss;
        ss << file.rdbuf();
        return ss.str();
    } catch (const std::exception& e) {
        spdlog::error("[ModLoader] Error reading file {}: {}", path.string(), e.what());
        return "";
    }
}

void ModLoader::CallAllInit() {
    // Init is no longer a special callback - mods register their initialization
    // code directly in the module body or via setTimeout(fn, 0)
    spdlog::debug("[ModLoader] Init phase complete - {} mods loaded", m_loadedMods.size());
}

void ModLoader::CallAllTick() {
    // Use the new dynamic callback system
    for (auto& mod : m_loadedMods) {
        if (mod.jsMod) {
            ExecuteTickCallbacks(mod.jsMod->GetContext());
        }
    }
}

void ModLoader::CallAllKeyDown(uint32_t key) {
    // Use the new dynamic callback system
    for (auto& mod : m_loadedMods) {
        if (mod.jsMod) {
            ExecuteKeyDownCallbacks(mod.jsMod->GetContext(), static_cast<int>(key));
        }
    }
}

void ModLoader::CallAllKeyUp(uint32_t key) {
    // Use the new dynamic callback system
    for (auto& mod : m_loadedMods) {
        if (mod.jsMod) {
            ExecuteKeyUpCallbacks(mod.jsMod->GetContext(), static_cast<int>(key));
        }
    }
}

void ModLoader::ProcessAllTimers(uint32_t gameTime) {
    // Process timers (setTimeout/setInterval) for all mods
    for (auto& mod : m_loadedMods) {
        if (mod.jsMod) {
            ProcessTimers(mod.jsMod->GetContext(), gameTime);
        }
    }
}

void ModLoader::UnloadAllMods() {
    for (auto& mod : m_loadedMods) {
        spdlog::info("[ModLoader] Unloading mod: {}", mod.manifest.name);
        if (mod.jsMod) {
            // Clear all callbacks for this context
            ClearAllCallbacks(mod.jsMod->GetContext());
        }
        mod.jsMod.reset();
    }
    m_loadedMods.clear();
}

} // namespace rdr2js
