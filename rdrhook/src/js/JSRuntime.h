#pragma once

#include <string>
#include <functional>
#include <memory>
#include <vector>
#include <unordered_map>

#include <JavaScriptCore/JSBase.h>

namespace rdr2js {


// Function pointer types for game integration
using GetNativeAddressFunc = uintptr_t(*)(uint64_t hash);
using GetGlobalPointerFunc = void*(*)(uint32_t globalVarId);

// JavaScript callback types
using JSNativeCallback = JSValueRef(*)(JSContextRef ctx, JSObjectRef function,
    JSObjectRef thisObject, size_t argumentCount, const JSValueRef arguments[], JSValueRef* exception);

// Forward declarations
class JSMod;

/**
 * JSRuntime - Wrapper around JavaScriptCore for RDR2 Script Hook
 *
 * Provides a simplified interface for:
 * - Creating and managing JavaScript contexts
 * - Executing JavaScript code
 * - Binding native functions to JavaScript
 * - Managing script lifecycle (init, tick, events)
 */
class JSRuntime {
public:
    JSRuntime();
    ~JSRuntime();

    // Disable copy
    JSRuntime(const JSRuntime&) = delete;
    JSRuntime& operator=(const JSRuntime&) = delete;

    /**
     * Initialize the JavaScript runtime with game function pointers
     * @param getNativeAddr Function to get native function addresses
     * @param getGlobalPtr Function to get global variable pointers
     */
    bool Initialize(GetNativeAddressFunc getNativeAddr, GetGlobalPointerFunc getGlobalPtr);

    /**
     * Shutdown the runtime and release all resources
     */
    void Shutdown();

    /**
     * Create a new JavaScript context for a mod
     * @param modName Name of the mod (for debugging/logging)
     * @return Unique pointer to JSMod or nullptr on failure
     */
    std::unique_ptr<JSMod> CreateModContext(const std::string& modName);

    /**
     * Execute JavaScript code in a context (script mode - no import/export)
     * @param ctx JavaScript context
     * @param code JavaScript source code
     * @param sourceURL Source URL for error reporting (optional)
     * @return true if execution succeeded
     */
    bool ExecuteScript(JSContextRef ctx, const std::string& code, const std::string& sourceURL = "");

    /**
     * Execute JavaScript code as an ES module (with import/export support)
     * @param ctx JavaScript context
     * @param code JavaScript module source code
     * @param moduleKey Module identifier/path
     * @return Module namespace object on success, nullptr on failure
     */
    JSObjectRef ExecuteModule(JSContextRef ctx, const std::string& code, const std::string& moduleKey);

    /**
     * Register a module source for later import (internal cache)
     * @param moduleKey Module identifier/path
     * @param code Module source code
     */
    void RegisterModuleSource(const std::string& moduleKey, const std::string& code);

    /**
     * Provide a module to JSC's module loader for a specific context
     * This must be called before loading modules that import it
     * @param ctx JavaScript context
     * @param moduleKey Module identifier (e.g., "natives", "core")
     * @param code Module source code
     */
    void ProvideModule(JSContextRef ctx, const std::string& moduleKey, const std::string& code);

    /**
     * Call a JavaScript function by name
     * @param ctx JavaScript context
     * @param functionName Name of the function to call
     * @param args Arguments to pass
     * @return Result value or nullptr on error
     */
    JSValueRef CallFunction(JSContextRef ctx, const std::string& functionName,
        const std::vector<JSValueRef>& args = {});

    /**
     * Check if a function exists in the global scope
     */
    bool HasFunction(JSContextRef ctx, const std::string& functionName);

    /**
     * Get the last error message
     */
    const std::string& GetLastError() const { return m_lastError; }

    // Game function accessors (for bindings)
    GetNativeAddressFunc GetNativeAddrFn() const { return m_getNativeAddr; }
    GetGlobalPointerFunc GetGlobalPtrFn() const { return m_getGlobalPtr; }

    /**
     * Drain microtask queue (needed after module evaluation and before context release)
     */
    void DrainMicrotasks(JSContextRef ctx);

    /**
     * Clean up module sources for a context (called when context is destroyed)
     */
    void CleanupContext(JSContextRef ctx);

private:
    /**
     * Register native bindings on a context
     */
    void RegisterNativeBindings(JSGlobalContextRef ctx);

    /**
     * Convert JSValueRef to string for error handling
     */
    std::string JSValueToString(JSContextRef ctx, JSValueRef value);

    /**
     * Format an exception with line/column details
     */
    std::string FormatException(JSContextRef ctx, JSValueRef exception);

    /**
     * Get module source code by key (for module loader callbacks)
     */
    const std::string* GetModuleSource(const std::string& moduleKey) const;

    GetNativeAddressFunc m_getNativeAddr = nullptr;
    GetGlobalPointerFunc m_getGlobalPtr = nullptr;
    bool m_initialized = false;
    std::string m_lastError;

    // Module source registry: moduleKey -> source code
    std::unordered_map<std::string, std::string> m_moduleSources;
};

/**
 * JSMod - Represents a single JavaScript mod with its own context
 */
class JSMod {
public:
    JSMod(JSGlobalContextRef ctx, const std::string& name, JSRuntime* runtime);
    ~JSMod();

    // Disable copy
    JSMod(const JSMod&) = delete;
    JSMod& operator=(const JSMod&) = delete;

    /**
     * Load and execute JavaScript code as a script (no import/export)
     */
    bool LoadScript(const std::string& code, const std::string& sourceURL = "");

    /**
     * Load and execute JavaScript code as an ES module (with import/export)
     */
    bool LoadModule(const std::string& code, const std::string& moduleKey = "");

    /**
     * Call the mod's init function if it exists
     */
    bool CallInit();

    /**
     * Call the mod's tick function if it exists
     */
    bool CallTick();

    /**
     * Call the mod's onKeyDown function if it exists
     */
    bool CallOnKeyDown(uint32_t key);

    /**
     * Call the mod's onKeyUp function if it exists
     */
    bool CallOnKeyUp(uint32_t key);

    /**
     * Get the JavaScript context
     */
    JSGlobalContextRef GetContext() const { return m_context; }

    /**
     * Get the mod name
     */
    const std::string& GetName() const { return m_name; }

    /**
     * Check if the mod has a specific callback (checks module exports)
     */
    bool HasCallback(const std::string& name);

    /**
     * Get the module namespace object (for accessing exports)
     */
    JSObjectRef GetModuleNamespace() const { return m_moduleNamespace; }

private:
    /**
     * Call an exported function from the module namespace
     */
    JSValueRef CallExportedFunction(const std::string& name, const std::vector<JSValueRef>& args = {});

    JSGlobalContextRef m_context;
    std::string m_name;
    JSRuntime* m_runtime;
    JSObjectRef m_moduleNamespace = nullptr;  // Module exports namespace
};

// Global runtime instance
JSRuntime& GetJSRuntime();

} // namespace rdr2js
