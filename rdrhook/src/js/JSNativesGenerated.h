#pragma once
// Auto-generated natives registration header
// Do not edit manually - regenerate with tools/codegen/generate_natives.py

#include <JavaScriptCore/JavaScript.h>
#include <cstdint>

namespace rdr2js {

// Return type enumeration for native functions
enum class NativeReturnType : uint8_t {
    Int = 0,      // Returns integer (most common)
    Float = 1,    // Returns float
    Vector3 = 2,  // Returns Vector3
    Void = 3,     // No return value
    Bool = 4,     // Returns boolean
    String = 5,   // Returns string (const char*)
};

// Information about a native function stored in JSObject private data
struct NativeInfo {
    uint64_t hash;
    NativeReturnType returnType;
    uint8_t paramCount;  // For validation/debugging
};

// Register all native functions on the given object
// This creates ~7000 function properties on nativesObj
void RegisterGeneratedNatives(JSContextRef ctx, JSObjectRef nativesObj);

// Get the JSClass used for native function objects (for cleanup)
JSClassRef GetNativeFunctionClass();

// Total number of registered natives (for debugging)
size_t GetRegisteredNativeCount();

} // namespace rdr2js
