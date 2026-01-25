#pragma once

#include "JSRuntime.h"

// Forward declarations
typedef struct OpaqueJSContext* JSGlobalContextRef;

namespace rdr2js {

class JSRuntime;

/**
 * Register all native bindings on a JavaScript context
 *
 * This sets up the following global objects and functions:
 *
 * - Native.invoke(hash, ...args) - Call a native function (low-level)
 * - Native.invokeFloat/invokeVector3 - Type-specific native calls
 *
 * - Global.getInt(id) / setInt(id, value) - Get/set game globals
 * - Global.getFloat(id) / setFloat(id, value)
 *
 * - console.log/warn/error(...args) - Logging
 * - Hash.joaat(str) - Calculate JOAAT hash
 * - Vector3(x, y, z) - 3D vector class
 * - VK_* key constants
 * - setTimeout/setInterval/clearTimeout/clearInterval - Timers
 *
 * Note: 'natives' and 'core' are NOT globals - use ES6 imports:
 *   import natives from 'natives';
 *   import { addTickCallback } from 'core';
 */
void RegisterAllBindings(JSGlobalContextRef ctx, JSRuntime* runtime);

/**
 * Register the Native object for calling game natives
 */
void RegisterNativeObject(JSGlobalContextRef ctx, JSRuntime* runtime);

/**
 * Register the Global object for accessing game globals
 */
void RegisterGlobalObject(JSGlobalContextRef ctx, JSRuntime* runtime);

/**
 * Register the console object for logging
 */
void RegisterConsoleObject(JSGlobalContextRef ctx);

/**
 * Register the Hash utility object
 */
void RegisterHashObject(JSGlobalContextRef ctx);

/**
 * Register virtual key constants
 */
void RegisterKeyConstants(JSGlobalContextRef ctx);

/**
 * Register Vector3 class
 */
void RegisterVector3Class(JSGlobalContextRef ctx);

/**
 * Register _Core object for internal runtime functions
 */
void RegisterCoreObject(JSGlobalContextRef ctx);

/**
 * Register internal module objects (__natives__, __core__)
 * These are accessed via ES6 module imports, not directly
 */
void RegisterInternalModules(JSGlobalContextRef ctx, JSRuntime* runtime);

/**
 * Update key states - call once per tick before processing JS
 */
void UpdateKeyStates();

/**
 * Update game time - call once per tick
 */
void UpdateGameTime(DWORD time);

// ============================================================================
// Helper functions used by JSNativesGenerated.cpp
// ============================================================================

/**
 * Create a JSStringRef from a std::string
 */
JSStringRef CreateJSString(const std::string& str);

/**
 * Get a std::string from a JSValueRef
 */
std::string GetStringFromJSValue(JSContextRef ctx, JSValueRef value);

/**
 * Set a number property on an object
 */
void SetNumberProperty(JSContextRef ctx, JSObjectRef obj, const char* name, double value);

} // namespace rdr2js
