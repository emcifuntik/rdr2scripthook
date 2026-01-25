#include "stdafx.h"
#include "JSCoreModule.h"
#include "JSNativeBindings.h"
#include "Logger.h"

#include <JavaScriptCore/APICast.h>
#include <JavaScriptCore/JSGlobalObject.h>
#include <JavaScriptCore/VM.h>
#include <JavaScriptCore/JSLock.h>

#include <unordered_map>
#include <vector>
#include <mutex>
#include <algorithm>

namespace rdr2js {

// ============================================================================
// Callback storage
// ============================================================================

struct CallbackEntry {
    CallbackId id;
    JSObjectRef callback;
    bool persistent;  // For GC protection
};

struct TimerEntry {
    CallbackId id;
    JSObjectRef callback;
    uint32_t triggerTime;
    uint32_t interval;  // 0 for setTimeout, >0 for setInterval
    bool cancelled;
};

// Per-context callback storage
struct ContextCallbacks {
    std::vector<CallbackEntry> tickCallbacks;
    std::vector<CallbackEntry> keyDownCallbacks;
    std::vector<CallbackEntry> keyUpCallbacks;
    std::vector<TimerEntry> timers;
    CallbackId nextId = 1;
};

static std::unordered_map<JSGlobalContextRef, ContextCallbacks> g_contextCallbacks;
static std::mutex g_callbacksMutex;

// ============================================================================
// Helper functions
// ============================================================================

static CallbackId GetNextId(ContextCallbacks& ctx) {
    return ctx.nextId++;
}

static void ProtectCallback(JSContextRef ctx, JSObjectRef callback) {
    JSValueProtect(ctx, callback);
}

static void UnprotectCallback(JSContextRef ctx, JSObjectRef callback) {
    JSValueUnprotect(ctx, callback);
}

// Drain microtasks to process Promise continuations
static void DrainMicrotasks(JSGlobalContextRef ctx) {
    JSC::JSGlobalObject* globalObject = toJS(ctx);
    JSC::VM& vm = globalObject->vm();
    JSC::JSLockHolder locker(vm);
    vm.drainMicrotasks();
}

// ============================================================================
// Core module JavaScript callbacks
// ============================================================================

// addTickCallback(fn) -> id
static JSValueRef JS_addTickCallback(JSContextRef ctx, JSObjectRef function,
    JSObjectRef thisObject, size_t argumentCount, const JSValueRef arguments[], JSValueRef* exception) {

    if (argumentCount < 1 || !JSValueIsObject(ctx, arguments[0])) {
        *exception = JSValueMakeString(ctx, CreateJSString("addTickCallback requires a function argument"));
        return JSValueMakeUndefined(ctx);
    }

    JSObjectRef callback = JSValueToObject(ctx, arguments[0], nullptr);
    if (!callback || !JSObjectIsFunction(ctx, callback)) {
        *exception = JSValueMakeString(ctx, CreateJSString("addTickCallback requires a function argument"));
        return JSValueMakeUndefined(ctx);
    }

    JSGlobalContextRef globalCtx = JSContextGetGlobalContext(ctx);

    std::lock_guard<std::mutex> lock(g_callbacksMutex);
    auto& callbacks = g_contextCallbacks[globalCtx];

    CallbackId id = GetNextId(callbacks);
    ProtectCallback(ctx, callback);
    callbacks.tickCallbacks.push_back({id, callback, true});

    spdlog::debug("[Core] Added tick callback id={}", id);
    return JSValueMakeNumber(ctx, id);
}

// removeTickCallback(id)
static JSValueRef JS_removeTickCallback(JSContextRef ctx, JSObjectRef function,
    JSObjectRef thisObject, size_t argumentCount, const JSValueRef arguments[], JSValueRef* exception) {

    if (argumentCount < 1) {
        *exception = JSValueMakeString(ctx, CreateJSString("removeTickCallback requires an id argument"));
        return JSValueMakeUndefined(ctx);
    }

    CallbackId id = static_cast<CallbackId>(JSValueToNumber(ctx, arguments[0], nullptr));
    JSGlobalContextRef globalCtx = JSContextGetGlobalContext(ctx);

    std::lock_guard<std::mutex> lock(g_callbacksMutex);
    auto it = g_contextCallbacks.find(globalCtx);
    if (it != g_contextCallbacks.end()) {
        auto& vec = it->second.tickCallbacks;
        auto found = std::find_if(vec.begin(), vec.end(), [id](const CallbackEntry& e) { return e.id == id; });
        if (found != vec.end()) {
            UnprotectCallback(ctx, found->callback);
            vec.erase(found);
            spdlog::debug("[Core] Removed tick callback id={}", id);
            return JSValueMakeBoolean(ctx, true);
        }
    }

    return JSValueMakeBoolean(ctx, false);
}

// addKeyDownCallback(fn) -> id
static JSValueRef JS_addKeyDownCallback(JSContextRef ctx, JSObjectRef function,
    JSObjectRef thisObject, size_t argumentCount, const JSValueRef arguments[], JSValueRef* exception) {

    if (argumentCount < 1 || !JSValueIsObject(ctx, arguments[0])) {
        *exception = JSValueMakeString(ctx, CreateJSString("addKeyDownCallback requires a function argument"));
        return JSValueMakeUndefined(ctx);
    }

    JSObjectRef callback = JSValueToObject(ctx, arguments[0], nullptr);
    if (!callback || !JSObjectIsFunction(ctx, callback)) {
        *exception = JSValueMakeString(ctx, CreateJSString("addKeyDownCallback requires a function argument"));
        return JSValueMakeUndefined(ctx);
    }

    JSGlobalContextRef globalCtx = JSContextGetGlobalContext(ctx);

    std::lock_guard<std::mutex> lock(g_callbacksMutex);
    auto& callbacks = g_contextCallbacks[globalCtx];

    CallbackId id = GetNextId(callbacks);
    ProtectCallback(ctx, callback);
    callbacks.keyDownCallbacks.push_back({id, callback, true});

    spdlog::debug("[Core] Added keyDown callback id={}", id);
    return JSValueMakeNumber(ctx, id);
}

// removeKeyDownCallback(id)
static JSValueRef JS_removeKeyDownCallback(JSContextRef ctx, JSObjectRef function,
    JSObjectRef thisObject, size_t argumentCount, const JSValueRef arguments[], JSValueRef* exception) {

    if (argumentCount < 1) {
        *exception = JSValueMakeString(ctx, CreateJSString("removeKeyDownCallback requires an id argument"));
        return JSValueMakeUndefined(ctx);
    }

    CallbackId id = static_cast<CallbackId>(JSValueToNumber(ctx, arguments[0], nullptr));
    JSGlobalContextRef globalCtx = JSContextGetGlobalContext(ctx);

    std::lock_guard<std::mutex> lock(g_callbacksMutex);
    auto it = g_contextCallbacks.find(globalCtx);
    if (it != g_contextCallbacks.end()) {
        auto& vec = it->second.keyDownCallbacks;
        auto found = std::find_if(vec.begin(), vec.end(), [id](const CallbackEntry& e) { return e.id == id; });
        if (found != vec.end()) {
            UnprotectCallback(ctx, found->callback);
            vec.erase(found);
            spdlog::debug("[Core] Removed keyDown callback id={}", id);
            return JSValueMakeBoolean(ctx, true);
        }
    }

    return JSValueMakeBoolean(ctx, false);
}

// addKeyUpCallback(fn) -> id
static JSValueRef JS_addKeyUpCallback(JSContextRef ctx, JSObjectRef function,
    JSObjectRef thisObject, size_t argumentCount, const JSValueRef arguments[], JSValueRef* exception) {

    if (argumentCount < 1 || !JSValueIsObject(ctx, arguments[0])) {
        *exception = JSValueMakeString(ctx, CreateJSString("addKeyUpCallback requires a function argument"));
        return JSValueMakeUndefined(ctx);
    }

    JSObjectRef callback = JSValueToObject(ctx, arguments[0], nullptr);
    if (!callback || !JSObjectIsFunction(ctx, callback)) {
        *exception = JSValueMakeString(ctx, CreateJSString("addKeyUpCallback requires a function argument"));
        return JSValueMakeUndefined(ctx);
    }

    JSGlobalContextRef globalCtx = JSContextGetGlobalContext(ctx);

    std::lock_guard<std::mutex> lock(g_callbacksMutex);
    auto& callbacks = g_contextCallbacks[globalCtx];

    CallbackId id = GetNextId(callbacks);
    ProtectCallback(ctx, callback);
    callbacks.keyUpCallbacks.push_back({id, callback, true});

    spdlog::debug("[Core] Added keyUp callback id={}", id);
    return JSValueMakeNumber(ctx, id);
}

// removeKeyUpCallback(id)
static JSValueRef JS_removeKeyUpCallback(JSContextRef ctx, JSObjectRef function,
    JSObjectRef thisObject, size_t argumentCount, const JSValueRef arguments[], JSValueRef* exception) {

    if (argumentCount < 1) {
        *exception = JSValueMakeString(ctx, CreateJSString("removeKeyUpCallback requires an id argument"));
        return JSValueMakeUndefined(ctx);
    }

    CallbackId id = static_cast<CallbackId>(JSValueToNumber(ctx, arguments[0], nullptr));
    JSGlobalContextRef globalCtx = JSContextGetGlobalContext(ctx);

    std::lock_guard<std::mutex> lock(g_callbacksMutex);
    auto it = g_contextCallbacks.find(globalCtx);
    if (it != g_contextCallbacks.end()) {
        auto& vec = it->second.keyUpCallbacks;
        auto found = std::find_if(vec.begin(), vec.end(), [id](const CallbackEntry& e) { return e.id == id; });
        if (found != vec.end()) {
            UnprotectCallback(ctx, found->callback);
            vec.erase(found);
            spdlog::debug("[Core] Removed keyUp callback id={}", id);
            return JSValueMakeBoolean(ctx, true);
        }
    }

    return JSValueMakeBoolean(ctx, false);
}

// Current game time for timers
static uint32_t g_currentGameTime = 0;

// setTimeout(fn, ms) -> id
static JSValueRef JS_setTimeout(JSContextRef ctx, JSObjectRef function,
    JSObjectRef thisObject, size_t argumentCount, const JSValueRef arguments[], JSValueRef* exception) {

    if (argumentCount < 2) {
        *exception = JSValueMakeString(ctx, CreateJSString("setTimeout requires function and delay arguments"));
        return JSValueMakeUndefined(ctx);
    }

    if (!JSValueIsObject(ctx, arguments[0])) {
        *exception = JSValueMakeString(ctx, CreateJSString("setTimeout first argument must be a function"));
        return JSValueMakeUndefined(ctx);
    }

    JSObjectRef callback = JSValueToObject(ctx, arguments[0], nullptr);
    if (!callback || !JSObjectIsFunction(ctx, callback)) {
        *exception = JSValueMakeString(ctx, CreateJSString("setTimeout first argument must be a function"));
        return JSValueMakeUndefined(ctx);
    }

    uint32_t delay = static_cast<uint32_t>(JSValueToNumber(ctx, arguments[1], nullptr));
    JSGlobalContextRef globalCtx = JSContextGetGlobalContext(ctx);

    std::lock_guard<std::mutex> lock(g_callbacksMutex);
    auto& callbacks = g_contextCallbacks[globalCtx];

    CallbackId id = GetNextId(callbacks);
    ProtectCallback(ctx, callback);

    TimerEntry timer;
    timer.id = id;
    timer.callback = callback;
    timer.triggerTime = g_currentGameTime + delay;
    timer.interval = 0;  // One-shot
    timer.cancelled = false;
    callbacks.timers.push_back(timer);

    spdlog::debug("[Core] Added setTimeout id={} delay={}ms", id, delay);
    return JSValueMakeNumber(ctx, id);
}

// clearTimeout(id)
static JSValueRef JS_clearTimeout(JSContextRef ctx, JSObjectRef function,
    JSObjectRef thisObject, size_t argumentCount, const JSValueRef arguments[], JSValueRef* exception) {

    if (argumentCount < 1) {
        return JSValueMakeUndefined(ctx);
    }

    CallbackId id = static_cast<CallbackId>(JSValueToNumber(ctx, arguments[0], nullptr));
    JSGlobalContextRef globalCtx = JSContextGetGlobalContext(ctx);

    std::lock_guard<std::mutex> lock(g_callbacksMutex);
    auto it = g_contextCallbacks.find(globalCtx);
    if (it != g_contextCallbacks.end()) {
        for (auto& timer : it->second.timers) {
            if (timer.id == id) {
                timer.cancelled = true;
                spdlog::debug("[Core] Cancelled timeout id={}", id);
                break;
            }
        }
    }

    return JSValueMakeUndefined(ctx);
}

// setInterval(fn, ms) -> id
static JSValueRef JS_setInterval(JSContextRef ctx, JSObjectRef function,
    JSObjectRef thisObject, size_t argumentCount, const JSValueRef arguments[], JSValueRef* exception) {

    if (argumentCount < 2) {
        *exception = JSValueMakeString(ctx, CreateJSString("setInterval requires function and interval arguments"));
        return JSValueMakeUndefined(ctx);
    }

    if (!JSValueIsObject(ctx, arguments[0])) {
        *exception = JSValueMakeString(ctx, CreateJSString("setInterval first argument must be a function"));
        return JSValueMakeUndefined(ctx);
    }

    JSObjectRef callback = JSValueToObject(ctx, arguments[0], nullptr);
    if (!callback || !JSObjectIsFunction(ctx, callback)) {
        *exception = JSValueMakeString(ctx, CreateJSString("setInterval first argument must be a function"));
        return JSValueMakeUndefined(ctx);
    }

    uint32_t interval = static_cast<uint32_t>(JSValueToNumber(ctx, arguments[1], nullptr));
    if (interval == 0) interval = 1;  // Minimum 1ms

    JSGlobalContextRef globalCtx = JSContextGetGlobalContext(ctx);

    std::lock_guard<std::mutex> lock(g_callbacksMutex);
    auto& callbacks = g_contextCallbacks[globalCtx];

    CallbackId id = GetNextId(callbacks);
    ProtectCallback(ctx, callback);

    TimerEntry timer;
    timer.id = id;
    timer.callback = callback;
    timer.triggerTime = g_currentGameTime + interval;
    timer.interval = interval;
    timer.cancelled = false;
    callbacks.timers.push_back(timer);

    spdlog::debug("[Core] Added setInterval id={} interval={}ms", id, interval);
    return JSValueMakeNumber(ctx, id);
}

// clearInterval(id) - same as clearTimeout
static JSValueRef JS_clearInterval(JSContextRef ctx, JSObjectRef function,
    JSObjectRef thisObject, size_t argumentCount, const JSValueRef arguments[], JSValueRef* exception) {
    return JS_clearTimeout(ctx, function, thisObject, argumentCount, arguments, exception);
}

// getGameTime() -> ms
static JSValueRef JS_getGameTime(JSContextRef ctx, JSObjectRef function,
    JSObjectRef thisObject, size_t argumentCount, const JSValueRef arguments[], JSValueRef* exception) {
    return JSValueMakeNumber(ctx, g_currentGameTime);
}

// ============================================================================
// Public API
// ============================================================================

void InitCoreModule() {
    std::lock_guard<std::mutex> lock(g_callbacksMutex);
    g_contextCallbacks.clear();
    g_currentGameTime = 0;
    spdlog::debug("[Core] Core module initialized");
}

JSObjectRef CreateCoreModuleObject(JSGlobalContextRef ctx) {
    JSObjectRef coreObj = JSObjectMake(ctx, nullptr, nullptr);

    // Helper macro for registering functions
    #define REGISTER_FUNC(name, func) do { \
        JSStringRef jsName = CreateJSString(name); \
        JSObjectRef jsFunc = JSObjectMakeFunctionWithCallback(ctx, jsName, func); \
        JSObjectSetProperty(ctx, coreObj, jsName, jsFunc, kJSPropertyAttributeReadOnly | kJSPropertyAttributeDontDelete, nullptr); \
        JSStringRelease(jsName); \
    } while(0)

    REGISTER_FUNC("addTickCallback", JS_addTickCallback);
    REGISTER_FUNC("removeTickCallback", JS_removeTickCallback);
    REGISTER_FUNC("addKeyDownCallback", JS_addKeyDownCallback);
    REGISTER_FUNC("removeKeyDownCallback", JS_removeKeyDownCallback);
    REGISTER_FUNC("addKeyUpCallback", JS_addKeyUpCallback);
    REGISTER_FUNC("removeKeyUpCallback", JS_removeKeyUpCallback);
    REGISTER_FUNC("getGameTime", JS_getGameTime);

    #undef REGISTER_FUNC

    spdlog::debug("[Core] Created core module object");
    return coreObj;
}

void RegisterGlobalTimers(JSGlobalContextRef ctx) {
    JSObjectRef globalObj = JSContextGetGlobalObject(ctx);

    // Register setTimeout/setInterval on global (standard browser-like APIs)
    JSStringRef setTimeoutName = CreateJSString("setTimeout");
    JSObjectRef setTimeoutFunc = JSObjectMakeFunctionWithCallback(ctx, setTimeoutName, JS_setTimeout);
    JSObjectSetProperty(ctx, globalObj, setTimeoutName, setTimeoutFunc, kJSPropertyAttributeNone, nullptr);
    JSStringRelease(setTimeoutName);

    JSStringRef clearTimeoutName = CreateJSString("clearTimeout");
    JSObjectRef clearTimeoutFunc = JSObjectMakeFunctionWithCallback(ctx, clearTimeoutName, JS_clearTimeout);
    JSObjectSetProperty(ctx, globalObj, clearTimeoutName, clearTimeoutFunc, kJSPropertyAttributeNone, nullptr);
    JSStringRelease(clearTimeoutName);

    JSStringRef setIntervalName = CreateJSString("setInterval");
    JSObjectRef setIntervalFunc = JSObjectMakeFunctionWithCallback(ctx, setIntervalName, JS_setInterval);
    JSObjectSetProperty(ctx, globalObj, setIntervalName, setIntervalFunc, kJSPropertyAttributeNone, nullptr);
    JSStringRelease(setIntervalName);

    JSStringRef clearIntervalName = CreateJSString("clearInterval");
    JSObjectRef clearIntervalFunc = JSObjectMakeFunctionWithCallback(ctx, clearIntervalName, JS_clearInterval);
    JSObjectSetProperty(ctx, globalObj, clearIntervalName, clearIntervalFunc, kJSPropertyAttributeNone, nullptr);
    JSStringRelease(clearIntervalName);

    spdlog::debug("[Core] Registered global timer functions");
}

void ExecuteTickCallbacks(JSGlobalContextRef ctx) {
    std::vector<JSObjectRef> callbacksCopy;

    {
        std::lock_guard<std::mutex> lock(g_callbacksMutex);
        auto it = g_contextCallbacks.find(ctx);
        if (it == g_contextCallbacks.end()) return;

        for (const auto& entry : it->second.tickCallbacks) {
            callbacksCopy.push_back(entry.callback);
        }
    }

    for (JSObjectRef callback : callbacksCopy) {
        JSValueRef exception = nullptr;
        JSObjectCallAsFunction(ctx, callback, nullptr, 0, nullptr, &exception);
        if (exception) {
            JSStringRef exStr = JSValueToStringCopy(ctx, exception, nullptr);
            size_t maxSize = JSStringGetMaximumUTF8CStringSize(exStr);
            std::vector<char> buffer(maxSize);
            JSStringGetUTF8CString(exStr, buffer.data(), maxSize);
            JSStringRelease(exStr);
            spdlog::error("[Core] Tick callback error: {}", buffer.data());
        }
    }

    // Drain microtasks to process Promise continuations
    DrainMicrotasks(ctx);
}

void ExecuteKeyDownCallbacks(JSGlobalContextRef ctx, int key) {
    std::vector<JSObjectRef> callbacksCopy;

    {
        std::lock_guard<std::mutex> lock(g_callbacksMutex);
        auto it = g_contextCallbacks.find(ctx);
        if (it == g_contextCallbacks.end()) return;

        for (const auto& entry : it->second.keyDownCallbacks) {
            callbacksCopy.push_back(entry.callback);
        }
    }

    JSValueRef keyArg = JSValueMakeNumber(ctx, key);

    for (JSObjectRef callback : callbacksCopy) {
        JSValueRef exception = nullptr;
        JSObjectCallAsFunction(ctx, callback, nullptr, 1, &keyArg, &exception);
        if (exception) {
            JSStringRef exStr = JSValueToStringCopy(ctx, exception, nullptr);
            size_t maxSize = JSStringGetMaximumUTF8CStringSize(exStr);
            std::vector<char> buffer(maxSize);
            JSStringGetUTF8CString(exStr, buffer.data(), maxSize);
            JSStringRelease(exStr);
            spdlog::error("[Core] KeyDown callback error: {}", buffer.data());
        }
    }

    // Drain microtasks to process Promise continuations
    DrainMicrotasks(ctx);
}

void ExecuteKeyUpCallbacks(JSGlobalContextRef ctx, int key) {
    std::vector<JSObjectRef> callbacksCopy;

    {
        std::lock_guard<std::mutex> lock(g_callbacksMutex);
        auto it = g_contextCallbacks.find(ctx);
        if (it == g_contextCallbacks.end()) return;

        for (const auto& entry : it->second.keyUpCallbacks) {
            callbacksCopy.push_back(entry.callback);
        }
    }

    JSValueRef keyArg = JSValueMakeNumber(ctx, key);

    for (JSObjectRef callback : callbacksCopy) {
        JSValueRef exception = nullptr;
        JSObjectCallAsFunction(ctx, callback, nullptr, 1, &keyArg, &exception);
        if (exception) {
            JSStringRef exStr = JSValueToStringCopy(ctx, exception, nullptr);
            size_t maxSize = JSStringGetMaximumUTF8CStringSize(exStr);
            std::vector<char> buffer(maxSize);
            JSStringGetUTF8CString(exStr, buffer.data(), maxSize);
            JSStringRelease(exStr);
            spdlog::error("[Core] KeyUp callback error: {}", buffer.data());
        }
    }

    // Drain microtasks to process Promise continuations
    DrainMicrotasks(ctx);
}

void ProcessTimers(JSGlobalContextRef ctx, uint32_t currentTime) {
    g_currentGameTime = currentTime;

    std::vector<std::pair<JSObjectRef, CallbackId>> toExecute;
    std::vector<CallbackId> toRemove;

    {
        std::lock_guard<std::mutex> lock(g_callbacksMutex);
        auto it = g_contextCallbacks.find(ctx);
        if (it == g_contextCallbacks.end()) return;

        for (auto& timer : it->second.timers) {
            if (timer.cancelled) {
                toRemove.push_back(timer.id);
                continue;
            }

            if (currentTime >= timer.triggerTime) {
                toExecute.push_back({timer.callback, timer.id});

                if (timer.interval > 0) {
                    // Reschedule interval timer
                    timer.triggerTime = currentTime + timer.interval;
                } else {
                    // One-shot timer - mark for removal
                    toRemove.push_back(timer.id);
                }
            }
        }

        // Remove cancelled and completed one-shot timers
        auto& timers = it->second.timers;
        timers.erase(
            std::remove_if(timers.begin(), timers.end(),
                [&toRemove, ctx](const TimerEntry& t) {
                    bool shouldRemove = std::find(toRemove.begin(), toRemove.end(), t.id) != toRemove.end();
                    if (shouldRemove && t.interval == 0) {
                        UnprotectCallback(ctx, t.callback);
                    }
                    return shouldRemove;
                }),
            timers.end());
    }

    // Execute callbacks outside the lock
    for (const auto& [callback, id] : toExecute) {
        JSValueRef exception = nullptr;
        JSObjectCallAsFunction(ctx, callback, nullptr, 0, nullptr, &exception);
        if (exception) {
            JSStringRef exStr = JSValueToStringCopy(ctx, exception, nullptr);
            size_t maxSize = JSStringGetMaximumUTF8CStringSize(exStr);
            std::vector<char> buffer(maxSize);
            JSStringGetUTF8CString(exStr, buffer.data(), maxSize);
            JSStringRelease(exStr);
            spdlog::error("[Core] Timer {} error: {}", id, buffer.data());
        }
    }

    // Drain microtasks to process Promise continuations (for setTimeout/setInterval)
    if (!toExecute.empty()) {
        DrainMicrotasks(ctx);
    }
}

void ClearAllCallbacks(JSGlobalContextRef ctx) {
    std::lock_guard<std::mutex> lock(g_callbacksMutex);
    auto it = g_contextCallbacks.find(ctx);
    if (it != g_contextCallbacks.end()) {
        // Unprotect all callbacks
        for (const auto& entry : it->second.tickCallbacks) {
            UnprotectCallback(ctx, entry.callback);
        }
        for (const auto& entry : it->second.keyDownCallbacks) {
            UnprotectCallback(ctx, entry.callback);
        }
        for (const auto& entry : it->second.keyUpCallbacks) {
            UnprotectCallback(ctx, entry.callback);
        }
        for (const auto& timer : it->second.timers) {
            UnprotectCallback(ctx, timer.callback);
        }

        g_contextCallbacks.erase(it);
        spdlog::debug("[Core] Cleared all callbacks for context");
    }
}

size_t GetTickCallbackCount() {
    std::lock_guard<std::mutex> lock(g_callbacksMutex);
    size_t count = 0;
    for (const auto& [ctx, callbacks] : g_contextCallbacks) {
        count += callbacks.tickCallbacks.size();
    }
    return count;
}

size_t GetActiveTimerCount() {
    std::lock_guard<std::mutex> lock(g_callbacksMutex);
    size_t count = 0;
    for (const auto& [ctx, callbacks] : g_contextCallbacks) {
        for (const auto& timer : callbacks.timers) {
            if (!timer.cancelled) count++;
        }
    }
    return count;
}

} // namespace rdr2js
