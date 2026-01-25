#pragma once

#include <string>
#include <vector>
#include <memory>
#include <filesystem>
#include <functional>

namespace rdr2js {

class JSMod;
class JSRuntime;

/**
 * Mod manifest structure parsed from mod.toml
 *
 * Example mod.toml:
 * ```toml
 * [mod]
 * name = "My Awesome Mod"
 * version = "1.0.0"
 * author = "AuthorName"
 * description = "A cool mod for RDR2"
 * entrypoint = "main.js"
 *
 * [dependencies]
 * # Optional: other mods this mod depends on
 * ```
 */
struct ModManifest {
    std::string name;
    std::string version;
    std::string author;
    std::string description;
    std::string entrypoint;
    std::filesystem::path modPath;

    bool IsValid() const {
        return !name.empty() && !entrypoint.empty();
    }
};

/**
 * Loaded mod information
 *
 * With the dynamic callback system, mods register their callbacks using:
 * - core.addTickCallback(fn) / core.removeTickCallback(id)
 * - core.addKeyDownCallback(fn) / core.removeKeyDownCallback(id)
 * - core.addKeyUpCallback(fn) / core.removeKeyUpCallback(id)
 * - setTimeout(fn, ms) / clearTimeout(id)
 * - setInterval(fn, ms) / clearInterval(id)
 */
struct LoadedMod {
    ModManifest manifest;
    std::unique_ptr<JSMod> jsMod;
};

/**
 * ModLoader - Loads and manages JavaScript mods from the mods directory
 *
 * Directory structure:
 * mods/
 *   my-mod/
 *     mod.toml       <- Manifest file
 *     main.js        <- Entry point (specified in manifest)
 *     other.js       <- Additional scripts (can be imported)
 *   another-mod/
 *     mod.toml
 *     index.js
 */
class ModLoader {
public:
    ModLoader();
    ~ModLoader();

    /**
     * Initialize the mod loader with the base path
     * @param basePath Path to the directory containing 'mods' folder
     * @param runtime JavaScript runtime instance
     */
    bool Initialize(const std::wstring& basePath, JSRuntime* runtime);

    /**
     * Scan and load all mods from the mods directory
     * @return Number of mods successfully loaded
     */
    int LoadAllMods();

    /**
     * Get list of loaded mods
     */
    const std::vector<LoadedMod>& GetLoadedMods() const { return m_loadedMods; }

    /**
     * Call init() on all loaded mods
     */
    void CallAllInit();

    /**
     * Call tick() on all loaded mods
     */
    void CallAllTick();

    /**
     * Call onKeyDown() on all loaded mods
     */
    void CallAllKeyDown(uint32_t key);

    /**
     * Call onKeyUp() on all loaded mods
     */
    void CallAllKeyUp(uint32_t key);

    /**
     * Process all timers (setTimeout/setInterval)
     * @param gameTime Current game time in milliseconds
     */
    void ProcessAllTimers(uint32_t gameTime);

    /**
     * Unload all mods
     */
    void UnloadAllMods();

    /**
     * Get the mods directory path
     */
    const std::filesystem::path& GetModsPath() const { return m_modsPath; }

private:
    /**
     * Parse a mod.toml manifest file
     */
    bool ParseManifest(const std::filesystem::path& manifestPath, ModManifest& manifest);

    /**
     * Load a single mod from its directory
     */
    bool LoadMod(const std::filesystem::path& modDir);

    /**
     * Read file contents as string
     */
    std::string ReadFileContents(const std::filesystem::path& path);

    std::filesystem::path m_basePath;
    std::filesystem::path m_modsPath;
    JSRuntime* m_runtime = nullptr;
    std::vector<LoadedMod> m_loadedMods;
    bool m_initialized = false;
};

/**
 * Get the global ModLoader instance
 */
ModLoader& GetModLoader();

} // namespace rdr2js
