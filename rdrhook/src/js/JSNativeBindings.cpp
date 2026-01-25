#include "stdafx.h"
#include "JSNativeBindings.h"
#include "JSNativesGenerated.h"
#include "JSCoreModule.h"
#include "JSRuntime.h"
#include "Logger.h"

#include <JavaScriptCore/JavaScript.h>
#include <JavaScriptCore/APICast.h>
#include <JavaScriptCore/JSCJSValue.h>
#include <JavaScriptCore/JSGlobalObject.h>
#include <sstream>

namespace rdr2js {

// Native context for calling game functions (similar to trainer's Native::Context)
struct NativeContext {
    uint64_t* retVal = stack;
    uint64_t argCount = 0;
    uint64_t* stackPtr = stack;
    uint64_t dataCount = 0;
    uint64_t spaceForResults[24];
    uint64_t stack[24]{ 0 };

    void Reset() {
        argCount = 0;
        dataCount = 0;
        memset(stack, 0, sizeof(stack));
    }

    void Push(uint64_t value) {
        stack[argCount++] = value;
    }

    template<typename T>
    T Result() {
        return *reinterpret_cast<T*>(retVal);
    }

    void CopyResults() {
        uint64_t a1 = (uint64_t)this;
        uint64_t result;

        for (; *(uint32_t*)(a1 + 24); *(uint32_t*)(*(uint64_t*)(a1 + 8i64 * *(signed int*)(a1 + 24) + 32) + 16i64) = result) {
            --*(uint32_t*)(a1 + 24);
            **(uint32_t**)(a1 + 8i64 * *(signed int*)(a1 + 24) + 32) = *(uint32_t*)(a1 + 16 * (*(signed int*)(a1 + 24) + 4i64));
            *(uint32_t*)(*(uint64_t*)(a1 + 8i64 * *(signed int*)(a1 + 24) + 32) + 8i64) = *(uint32_t*)(a1 + 16i64 * *(signed int*)(a1 + 24) + 68);
            result = *(unsigned int*)(a1 + 16i64 * *(signed int*)(a1 + 24) + 72);
        }
        --*(uint32_t*)(a1 + 24);
    }
};

typedef void(__cdecl* NativeHandler)(NativeContext* context);

// SEH-safe helper functions (can't use __try with C++ objects that need unwinding)
static bool SafeCallNativeHandler(NativeHandler handler, NativeContext* ctx, DWORD* exceptionCode) {
    __try {
        handler(ctx);
        return true;
    }
    __except (EXCEPTION_EXECUTE_HANDLER) {
        if (exceptionCode) *exceptionCode = GetExceptionCode();
        return false;
    }
}

static bool SafeCopyResults(NativeContext* ctx, DWORD* exceptionCode) {
    __try {
        ctx->CopyResults();
        return true;
    }
    __except (EXCEPTION_EXECUTE_HANDLER) {
        if (exceptionCode) *exceptionCode = GetExceptionCode();
        return false;
    }
}

// Thread-local storage for runtime pointer (needed in callbacks)
// Accessible from JSNativesGenerated.cpp
thread_local JSRuntime* g_currentRuntime = nullptr;

// Helper to convert std::string to JSStringRef
JSStringRef CreateJSString(const std::string& str) {
    return JSStringCreateWithUTF8CString(str.c_str());
}

// Helper to get string from JSValue
std::string GetStringFromJSValue(JSContextRef ctx, JSValueRef value) {
    if (!value || JSValueIsUndefined(ctx, value) || JSValueIsNull(ctx, value)) {
        return "";
    }

    JSValueRef exception = nullptr;
    JSStringRef jsStr = JSValueToStringCopy(ctx, value, &exception);
    if (!jsStr) return "";

    size_t maxSize = JSStringGetMaximumUTF8CStringSize(jsStr);
    std::vector<char> buffer(maxSize);
    JSStringGetUTF8CString(jsStr, buffer.data(), maxSize);
    JSStringRelease(jsStr);

    return std::string(buffer.data());
}

// Helper to set a property on an object
void SetObjectProperty(JSContextRef ctx, JSObjectRef obj, const char* name, JSValueRef value) {
    JSStringRef propName = JSStringCreateWithUTF8CString(name);
    JSObjectSetProperty(ctx, obj, propName, value, kJSPropertyAttributeNone, nullptr);
    JSStringRelease(propName);
}

// Helper to set a number property
void SetNumberProperty(JSContextRef ctx, JSObjectRef obj, const char* name, double value) {
    SetObjectProperty(ctx, obj, name, JSValueMakeNumber(ctx, value));
}

// ============================================================================
// Native.invoke - Call a game native function
// ============================================================================

static JSValueRef JS_Native_invoke(JSContextRef ctx, JSObjectRef function,
    JSObjectRef thisObject, size_t argumentCount, const JSValueRef arguments[], JSValueRef* exception) {

    spdlog::debug("[JS Native] invoke called with {} arguments", argumentCount);

    if (argumentCount < 1) {
        spdlog::error("[JS Native] invoke: missing hash argument");
        *exception = JSValueMakeString(ctx, CreateJSString("Native.invoke requires at least a hash argument"));
        return JSValueMakeUndefined(ctx);
    }

    if (!g_currentRuntime) {
        spdlog::error("[JS Native] invoke: g_currentRuntime is null");
        *exception = JSValueMakeString(ctx, CreateJSString("Runtime not available"));
        return JSValueMakeUndefined(ctx);
    }

    // Get the native hash (first argument)
    // Handle both Number and BigInt types (BigInt is needed for 64-bit hashes > 2^53)
    uint64_t hash = 0;
    JSC::JSGlobalObject* globalObject = toJS(ctx);
    JSC::JSValue hashValue = toJS(globalObject, arguments[0]);

    if (hashValue.isBigInt()) {
        // BigInt - use toBigUInt64 for full 64-bit precision
        hash = hashValue.toBigUInt64(globalObject);
        spdlog::debug("[JS Native] invoke: hash (BigInt) = 0x{:X}", hash);
    } else {
        // Regular number - limited to 53-bit precision
        hash = static_cast<uint64_t>(JSValueToNumber(ctx, arguments[0], nullptr));
        spdlog::debug("[JS Native] invoke: hash (Number) = 0x{:X}", hash);
    }

    // Get native function address
    auto getNativeAddr = g_currentRuntime->GetNativeAddrFn();
    spdlog::debug("[JS Native] invoke: getNativeAddr function ptr = {:p}", (void*)getNativeAddr);

    if (!getNativeAddr) {
        spdlog::error("[JS Native] invoke: getNativeAddr function is null");
        *exception = JSValueMakeString(ctx, CreateJSString("getNativeAddr function not set"));
        return JSValueMakeUndefined(ctx);
    }

    spdlog::debug("[JS Native] invoke: calling getNativeAddr(0x{:X})...", hash);
    uintptr_t nativeAddr = getNativeAddr(hash);
    spdlog::debug("[JS Native] invoke: nativeAddr = {:p}", (void*)nativeAddr);

    if (!nativeAddr) {
        spdlog::error("[JS Native] invoke: native function not found for hash 0x{:X}", hash);
        *exception = JSValueMakeString(ctx, CreateJSString("Native function not found for hash"));
        return JSValueMakeUndefined(ctx);
    }

    NativeHandler handler = reinterpret_cast<NativeHandler>(nativeAddr);
    spdlog::debug("[JS Native] invoke: handler = {:p}", (void*)handler);

    // Build the native context with arguments
    static thread_local NativeContext nativeCtx;
    nativeCtx.Reset();
    spdlog::debug("[JS Native] invoke: context reset, processing {} extra args", argumentCount - 1);

    // Push all arguments (skip first which is the hash)
    for (size_t i = 1; i < argumentCount; i++) {
        JSValueRef arg = arguments[i];

        if (JSValueIsNumber(ctx, arg)) {
            double num = JSValueToNumber(ctx, arg, nullptr);
            // Check if it's an integer or float
            if (num == static_cast<double>(static_cast<int64_t>(num))) {
                uint64_t val = static_cast<uint64_t>(static_cast<int64_t>(num));
                spdlog::debug("[JS Native] invoke: arg[{}] = int {}", i, val);
                nativeCtx.Push(val);
            } else {
                // It's a float - need to push as float bits
                float f = static_cast<float>(num);
                uint32_t bits = *reinterpret_cast<uint32_t*>(&f);
                spdlog::debug("[JS Native] invoke: arg[{}] = float {} (bits: 0x{:X})", i, f, bits);
                nativeCtx.Push(static_cast<uint64_t>(bits));
            }
        } else if (JSValueIsBoolean(ctx, arg)) {
            uint64_t val = JSValueToBoolean(ctx, arg) ? 1ULL : 0ULL;
            spdlog::debug("[JS Native] invoke: arg[{}] = bool {}", i, val);
            nativeCtx.Push(val);
        } else if (JSValueIsString(ctx, arg)) {
            // For strings, we need to get the C string and pass the pointer
            std::string str = GetStringFromJSValue(ctx, arg);
            spdlog::debug("[JS Native] invoke: arg[{}] = string \"{}\"", i, str);
            // Note: This is potentially unsafe as the string might go out of scope
            // For production, we'd need to manage string lifetime properly
            nativeCtx.Push(reinterpret_cast<uint64_t>(str.c_str()));
        } else if (JSValueIsNull(ctx, arg) || JSValueIsUndefined(ctx, arg)) {
            spdlog::debug("[JS Native] invoke: arg[{}] = null/undefined", i);
            nativeCtx.Push(0ULL);
        } else if (JSValueIsObject(ctx, arg)) {
            // Check if it's a Vector3
            JSObjectRef obj = JSValueToObject(ctx, arg, nullptr);
            JSStringRef xProp = JSStringCreateWithUTF8CString("x");
            JSStringRef yProp = JSStringCreateWithUTF8CString("y");
            JSStringRef zProp = JSStringCreateWithUTF8CString("z");

            if (JSObjectHasProperty(ctx, obj, xProp)) {
                // It's likely a Vector3
                float x = static_cast<float>(JSValueToNumber(ctx, JSObjectGetProperty(ctx, obj, xProp, nullptr), nullptr));
                float y = static_cast<float>(JSValueToNumber(ctx, JSObjectGetProperty(ctx, obj, yProp, nullptr), nullptr));
                float z = static_cast<float>(JSValueToNumber(ctx, JSObjectGetProperty(ctx, obj, zProp, nullptr), nullptr));

                spdlog::debug("[JS Native] invoke: arg[{}] = Vector3({}, {}, {})", i, x, y, z);

                // Push Vector3 as three separate floats
                nativeCtx.Push(*reinterpret_cast<uint32_t*>(&x));
                nativeCtx.Push(*reinterpret_cast<uint32_t*>(&y));
                nativeCtx.Push(*reinterpret_cast<uint32_t*>(&z));
            } else {
                spdlog::debug("[JS Native] invoke: arg[{}] = object (not Vector3)", i);
                nativeCtx.Push(0ULL);
            }

            JSStringRelease(xProp);
            JSStringRelease(yProp);
            JSStringRelease(zProp);
        } else {
            spdlog::debug("[JS Native] invoke: arg[{}] = unknown type", i);
            nativeCtx.Push(0ULL);
        }
    }

    spdlog::debug("[JS Native] invoke: calling native handler at {:p} with {} args...", (void*)handler, nativeCtx.argCount);

    // Call the native function with SEH protection
    DWORD exceptionCode = 0;
    if (!SafeCallNativeHandler(handler, &nativeCtx, &exceptionCode)) {
        spdlog::error("[JS Native] invoke: CRASH in native handler! Exception code: 0x{:X}", exceptionCode);
        *exception = JSValueMakeString(ctx, CreateJSString("Native function crashed"));
        return JSValueMakeUndefined(ctx);
    }

    spdlog::debug("[JS Native] invoke: native handler returned, calling CopyResults...");

    if (!SafeCopyResults(&nativeCtx, &exceptionCode)) {
        spdlog::error("[JS Native] invoke: CRASH in CopyResults! Exception code: 0x{:X}", exceptionCode);
    }

    // Return the result as a number (most common case)
    // For more complex return types, we'd need type hints
    uint64_t result = nativeCtx.Result<uint64_t>();
    spdlog::debug("[JS Native] invoke: result = 0x{:X} ({})", result, result);

    return JSValueMakeNumber(ctx, static_cast<double>(result));
}

// ============================================================================
// Native.invokeFloat - Call native and return float result
// ============================================================================

static JSValueRef JS_Native_invokeFloat(JSContextRef ctx, JSObjectRef function,
    JSObjectRef thisObject, size_t argumentCount, const JSValueRef arguments[], JSValueRef* exception) {

    JSValueRef result = JS_Native_invoke(ctx, function, thisObject, argumentCount, arguments, exception);
    if (*exception) return result;

    // Reinterpret the result as float
    double numResult = JSValueToNumber(ctx, result, nullptr);
    uint32_t bits = static_cast<uint32_t>(numResult);
    float floatResult = *reinterpret_cast<float*>(&bits);

    return JSValueMakeNumber(ctx, floatResult);
}

// ============================================================================
// Native.invokeVector3 - Call native and return Vector3 result
// ============================================================================

static JSValueRef JS_Native_invokeVector3(JSContextRef ctx, JSObjectRef function,
    JSObjectRef thisObject, size_t argumentCount, const JSValueRef arguments[], JSValueRef* exception) {

    if (argumentCount < 1) {
        *exception = JSValueMakeString(ctx, CreateJSString("Native.invokeVector3 requires at least a hash argument"));
        return JSValueMakeUndefined(ctx);
    }

    if (!g_currentRuntime) {
        *exception = JSValueMakeString(ctx, CreateJSString("Runtime not available"));
        return JSValueMakeUndefined(ctx);
    }

    // Get the native hash - handle both Number and BigInt types
    uint64_t hash = 0;
    JSC::JSGlobalObject* globalObject = toJS(ctx);
    JSC::JSValue hashValue = toJS(globalObject, arguments[0]);

    if (hashValue.isBigInt()) {
        hash = hashValue.toBigUInt64(globalObject);
    } else {
        hash = static_cast<uint64_t>(JSValueToNumber(ctx, arguments[0], nullptr));
    }

    auto getNativeAddr = g_currentRuntime->GetNativeAddrFn();
    uintptr_t nativeAddr = getNativeAddr(hash);

    if (!nativeAddr) {
        *exception = JSValueMakeString(ctx, CreateJSString("Native function not found"));
        return JSValueMakeUndefined(ctx);
    }

    NativeHandler handler = reinterpret_cast<NativeHandler>(nativeAddr);

    static thread_local NativeContext nativeCtx;
    nativeCtx.Reset();

    for (size_t i = 1; i < argumentCount; i++) {
        JSValueRef arg = arguments[i];
        if (JSValueIsNumber(ctx, arg)) {
            double num = JSValueToNumber(ctx, arg, nullptr);
            if (num == static_cast<double>(static_cast<int64_t>(num))) {
                nativeCtx.Push(static_cast<uint64_t>(static_cast<int64_t>(num)));
            } else {
                float f = static_cast<float>(num);
                nativeCtx.Push(*reinterpret_cast<uint32_t*>(&f));
            }
        } else {
            nativeCtx.Push(0ULL);
        }
    }

    handler(&nativeCtx);
    nativeCtx.CopyResults();

    // Extract Vector3 from result
    float x = *reinterpret_cast<float*>(nativeCtx.retVal + 0);
    float y = *reinterpret_cast<float*>((uintptr_t)nativeCtx.retVal + 8);
    float z = *reinterpret_cast<float*>((uintptr_t)nativeCtx.retVal + 16);

    // Create Vector3 object
    JSObjectRef vec3 = JSObjectMake(ctx, nullptr, nullptr);
    SetNumberProperty(ctx, vec3, "x", x);
    SetNumberProperty(ctx, vec3, "y", y);
    SetNumberProperty(ctx, vec3, "z", z);

    return vec3;
}

// ============================================================================
// Global object - Access game global variables
// ============================================================================

static JSValueRef JS_Global_getInt(JSContextRef ctx, JSObjectRef function,
    JSObjectRef thisObject, size_t argumentCount, const JSValueRef arguments[], JSValueRef* exception) {

    if (argumentCount < 1) {
        *exception = JSValueMakeString(ctx, CreateJSString("Global.getInt requires a global ID"));
        return JSValueMakeUndefined(ctx);
    }

    if (!g_currentRuntime) {
        *exception = JSValueMakeString(ctx, CreateJSString("Runtime not available"));
        return JSValueMakeUndefined(ctx);
    }

    uint32_t globalId = static_cast<uint32_t>(JSValueToNumber(ctx, arguments[0], nullptr));
    auto getGlobalPtr = g_currentRuntime->GetGlobalPtrFn();
    void* ptr = getGlobalPtr(globalId);

    if (!ptr) {
        return JSValueMakeNumber(ctx, 0);
    }

    int32_t value = *static_cast<int32_t*>(ptr);
    return JSValueMakeNumber(ctx, value);
}

static JSValueRef JS_Global_setInt(JSContextRef ctx, JSObjectRef function,
    JSObjectRef thisObject, size_t argumentCount, const JSValueRef arguments[], JSValueRef* exception) {

    if (argumentCount < 2) {
        *exception = JSValueMakeString(ctx, CreateJSString("Global.setInt requires global ID and value"));
        return JSValueMakeUndefined(ctx);
    }

    if (!g_currentRuntime) {
        *exception = JSValueMakeString(ctx, CreateJSString("Runtime not available"));
        return JSValueMakeUndefined(ctx);
    }

    uint32_t globalId = static_cast<uint32_t>(JSValueToNumber(ctx, arguments[0], nullptr));
    int32_t value = static_cast<int32_t>(JSValueToNumber(ctx, arguments[1], nullptr));

    auto getGlobalPtr = g_currentRuntime->GetGlobalPtrFn();
    void* ptr = getGlobalPtr(globalId);

    if (ptr) {
        *static_cast<int32_t*>(ptr) = value;
    }

    return JSValueMakeUndefined(ctx);
}

static JSValueRef JS_Global_getFloat(JSContextRef ctx, JSObjectRef function,
    JSObjectRef thisObject, size_t argumentCount, const JSValueRef arguments[], JSValueRef* exception) {

    if (argumentCount < 1) {
        *exception = JSValueMakeString(ctx, CreateJSString("Global.getFloat requires a global ID"));
        return JSValueMakeUndefined(ctx);
    }

    if (!g_currentRuntime) {
        *exception = JSValueMakeString(ctx, CreateJSString("Runtime not available"));
        return JSValueMakeUndefined(ctx);
    }

    uint32_t globalId = static_cast<uint32_t>(JSValueToNumber(ctx, arguments[0], nullptr));
    auto getGlobalPtr = g_currentRuntime->GetGlobalPtrFn();
    void* ptr = getGlobalPtr(globalId);

    if (!ptr) {
        return JSValueMakeNumber(ctx, 0.0);
    }

    float value = *static_cast<float*>(ptr);
    return JSValueMakeNumber(ctx, value);
}

static JSValueRef JS_Global_setFloat(JSContextRef ctx, JSObjectRef function,
    JSObjectRef thisObject, size_t argumentCount, const JSValueRef arguments[], JSValueRef* exception) {

    if (argumentCount < 2) {
        *exception = JSValueMakeString(ctx, CreateJSString("Global.setFloat requires global ID and value"));
        return JSValueMakeUndefined(ctx);
    }

    if (!g_currentRuntime) {
        *exception = JSValueMakeString(ctx, CreateJSString("Runtime not available"));
        return JSValueMakeUndefined(ctx);
    }

    uint32_t globalId = static_cast<uint32_t>(JSValueToNumber(ctx, arguments[0], nullptr));
    float value = static_cast<float>(JSValueToNumber(ctx, arguments[1], nullptr));

    auto getGlobalPtr = g_currentRuntime->GetGlobalPtrFn();
    void* ptr = getGlobalPtr(globalId);

    if (ptr) {
        *static_cast<float*>(ptr) = value;
    }

    return JSValueMakeUndefined(ctx);
}

// ============================================================================
// console object - Logging
// ============================================================================

static JSValueRef JS_console_log(JSContextRef ctx, JSObjectRef function,
    JSObjectRef thisObject, size_t argumentCount, const JSValueRef arguments[], JSValueRef* exception) {

    std::ostringstream ss;
    for (size_t i = 0; i < argumentCount; i++) {
        if (i > 0) ss << " ";
        ss << GetStringFromJSValue(ctx, arguments[i]);
    }

    spdlog::info("[JS] {}", ss.str());
    return JSValueMakeUndefined(ctx);
}

static JSValueRef JS_console_warn(JSContextRef ctx, JSObjectRef function,
    JSObjectRef thisObject, size_t argumentCount, const JSValueRef arguments[], JSValueRef* exception) {

    std::ostringstream ss;
    for (size_t i = 0; i < argumentCount; i++) {
        if (i > 0) ss << " ";
        ss << GetStringFromJSValue(ctx, arguments[i]);
    }

    spdlog::warn("[JS] {}", ss.str());
    return JSValueMakeUndefined(ctx);
}

static JSValueRef JS_console_error(JSContextRef ctx, JSObjectRef function,
    JSObjectRef thisObject, size_t argumentCount, const JSValueRef arguments[], JSValueRef* exception) {

    std::ostringstream ss;
    for (size_t i = 0; i < argumentCount; i++) {
        if (i > 0) ss << " ";
        ss << GetStringFromJSValue(ctx, arguments[i]);
    }

    spdlog::error("[JS] {}", ss.str());
    return JSValueMakeUndefined(ctx);
}

// ============================================================================
// Hash object - String hashing
// ============================================================================

static uint32_t JoaatHash(const std::string& str) {
    uint32_t hash = 0;
    for (char c : str) {
        hash += static_cast<unsigned char>(tolower(c));
        hash += (hash << 10);
        hash ^= (hash >> 6);
    }
    hash += (hash << 3);
    hash ^= (hash >> 11);
    hash += (hash << 15);
    return hash;
}

static JSValueRef JS_Hash_joaat(JSContextRef ctx, JSObjectRef function,
    JSObjectRef thisObject, size_t argumentCount, const JSValueRef arguments[], JSValueRef* exception) {

    if (argumentCount < 1) {
        *exception = JSValueMakeString(ctx, CreateJSString("Hash.joaat requires a string argument"));
        return JSValueMakeUndefined(ctx);
    }

    std::string str = GetStringFromJSValue(ctx, arguments[0]);
    uint32_t hash = JoaatHash(str);

    return JSValueMakeNumber(ctx, hash);
}

// ============================================================================
// Registration functions
// ============================================================================

void RegisterNativeObject(JSGlobalContextRef ctx, JSRuntime* runtime) {
    JSObjectRef globalObj = JSContextGetGlobalObject(ctx);
    JSObjectRef nativeObj = JSObjectMake(ctx, nullptr, nullptr);

    // Native.invoke
    JSStringRef invokeName = CreateJSString("invoke");
    JSObjectRef invokeFunc = JSObjectMakeFunctionWithCallback(ctx, invokeName, JS_Native_invoke);
    JSObjectSetProperty(ctx, nativeObj, invokeName, invokeFunc, kJSPropertyAttributeNone, nullptr);
    JSStringRelease(invokeName);

    // Native.invokeFloat
    JSStringRef invokeFloatName = CreateJSString("invokeFloat");
    JSObjectRef invokeFloatFunc = JSObjectMakeFunctionWithCallback(ctx, invokeFloatName, JS_Native_invokeFloat);
    JSObjectSetProperty(ctx, nativeObj, invokeFloatName, invokeFloatFunc, kJSPropertyAttributeNone, nullptr);
    JSStringRelease(invokeFloatName);

    // Native.invokeVector3
    JSStringRef invokeVector3Name = CreateJSString("invokeVector3");
    JSObjectRef invokeVector3Func = JSObjectMakeFunctionWithCallback(ctx, invokeVector3Name, JS_Native_invokeVector3);
    JSObjectSetProperty(ctx, nativeObj, invokeVector3Name, invokeVector3Func, kJSPropertyAttributeNone, nullptr);
    JSStringRelease(invokeVector3Name);

    // Set Native object on global
    JSStringRef nativeName = CreateJSString("Native");
    JSObjectSetProperty(ctx, globalObj, nativeName, nativeObj, kJSPropertyAttributeNone, nullptr);
    JSStringRelease(nativeName);
}

void RegisterGlobalObject(JSGlobalContextRef ctx, JSRuntime* runtime) {
    JSObjectRef globalObj = JSContextGetGlobalObject(ctx);
    JSObjectRef globalsObj = JSObjectMake(ctx, nullptr, nullptr);

    // Global.getInt
    JSStringRef getIntName = CreateJSString("getInt");
    JSObjectRef getIntFunc = JSObjectMakeFunctionWithCallback(ctx, getIntName, JS_Global_getInt);
    JSObjectSetProperty(ctx, globalsObj, getIntName, getIntFunc, kJSPropertyAttributeNone, nullptr);
    JSStringRelease(getIntName);

    // Global.setInt
    JSStringRef setIntName = CreateJSString("setInt");
    JSObjectRef setIntFunc = JSObjectMakeFunctionWithCallback(ctx, setIntName, JS_Global_setInt);
    JSObjectSetProperty(ctx, globalsObj, setIntName, setIntFunc, kJSPropertyAttributeNone, nullptr);
    JSStringRelease(setIntName);

    // Global.getFloat
    JSStringRef getFloatName = CreateJSString("getFloat");
    JSObjectRef getFloatFunc = JSObjectMakeFunctionWithCallback(ctx, getFloatName, JS_Global_getFloat);
    JSObjectSetProperty(ctx, globalsObj, getFloatName, getFloatFunc, kJSPropertyAttributeNone, nullptr);
    JSStringRelease(getFloatName);

    // Global.setFloat
    JSStringRef setFloatName = CreateJSString("setFloat");
    JSObjectRef setFloatFunc = JSObjectMakeFunctionWithCallback(ctx, setFloatName, JS_Global_setFloat);
    JSObjectSetProperty(ctx, globalsObj, setFloatName, setFloatFunc, kJSPropertyAttributeNone, nullptr);
    JSStringRelease(setFloatName);

    // Set Global object
    JSStringRef globalsName = CreateJSString("Global");
    JSObjectSetProperty(ctx, globalObj, globalsName, globalsObj, kJSPropertyAttributeNone, nullptr);
    JSStringRelease(globalsName);
}

void RegisterConsoleObject(JSGlobalContextRef ctx) {
    JSObjectRef globalObj = JSContextGetGlobalObject(ctx);
    JSObjectRef consoleObj = JSObjectMake(ctx, nullptr, nullptr);

    // console.log
    JSStringRef logName = CreateJSString("log");
    JSObjectRef logFunc = JSObjectMakeFunctionWithCallback(ctx, logName, JS_console_log);
    JSObjectSetProperty(ctx, consoleObj, logName, logFunc, kJSPropertyAttributeNone, nullptr);
    JSStringRelease(logName);

    // console.warn
    JSStringRef warnName = CreateJSString("warn");
    JSObjectRef warnFunc = JSObjectMakeFunctionWithCallback(ctx, warnName, JS_console_warn);
    JSObjectSetProperty(ctx, consoleObj, warnName, warnFunc, kJSPropertyAttributeNone, nullptr);
    JSStringRelease(warnName);

    // console.error
    JSStringRef errorName = CreateJSString("error");
    JSObjectRef errorFunc = JSObjectMakeFunctionWithCallback(ctx, errorName, JS_console_error);
    JSObjectSetProperty(ctx, consoleObj, errorName, errorFunc, kJSPropertyAttributeNone, nullptr);
    JSStringRelease(errorName);

    // Set console object
    JSStringRef consoleName = CreateJSString("console");
    JSObjectSetProperty(ctx, globalObj, consoleName, consoleObj, kJSPropertyAttributeNone, nullptr);
    JSStringRelease(consoleName);
}

void RegisterHashObject(JSGlobalContextRef ctx) {
    JSObjectRef globalObj = JSContextGetGlobalObject(ctx);
    JSObjectRef hashObj = JSObjectMake(ctx, nullptr, nullptr);

    // Hash.joaat
    JSStringRef joaatName = CreateJSString("joaat");
    JSObjectRef joaatFunc = JSObjectMakeFunctionWithCallback(ctx, joaatName, JS_Hash_joaat);
    JSObjectSetProperty(ctx, hashObj, joaatName, joaatFunc, kJSPropertyAttributeNone, nullptr);
    JSStringRelease(joaatName);

    // Set Hash object
    JSStringRef hashName = CreateJSString("Hash");
    JSObjectSetProperty(ctx, globalObj, hashName, hashObj, kJSPropertyAttributeNone, nullptr);
    JSStringRelease(hashName);
}

void RegisterKeyConstants(JSGlobalContextRef ctx) {
    JSObjectRef globalObj = JSContextGetGlobalObject(ctx);

    // Common virtual key codes
    SetNumberProperty(ctx, globalObj, "VK_BACK", 0x08);
    SetNumberProperty(ctx, globalObj, "VK_TAB", 0x09);
    SetNumberProperty(ctx, globalObj, "VK_RETURN", 0x0D);
    SetNumberProperty(ctx, globalObj, "VK_SHIFT", 0x10);
    SetNumberProperty(ctx, globalObj, "VK_CONTROL", 0x11);
    SetNumberProperty(ctx, globalObj, "VK_MENU", 0x12);  // Alt
    SetNumberProperty(ctx, globalObj, "VK_PAUSE", 0x13);
    SetNumberProperty(ctx, globalObj, "VK_CAPITAL", 0x14);
    SetNumberProperty(ctx, globalObj, "VK_ESCAPE", 0x1B);
    SetNumberProperty(ctx, globalObj, "VK_SPACE", 0x20);
    SetNumberProperty(ctx, globalObj, "VK_PRIOR", 0x21);  // Page Up
    SetNumberProperty(ctx, globalObj, "VK_NEXT", 0x22);   // Page Down
    SetNumberProperty(ctx, globalObj, "VK_END", 0x23);
    SetNumberProperty(ctx, globalObj, "VK_HOME", 0x24);
    SetNumberProperty(ctx, globalObj, "VK_LEFT", 0x25);
    SetNumberProperty(ctx, globalObj, "VK_UP", 0x26);
    SetNumberProperty(ctx, globalObj, "VK_RIGHT", 0x27);
    SetNumberProperty(ctx, globalObj, "VK_DOWN", 0x28);
    SetNumberProperty(ctx, globalObj, "VK_INSERT", 0x2D);
    SetNumberProperty(ctx, globalObj, "VK_DELETE", 0x2E);

    // Number keys
    for (int i = 0; i <= 9; i++) {
        char name[8];
        snprintf(name, sizeof(name), "VK_%d", i);
        SetNumberProperty(ctx, globalObj, name, 0x30 + i);
    }

    // Letter keys
    for (int i = 0; i < 26; i++) {
        char name[8];
        snprintf(name, sizeof(name), "VK_%c", 'A' + i);
        SetNumberProperty(ctx, globalObj, name, 0x41 + i);
    }

    // Function keys
    for (int i = 1; i <= 12; i++) {
        char name[8];
        snprintf(name, sizeof(name), "VK_F%d", i);
        SetNumberProperty(ctx, globalObj, name, 0x6F + i);
    }

    // Numpad keys
    for (int i = 0; i <= 9; i++) {
        char name[16];
        snprintf(name, sizeof(name), "VK_NUMPAD%d", i);
        SetNumberProperty(ctx, globalObj, name, 0x60 + i);
    }

    SetNumberProperty(ctx, globalObj, "VK_MULTIPLY", 0x6A);
    SetNumberProperty(ctx, globalObj, "VK_ADD", 0x6B);
    SetNumberProperty(ctx, globalObj, "VK_SUBTRACT", 0x6D);
    SetNumberProperty(ctx, globalObj, "VK_DECIMAL", 0x6E);
    SetNumberProperty(ctx, globalObj, "VK_DIVIDE", 0x6F);
}

void RegisterVector3Class(JSGlobalContextRef ctx) {
    // Create a simple Vector3 constructor function via JavaScript
    const char* vector3Code =
        "function Vector3(x, y, z) { this.x = x || 0; this.y = y || 0; this.z = z || 0; }\n"
        "Vector3.prototype.add = function(o) { return new Vector3(this.x + o.x, this.y + o.y, this.z + o.z); };\n"
        "Vector3.prototype.sub = function(o) { return new Vector3(this.x - o.x, this.y - o.y, this.z - o.z); };\n"
        "Vector3.prototype.mul = function(s) { return new Vector3(this.x * s, this.y * s, this.z * s); };\n"
        "Vector3.prototype.length = function() { return Math.sqrt(this.x*this.x + this.y*this.y + this.z*this.z); };\n"
        "Vector3.prototype.normalize = function() { var l = this.length(); return l === 0 ? new Vector3() : new Vector3(this.x/l, this.y/l, this.z/l); };\n"
        "Vector3.prototype.toString = function() { return 'Vector3(' + this.x + ', ' + this.y + ', ' + this.z + ')'; };\n";

    JSStringRef script = CreateJSString(vector3Code);
    JSStringRef source = CreateJSString("Vector3.js");
    JSEvaluateScript(ctx, script, nullptr, source, 1, nullptr);
    JSStringRelease(script);
    JSStringRelease(source);
}

// ============================================================================
// _Core object - Internal runtime functions for core.js module
// ============================================================================

// Key state tracking
static uint8_t g_keyStates[256] = { 0 };
static uint8_t g_prevKeyStates[256] = { 0 };
static DWORD g_gameTime = 0;

void UpdateKeyStates() {
    memcpy(g_prevKeyStates, g_keyStates, sizeof(g_keyStates));
    for (int i = 0; i < 256; i++) {
        g_keyStates[i] = (GetAsyncKeyState(i) & 0x8000) ? 1 : 0;
    }
}

void UpdateGameTime(DWORD time) {
    g_gameTime = time;
}

static JSValueRef JS_Core_getGameTime(JSContextRef ctx, JSObjectRef function,
    JSObjectRef thisObject, size_t argumentCount, const JSValueRef arguments[], JSValueRef* exception) {
    return JSValueMakeNumber(ctx, g_gameTime);
}

static JSValueRef JS_Core_isKeyPressed(JSContextRef ctx, JSObjectRef function,
    JSObjectRef thisObject, size_t argumentCount, const JSValueRef arguments[], JSValueRef* exception) {
    if (argumentCount < 1) {
        return JSValueMakeBoolean(ctx, false);
    }
    int keyCode = static_cast<int>(JSValueToNumber(ctx, arguments[0], nullptr));
    if (keyCode < 0 || keyCode >= 256) {
        return JSValueMakeBoolean(ctx, false);
    }
    return JSValueMakeBoolean(ctx, g_keyStates[keyCode] != 0);
}

static JSValueRef JS_Core_isKeyJustPressed(JSContextRef ctx, JSObjectRef function,
    JSObjectRef thisObject, size_t argumentCount, const JSValueRef arguments[], JSValueRef* exception) {
    if (argumentCount < 1) {
        return JSValueMakeBoolean(ctx, false);
    }
    int keyCode = static_cast<int>(JSValueToNumber(ctx, arguments[0], nullptr));
    if (keyCode < 0 || keyCode >= 256) {
        return JSValueMakeBoolean(ctx, false);
    }
    // Key is just pressed if it's down now but wasn't before
    return JSValueMakeBoolean(ctx, g_keyStates[keyCode] != 0 && g_prevKeyStates[keyCode] == 0);
}

void RegisterCoreObject(JSGlobalContextRef ctx) {
    JSObjectRef globalObj = JSContextGetGlobalObject(ctx);
    JSObjectRef coreObj = JSObjectMake(ctx, nullptr, nullptr);

    // _Core.getGameTime
    JSStringRef getGameTimeName = CreateJSString("getGameTime");
    JSObjectRef getGameTimeFunc = JSObjectMakeFunctionWithCallback(ctx, getGameTimeName, JS_Core_getGameTime);
    JSObjectSetProperty(ctx, coreObj, getGameTimeName, getGameTimeFunc, kJSPropertyAttributeNone, nullptr);
    JSStringRelease(getGameTimeName);

    // _Core.isKeyPressed
    JSStringRef isKeyPressedName = CreateJSString("isKeyPressed");
    JSObjectRef isKeyPressedFunc = JSObjectMakeFunctionWithCallback(ctx, isKeyPressedName, JS_Core_isKeyPressed);
    JSObjectSetProperty(ctx, coreObj, isKeyPressedName, isKeyPressedFunc, kJSPropertyAttributeNone, nullptr);
    JSStringRelease(isKeyPressedName);

    // _Core.isKeyJustPressed
    JSStringRef isKeyJustPressedName = CreateJSString("isKeyJustPressed");
    JSObjectRef isKeyJustPressedFunc = JSObjectMakeFunctionWithCallback(ctx, isKeyJustPressedName, JS_Core_isKeyJustPressed);
    JSObjectSetProperty(ctx, coreObj, isKeyJustPressedName, isKeyJustPressedFunc, kJSPropertyAttributeNone, nullptr);
    JSStringRelease(isKeyJustPressedName);

    // Set _Core object
    JSStringRef coreName = CreateJSString("_Core");
    JSObjectSetProperty(ctx, globalObj, coreName, coreObj, kJSPropertyAttributeNone, nullptr);
    JSStringRelease(coreName);
}

void RegisterAllBindings(JSGlobalContextRef ctx, JSRuntime* runtime) {
    // Set the current runtime for callbacks
    g_currentRuntime = runtime;

    // Register standard global objects
    RegisterConsoleObject(ctx);
    RegisterHashObject(ctx);
    RegisterKeyConstants(ctx);
    RegisterVector3Class(ctx);
    RegisterNativeObject(ctx, runtime);  // Native.invoke() for raw calls
    RegisterGlobalObject(ctx, runtime);  // Global.getInt/setInt for script globals
    RegisterGlobalTimers(ctx);           // setTimeout/setInterval on global

    // Register internal module objects (accessed via ES6 imports, not directly)
    // These use __ prefix to indicate they're internal implementation details
    RegisterInternalModules(ctx, runtime);
}

void RegisterInternalModules(JSGlobalContextRef ctx, JSRuntime* runtime) {
    JSObjectRef globalObj = JSContextGetGlobalObject(ctx);

    // Create and register __natives__ (internal, accessed via 'import natives from "natives"')
    JSObjectRef nativesObj = JSObjectMake(ctx, nullptr, nullptr);
    RegisterGeneratedNatives(ctx, nativesObj);
    JSStringRef nativesName = CreateJSString("__natives__");
    JSObjectSetProperty(ctx, globalObj, nativesName, nativesObj,
        kJSPropertyAttributeReadOnly | kJSPropertyAttributeDontEnum | kJSPropertyAttributeDontDelete, nullptr);
    JSStringRelease(nativesName);
    spdlog::info("[JS] Registered {} native functions (accessible via 'import natives from \"natives\"')",
        GetRegisteredNativeCount());

    // Create and register __core__ (internal, accessed via 'import { addTickCallback } from "core"')
    JSObjectRef coreObj = CreateCoreModuleObject(ctx);
    JSStringRef coreName = CreateJSString("__core__");
    JSObjectSetProperty(ctx, globalObj, coreName, coreObj,
        kJSPropertyAttributeReadOnly | kJSPropertyAttributeDontEnum | kJSPropertyAttributeDontDelete, nullptr);
    JSStringRelease(coreName);
    spdlog::info("[JS] Registered core module (accessible via 'import {{ ... }} from \"core\"')");
}

} // namespace rdr2js
