#pragma once

#include <string>
#include <unordered_map>
#include <filesystem>
#include <JavaScriptCore/JavaScript.h>

namespace rdr2js {

/**
 * JSModuleSupport - Provides ES6 module support for JavaScriptCore
 *
 * Handles:
 * - Loading and evaluating ES6 modules
 * - Resolving module imports (built-in modules like 'natives', 'core')
 * - File-based module resolution
 */
class JSModuleSupport {
public:
    static JSModuleSupport& Get();

    /**
     * Initialize module support with built-in module sources
     */
    void Initialize();

    /**
     * Register a built-in module source
     * @param name Module name (e.g., "natives", "core")
     * @param source JavaScript source code
     */
    void RegisterModule(const std::string& name, const std::string& source);

    /**
     * Load built-in modules from a directory
     * @param modulesPath Path containing .js module files
     */
    void LoadModulesFromPath(const std::filesystem::path& modulesPath);

    /**
     * Set the base path for relative module resolution
     */
    void SetModBasePath(const std::filesystem::path& path);

    /**
     * Evaluate a script as an ES6 module
     * @param ctx JavaScript context
     * @param source Module source code
     * @param sourceURL Source URL for error reporting
     * @return true on success
     */
    bool EvaluateModule(JSGlobalContextRef ctx, const std::string& source, const std::string& sourceURL);

    /**
     * Check if a module name is a built-in module
     */
    bool IsBuiltinModule(const std::string& name) const;

    /**
     * Get source code for a built-in module
     */
    const std::string& GetModuleSource(const std::string& name) const;

    /**
     * Get the current mod base path
     */
    const std::filesystem::path& GetModBasePath() const { return m_modBasePath; }

private:
    JSModuleSupport() = default;

    std::unordered_map<std::string, std::string> m_modules;
    std::filesystem::path m_modBasePath;
    bool m_initialized = false;

    static std::string s_emptyString;
};

} // namespace rdr2js
