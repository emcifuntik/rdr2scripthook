#pragma once

#include <JavaScriptCore/JavaScript.h>
#include <cstdint>
#include <functional>

namespace rdr2js {

// Callback ID type
using CallbackId = uint32_t;

/**
 * Initialize the core module system
 * Must be called before registering any callbacks
 */
void InitCoreModule();

/**
 * Create the 'core' module object (without registering as global)
 * Returns an object with: addTickCallback, removeTickCallback, addKeyDownCallback, etc.
 * Used by ModuleRegistry for ES6 module imports
 */
JSObjectRef CreateCoreModuleObject(JSGlobalContextRef ctx);

/**
 * Register timer functions (setTimeout/setInterval) on global
 * These are standard browser-like APIs that should be globally available
 */
void RegisterGlobalTimers(JSGlobalContextRef ctx);

/**
 * Execute all registered tick callbacks
 * Called once per frame from the main game loop
 */
void ExecuteTickCallbacks(JSGlobalContextRef ctx);

/**
 * Execute key down callbacks for a specific key
 * @param key Virtual key code
 */
void ExecuteKeyDownCallbacks(JSGlobalContextRef ctx, int key);

/**
 * Execute key up callbacks for a specific key
 * @param key Virtual key code
 */
void ExecuteKeyUpCallbacks(JSGlobalContextRef ctx, int key);

/**
 * Process timers (setTimeout/setInterval)
 * @param currentTime Current game time in milliseconds
 */
void ProcessTimers(JSGlobalContextRef ctx, uint32_t currentTime);

/**
 * Clear all callbacks for a context (called when mod is unloaded)
 */
void ClearAllCallbacks(JSGlobalContextRef ctx);

/**
 * Get count of registered tick callbacks (for debugging)
 */
size_t GetTickCallbackCount();

/**
 * Get count of active timers (for debugging)
 */
size_t GetActiveTimerCount();

} // namespace rdr2js
