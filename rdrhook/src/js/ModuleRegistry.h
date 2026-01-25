#pragma once

#include <JavaScriptCore/JavaScript.h>
#include <unordered_map>
#include <string>

namespace rdr2js {

class JSRuntime;

/**
 * ModuleRegistry - Holds references to built-in module objects
 *
 * This allows modules like 'natives' and 'core' to be accessed through
 * ES6 imports without polluting the global namespace.
 *
 * Usage in module loader:
 *   When 'natives' is imported, the loader retrieves the object from the registry
 *   and creates a synthetic module that exports it.
 */
class ModuleRegistry {
public:
    ModuleRegistry() = default;
    ~ModuleRegistry();

    /**
     * Initialize the registry for a context
     * Creates and stores the built-in module objects
     */
    void Initialize(JSGlobalContextRef ctx, JSRuntime* runtime);

    /**
     * Get a module object by name
     * @return The module object, or nullptr if not found
     */
    JSObjectRef GetModule(const std::string& name) const;

    /**
     * Check if a module exists
     */
    bool HasModule(const std::string& name) const;

    /**
     * Clean up all module references
     */
    void Cleanup();

private:
    JSGlobalContextRef m_context = nullptr;
    std::unordered_map<std::string, JSObjectRef> m_modules;
};

} // namespace rdr2js
