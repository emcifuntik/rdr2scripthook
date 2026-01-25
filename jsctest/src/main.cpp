// JSC Test - Standalone JavaScriptCore test executable
// This mirrors the JSRuntime initialization logic for easier debugging

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <unordered_map>
#include <mutex>
#include <filesystem>
#include <thread>
#include <chrono>
#include <algorithm>

#include <JavaScriptCore/JavaScript.h>

// JSC internal headers for module support
#include <JavaScriptCore/APICast.h>
#include <JavaScriptCore/SourceCode.h>
#include <JavaScriptCore/Completion.h>
#include <JavaScriptCore/JSInternalPromise.h>
#include <JavaScriptCore/JSPromise.h>
#include <JavaScriptCore/ExceptionScope.h>
#include <JavaScriptCore/VM.h>
#include <JavaScriptCore/JSLock.h>
#include <JavaScriptCore/JSGlobalObject.h>
#include <JavaScriptCore/JSModuleLoader.h>
#include <JavaScriptCore/JSModuleNamespaceObject.h>
#include <JavaScriptCore/JSString.h>
#include <JavaScriptCore/InitializeThreading.h>
#include <JavaScriptCore/DFGDoesGCCheck.h>
#include <JavaScriptCore/GlobalObjectMethodTable.h>
#include <JavaScriptCore/Identifier.h>
#include <JavaScriptCore/JSSourceCode.h>
#include <JavaScriptCore/RuntimeFlags.h>
#include <JavaScriptCore/Symbol.h>

// WTF initialization headers
#include <wtf/Threading.h>
#include <wtf/MainThread.h>
#include <wtf/DataLog.h>
#include <wtf/URL.h>
#include <JavaScriptCore/JSCConfig.h>
#include <JavaScriptCore/Options.h>

// Global map for module sources
static std::unordered_map<JSC::JSGlobalObject*, std::unordered_map<std::string, std::string>> g_contextModuleSources;
static std::mutex g_contextModuleSourcesMutex;

// Store the last evaluated module record for namespace retrieval
static std::unordered_map<JSC::JSGlobalObject*, JSC::JSValue> g_lastModuleRecord;
static std::mutex g_lastModuleRecordMutex;

// Forward declarations
std::string formatException(JSContextRef ctx, JSValueRef exception);

// ============================================================================
// Timer Support for Promise testing
// ============================================================================

struct TimerEntry {
    uint32_t id;
    JSObjectRef callback;
    JSGlobalContextRef ctx;
    std::chrono::steady_clock::time_point triggerTime;
    uint32_t interval;  // 0 for setTimeout, >0 for setInterval (ms)
    bool cancelled;
};

static std::vector<TimerEntry> g_timers;
static std::mutex g_timersMutex;
static uint32_t g_nextTimerId = 1;

// ============================================================================
// Custom GlobalObjectMethodTable Callbacks
// ============================================================================

// Return whether we support rich source info (stack traces, etc.)
static bool customSupportsRichSourceInfo(const JSC::JSGlobalObject*)
{
    return true;
}

// Return whether scripts should be interrupted (for infinite loop prevention)
static bool customShouldInterruptScript(const JSC::JSGlobalObject*)
{
    return false;
}

// Return runtime flags for the JavaScript environment
static JSC::RuntimeFlags customJavaScriptRuntimeFlags(const JSC::JSGlobalObject*)
{
    return JSC::RuntimeFlags();
}

// Return whether scripts should be interrupted before timeout
static bool customShouldInterruptScriptBeforeTimeout(const JSC::JSGlobalObject*)
{
    return false;
}

// Return the structure for trusted scripts (nullptr = no trusted script support)
static JSC::Structure* customTrustedScriptStructure(JSC::JSGlobalObject*)
{
    return nullptr;
}

// Track promise rejections (for unhandled rejection reporting)
static void customPromiseRejectionTracker(JSC::JSGlobalObject*, JSC::JSPromise*, JSC::JSPromiseRejectionOperation)
{
    // No-op: we don't track unhandled promise rejections
}

// ============================================================================
// Custom Module Loader Callbacks
// ============================================================================

// Resolve a module specifier to a module key
static JSC::Identifier customModuleLoaderResolve(
    JSC::JSGlobalObject* globalObject,
    JSC::JSModuleLoader*,
    JSC::JSValue keyValue,
    JSC::JSValue referrerValue,
    JSC::JSValue)
{
    JSC::VM& vm = globalObject->vm();

    // Handle symbol keys (JSC uses symbols for module identifiers internally)
    if (keyValue.isSymbol()) {
        JSC::Symbol* symbol = JSC::asSymbol(keyValue);
        const auto& privateName = symbol->privateName();
        std::cout << "[Module] Resolve: symbol key" << std::endl;
        return JSC::Identifier::fromUid(privateName);
    }

    if (keyValue.isString()) {
        WTF::String key = keyValue.toWTFString(globalObject);
        std::cout << "[Module] Resolve: string key='" << key.utf8().data() << "'" << std::endl;
        return JSC::Identifier::fromString(vm, key);
    }

    std::cerr << "[Module] Resolve: unknown key type" << std::endl;
    return JSC::Identifier();
}

// Fetch a module's source code
static JSC::JSInternalPromise* customModuleLoaderFetch(
    JSC::JSGlobalObject* globalObject,
    JSC::JSModuleLoader*,
    JSC::JSValue keyValue,
    JSC::JSValue,
    JSC::JSValue)
{
    JSC::VM& vm = globalObject->vm();

    WTF::String key;
    if (keyValue.isSymbol()) {
        // Symbol keys are used by JSC for internal module tracking
        // We shouldn't try to fetch these - the main module source is already
        // provided inline via loadAndEvaluateModule, so reject symbol fetches
        std::cout << "[Module] Fetch: symbol key - rejecting (main module already provided inline)" << std::endl;
        auto* promise = JSC::JSInternalPromise::create(vm, globalObject->internalPromiseStructure());
        promise->reject(vm, globalObject, JSC::createError(globalObject, "Cannot fetch module by symbol key"_s));
        return promise;
    } else if (keyValue.isString()) {
        key = keyValue.toWTFString(globalObject);
        std::cout << "[Module] Fetch: string key='" << key.utf8().data() << "'" << std::endl;
    } else {
        std::cerr << "[Module] Fetch: unknown key type" << std::endl;
        auto* promise = JSC::JSInternalPromise::create(vm, globalObject->internalPromiseStructure());
        promise->reject(vm, globalObject, JSC::createError(globalObject, "Invalid module key type"_s));
        return promise;
    }

    // Look up the module source in our map
    std::string moduleKey = key.utf8().data();
    std::string moduleSource;

    {
        std::lock_guard<std::mutex> lock(g_contextModuleSourcesMutex);
        auto contextIt = g_contextModuleSources.find(globalObject);
        if (contextIt != g_contextModuleSources.end()) {
            auto moduleIt = contextIt->second.find(moduleKey);
            if (moduleIt != contextIt->second.end()) {
                moduleSource = moduleIt->second;
            }
        }
    }

    if (moduleSource.empty()) {
        std::cerr << "[Module] Module not found: " << moduleKey << std::endl;
        auto* promise = JSC::JSInternalPromise::create(vm, globalObject->internalPromiseStructure());
        WTF::String errorMsg = WTF::makeString("Module not found: "_s, key);
        promise->reject(vm, globalObject, JSC::createError(globalObject, errorMsg));
        return promise;
    }

    // Create source code for the module
    // Use std::span for proper handling of large buffers
    WTF::String sourceString = WTF::String::fromUTF8(std::span<const char>(moduleSource.data(), moduleSource.size()));
    if (sourceString.isNull()) {
        std::cerr << "[Module] Failed to create WTF::String from module source" << std::endl;
        auto* promise = JSC::JSInternalPromise::create(vm, globalObject->internalPromiseStructure());
        promise->reject(vm, globalObject, JSC::createError(globalObject, "Failed to convert module source to string"_s));
        return promise;
    }

    // Create proper SourceOrigin with URL for better error messages
    WTF::URL moduleURL = WTF::URL(WTF::URL(), key);
    JSC::SourceOrigin sourceOrigin(moduleURL);

    JSC::SourceCode sourceCode = JSC::makeSource(
        sourceString,
        sourceOrigin,
        JSC::SourceTaintedOrigin::Untainted,
        key,
        WTF::TextPosition(),
        JSC::SourceProviderSourceType::Module
    );

    // Create a resolved promise with the source code
    auto* promise = JSC::JSInternalPromise::create(vm, globalObject->internalPromiseStructure());
    promise->resolve(globalObject, JSC::JSSourceCode::create(vm, std::move(sourceCode)));

    return promise;
}

// Handle module evaluation - capture the module record for namespace retrieval
static JSC::JSValue customModuleLoaderEvaluate(
    JSC::JSGlobalObject* globalObject,
    JSC::JSModuleLoader* moduleLoader,
    JSC::JSValue key,
    JSC::JSValue moduleRecord,
    JSC::JSValue scriptFetcher,
    JSC::JSValue sentValue,
    JSC::JSValue resumeMode)
{
    std::cout << "[Module] Evaluate: storing module record" << std::endl;

    // Store the module record for later namespace retrieval
    {
        std::lock_guard<std::mutex> lock(g_lastModuleRecordMutex);
        g_lastModuleRecord[globalObject] = moduleRecord;
    }

    // Call the default evaluator
    return moduleLoader->evaluateNonVirtual(globalObject, key, moduleRecord, scriptFetcher, sentValue, resumeMode);
}

// Handle dynamic import() calls
static JSC::JSInternalPromise* customModuleLoaderImportModule(
    JSC::JSGlobalObject* globalObject,
    JSC::JSModuleLoader* moduleLoader,
    JSC::JSString* moduleNameValue,
    JSC::JSValue parameters,
    const JSC::SourceOrigin& sourceOrigin)
{
    JSC::VM& vm = globalObject->vm();

    WTF::String moduleName = moduleNameValue->value(globalObject);
    std::cout << "[Module] ImportModule: '" << moduleName.utf8().data() << "'" << std::endl;

    // Use the module loader to load and evaluate the module
    return moduleLoader->loadAndEvaluateModule(globalObject, moduleNameValue, parameters, JSC::jsUndefined());
}

// Custom GlobalObjectMethodTable with our module loader callbacks
static const JSC::GlobalObjectMethodTable s_customGlobalObjectMethodTable = {
    &customSupportsRichSourceInfo,
    &customShouldInterruptScript,
    &customJavaScriptRuntimeFlags,
    nullptr, // queueMicrotaskToEventLoop
    &customShouldInterruptScriptBeforeTimeout,
    &customModuleLoaderImportModule,
    &customModuleLoaderResolve,
    &customModuleLoaderFetch,
    nullptr, // moduleLoaderCreateImportMetaProperties
    &customModuleLoaderEvaluate,
    &customPromiseRejectionTracker,
    nullptr, // reportUncaughtExceptionAtEventLoop
    nullptr, // currentScriptExecutionOwner
    nullptr, // scriptExecutionStatus
    nullptr, // reportViolationForUnsafeEval
    nullptr, // defaultLanguage
    nullptr, // compileStreaming
    nullptr, // instantiateStreaming
    nullptr, // deriveShadowRealmGlobalObject
    nullptr, // codeForEval
    nullptr, // canCompileStrings
    &customTrustedScriptStructure,
};

// setTimeout(fn, ms) -> id
static JSValueRef jsSetTimeout(JSContextRef ctx, JSObjectRef function,
    JSObjectRef thisObject, size_t argumentCount,
    const JSValueRef arguments[], JSValueRef* exception) {

    if (argumentCount < 2) {
        return JSValueMakeNumber(ctx, 0);
    }

    if (!JSValueIsObject(ctx, arguments[0])) {
        return JSValueMakeNumber(ctx, 0);
    }

    JSObjectRef callback = JSValueToObject(ctx, arguments[0], nullptr);
    if (!callback || !JSObjectIsFunction(ctx, callback)) {
        return JSValueMakeNumber(ctx, 0);
    }

    uint32_t delay = static_cast<uint32_t>(JSValueToNumber(ctx, arguments[1], nullptr));

    JSValueProtect(ctx, callback);

    std::lock_guard<std::mutex> lock(g_timersMutex);
    uint32_t id = g_nextTimerId++;

    TimerEntry timer;
    timer.id = id;
    timer.callback = callback;
    timer.ctx = JSContextGetGlobalContext(ctx);
    timer.triggerTime = std::chrono::steady_clock::now() + std::chrono::milliseconds(delay);
    timer.interval = 0;  // One-shot
    timer.cancelled = false;
    g_timers.push_back(timer);

    return JSValueMakeNumber(ctx, id);
}

// setInterval(fn, ms) -> id
static JSValueRef jsSetInterval(JSContextRef ctx, JSObjectRef function,
    JSObjectRef thisObject, size_t argumentCount,
    const JSValueRef arguments[], JSValueRef* exception) {

    if (argumentCount < 2) {
        return JSValueMakeNumber(ctx, 0);
    }

    if (!JSValueIsObject(ctx, arguments[0])) {
        return JSValueMakeNumber(ctx, 0);
    }

    JSObjectRef callback = JSValueToObject(ctx, arguments[0], nullptr);
    if (!callback || !JSObjectIsFunction(ctx, callback)) {
        return JSValueMakeNumber(ctx, 0);
    }

    uint32_t interval = static_cast<uint32_t>(JSValueToNumber(ctx, arguments[1], nullptr));
    if (interval == 0) interval = 1;

    JSValueProtect(ctx, callback);

    std::lock_guard<std::mutex> lock(g_timersMutex);
    uint32_t id = g_nextTimerId++;

    TimerEntry timer;
    timer.id = id;
    timer.callback = callback;
    timer.ctx = JSContextGetGlobalContext(ctx);
    timer.triggerTime = std::chrono::steady_clock::now() + std::chrono::milliseconds(interval);
    timer.interval = interval;
    timer.cancelled = false;
    g_timers.push_back(timer);

    return JSValueMakeNumber(ctx, id);
}

// clearTimeout(id) / clearInterval(id)
static JSValueRef jsClearTimer(JSContextRef ctx, JSObjectRef function,
    JSObjectRef thisObject, size_t argumentCount,
    const JSValueRef arguments[], JSValueRef* exception) {

    if (argumentCount < 1) {
        return JSValueMakeUndefined(ctx);
    }

    uint32_t id = static_cast<uint32_t>(JSValueToNumber(ctx, arguments[0], nullptr));

    std::lock_guard<std::mutex> lock(g_timersMutex);
    for (auto& timer : g_timers) {
        if (timer.id == id) {
            timer.cancelled = true;
            break;
        }
    }

    return JSValueMakeUndefined(ctx);
}

// Process timers and return true if any timers are still pending
static bool processTimers(JSC::VM& vm) {
    std::vector<std::pair<JSObjectRef, JSGlobalContextRef>> toExecute;
    std::vector<uint32_t> toRemove;
    auto now = std::chrono::steady_clock::now();

    {
        std::lock_guard<std::mutex> lock(g_timersMutex);

        for (auto& timer : g_timers) {
            if (timer.cancelled) {
                toRemove.push_back(timer.id);
                continue;
            }

            if (now >= timer.triggerTime) {
                toExecute.push_back({timer.callback, timer.ctx});

                if (timer.interval > 0) {
                    // Reschedule interval timer
                    timer.triggerTime = now + std::chrono::milliseconds(timer.interval);
                } else {
                    // One-shot timer - mark for removal
                    toRemove.push_back(timer.id);
                }
            }
        }

        // Remove cancelled and completed one-shot timers
        g_timers.erase(
            std::remove_if(g_timers.begin(), g_timers.end(),
                [&toRemove](const TimerEntry& t) {
                    bool shouldRemove = std::find(toRemove.begin(), toRemove.end(), t.id) != toRemove.end();
                    if (shouldRemove) {
                        JSValueUnprotect(t.ctx, t.callback);
                    }
                    return shouldRemove;
                }),
            g_timers.end());
    }

    // Execute callbacks outside the lock
    for (const auto& [callback, ctx] : toExecute) {
        JSValueRef exception = nullptr;
        JSObjectCallAsFunction(ctx, callback, nullptr, 0, nullptr, &exception);
        if (exception) {
            std::cerr << "[Timer] Error: " << formatException(ctx, exception) << std::endl;
        }
    }

    // Drain microtasks after executing timer callbacks (for Promise support)
    if (!toExecute.empty()) {
        vm.drainMicrotasks();
    }

    // Return true if there are still pending timers
    std::lock_guard<std::mutex> lock(g_timersMutex);
    return !g_timers.empty();
}

// Console log function for JS
static JSValueRef jsConsoleLog(JSContextRef ctx, JSObjectRef function,
    JSObjectRef thisObject, size_t argumentCount,
    const JSValueRef arguments[], JSValueRef* exception) {

    for (size_t i = 0; i < argumentCount; i++) {
        if (i > 0) std::cout << " ";

        JSStringRef jsStr = JSValueToStringCopy(ctx, arguments[i], exception);
        if (jsStr) {
            size_t maxSize = JSStringGetMaximumUTF8CStringSize(jsStr);
            std::vector<char> buffer(maxSize);
            JSStringGetUTF8CString(jsStr, buffer.data(), maxSize);
            std::cout << buffer.data();
            JSStringRelease(jsStr);
        }
    }
    std::cout << std::endl;

    return JSValueMakeUndefined(ctx);
}

// Read file contents
std::string readFile(const std::string& path) {
    std::ifstream file(path);
    if (!file.is_open()) {
        std::cerr << "Error: Could not open file: " << path << std::endl;
        return "";
    }
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

// Format JS exception
std::string formatException(JSContextRef ctx, JSValueRef exception) {
    if (!exception) return "(null exception)";

    JSStringRef jsStr = JSValueToStringCopy(ctx, exception, nullptr);
    if (!jsStr) return "(error converting exception to string)";

    size_t maxSize = JSStringGetMaximumUTF8CStringSize(jsStr);
    std::vector<char> buffer(maxSize);
    JSStringGetUTF8CString(jsStr, buffer.data(), maxSize);
    std::string result(buffer.data());
    JSStringRelease(jsStr);
    return result;
}

void printUsage(const char* progName) {
    std::cout << "Usage: " << progName << " <script.js> [--module]" << std::endl;
    std::cout << "  --module    Load as ES6 module instead of script" << std::endl;
}

// Provide a module source for the given context
void provideModule(JSC::JSGlobalObject* globalObject, const std::string& moduleKey, const std::string& code) {
    std::lock_guard<std::mutex> lock(g_contextModuleSourcesMutex);
    g_contextModuleSources[globalObject][moduleKey] = code;
    std::cout << "[Module] Provided module '" << moduleKey << "' (" << code.size() << " bytes)" << std::endl;
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        printUsage(argv[0]);
        return 1;
    }

    std::string scriptPath = argv[1];
    bool isModule = false;

    for (int i = 2; i < argc; i++) {
        if (std::string(argv[i]) == "--module") {
            isModule = true;
        }
    }

    std::string code = readFile(scriptPath);
    if (code.empty()) {
        return 1;
    }

    std::cout << "=== JSC Test ===" << std::endl;
    std::cout << "Script: " << scriptPath << std::endl;
    std::cout << "Mode: " << (isModule ? "ES6 Module" : "Script") << std::endl;
    std::cout << "================\n" << std::endl;

    // Set JSC log file
    WTF::setDataFile("jsc.log");

    // Initialize JSC (mirrors JSRuntime::Initialize)
    std::cout << "[Init] Enabling restricted options..." << std::endl;
    JSC::Config::enableRestrictedOptions();

    std::cout << "[Init] Initializing main thread..." << std::endl;
    WTF::initializeMainThread();

    std::cout << "[Init] Initializing JSC..." << std::endl;
    JSC::initialize();

    // Configure JSC options
    std::cout << "[Init] Configuring JSC options..." << std::endl;
    {
        JSC::Options::AllowUnfinalizedAccessScope scope;

        JSC::Options::useConcurrentJIT() = true;
        JSC::Options::useWasm() = true;
        JSC::Options::useSourceProviderCache() = true;
        JSC::Options::exposeInternalModuleLoader() = true;
        JSC::Options::useSharedArrayBuffer() = true;
        JSC::Options::useJIT() = true;
        JSC::Options::useBBQJIT() = true;
        JSC::Options::useJITCage() = false;
        JSC::Options::useShadowRealm() = true;
        JSC::Options::useV8DateParser() = true;
        JSC::Options::useMathSumPreciseMethod() = true;
        JSC::Options::evalMode() = false;
        JSC::Options::heapGrowthSteepnessFactor() = 1.0;
        JSC::Options::heapGrowthMaxIncrease() = 2.0;
        JSC::Options::useAsyncStackTrace() = true;
        JSC::Options::useExplicitResourceManagement() = true;
        JSC::Options::assertOptionsAreCoherent();
    }

    std::cout << "[Init] Creating VM..." << std::endl;

    using namespace JSC;

    // Create VM using tryCreate (mirrors JSRuntime::CreateModContext)
    RefPtr<VM> vmPtr = VM::tryCreate(HeapType::Large);
    if (!vmPtr) {
        std::cerr << "[Error] Failed to create JavaScript VM" << std::endl;
        return 1;
    }
    vmPtr->refSuppressingSaferCPPChecking();
    VM& vm = *vmPtr;

    std::cout << "[Init] Setting DoesGC expectation..." << std::endl;
    // Reset DoesGC expectation to allow GC before calling acquireAccess
    vm.setDoesGCExpectation(true, DFG::DoesGCCheck::Special::Uninitialized);

    std::cout << "[Init] Acquiring heap access..." << std::endl;
    vm.heap.acquireAccess();

    std::cout << "[Init] Taking JS lock..." << std::endl;
    JSLockHolder locker(vm);

    std::cout << "[Init] Creating global object with custom method table..." << std::endl;
    Structure* structure = JSGlobalObject::createStructure(vm, jsNull());
    JSGlobalObject* globalObject = JSGlobalObject::createWithCustomMethodTable(vm, structure, &s_customGlobalObjectMethodTable);

    JSGlobalContextRef ctx = toGlobalRef(globalObject);
    if (!ctx) {
        std::cerr << "[Error] Failed to create JavaScript context" << std::endl;
        return 1;
    }

    std::cout << "[Init] Registering console.log..." << std::endl;
    // Create console object with log function
    JSObjectRef global = JSContextGetGlobalObject(ctx);

    JSObjectRef consoleObj = JSObjectMake(ctx, nullptr, nullptr);
    JSStringRef logName = JSStringCreateWithUTF8CString("log");
    JSObjectRef logFunc = JSObjectMakeFunctionWithCallback(ctx, logName, jsConsoleLog);
    JSObjectSetProperty(ctx, consoleObj, logName, logFunc, kJSPropertyAttributeNone, nullptr);
    JSStringRelease(logName);

    JSStringRef consoleName = JSStringCreateWithUTF8CString("console");
    JSObjectSetProperty(ctx, global, consoleName, consoleObj, kJSPropertyAttributeNone, nullptr);
    JSStringRelease(consoleName);

    // Also add print as alias for console.log
    JSStringRef printName = JSStringCreateWithUTF8CString("print");
    JSObjectRef printFunc = JSObjectMakeFunctionWithCallback(ctx, printName, jsConsoleLog);
    JSObjectSetProperty(ctx, global, printName, printFunc, kJSPropertyAttributeNone, nullptr);
    JSStringRelease(printName);

    // Register timer functions (setTimeout, setInterval, clearTimeout, clearInterval)
    std::cout << "[Init] Registering timer functions..." << std::endl;

    JSStringRef setTimeoutName = JSStringCreateWithUTF8CString("setTimeout");
    JSObjectRef setTimeoutFunc = JSObjectMakeFunctionWithCallback(ctx, setTimeoutName, jsSetTimeout);
    JSObjectSetProperty(ctx, global, setTimeoutName, setTimeoutFunc, kJSPropertyAttributeNone, nullptr);
    JSStringRelease(setTimeoutName);

    JSStringRef setIntervalName = JSStringCreateWithUTF8CString("setInterval");
    JSObjectRef setIntervalFunc = JSObjectMakeFunctionWithCallback(ctx, setIntervalName, jsSetInterval);
    JSObjectSetProperty(ctx, global, setIntervalName, setIntervalFunc, kJSPropertyAttributeNone, nullptr);
    JSStringRelease(setIntervalName);

    JSStringRef clearTimeoutName = JSStringCreateWithUTF8CString("clearTimeout");
    JSObjectRef clearTimeoutFunc = JSObjectMakeFunctionWithCallback(ctx, clearTimeoutName, jsClearTimer);
    JSObjectSetProperty(ctx, global, clearTimeoutName, clearTimeoutFunc, kJSPropertyAttributeNone, nullptr);
    JSStringRelease(clearTimeoutName);

    JSStringRef clearIntervalName = JSStringCreateWithUTF8CString("clearInterval");
    JSObjectRef clearIntervalFunc = JSObjectMakeFunctionWithCallback(ctx, clearIntervalName, jsClearTimer);
    JSObjectSetProperty(ctx, global, clearIntervalName, clearIntervalFunc, kJSPropertyAttributeNone, nullptr);
    JSStringRelease(clearIntervalName);

    std::cout << "[Init] Initialization complete!\n" << std::endl;

    int exitCode = 0;

    if (isModule) {
        // Execute as ES6 module
        std::cout << "[Exec] Loading as ES6 module..." << std::endl;

        // Try to load real natives.js from same directory as the script, or use mock
        std::filesystem::path scriptDir = std::filesystem::path(scriptPath).parent_path();
        std::filesystem::path realNativesPath = scriptDir / "natives.js";

        std::string nativesModuleCode;
        if (std::filesystem::exists(realNativesPath)) {
            nativesModuleCode = readFile(realNativesPath.string());
            std::cout << "[Module] Loaded REAL natives.js from " << realNativesPath << " (" << nativesModuleCode.size() << " bytes)" << std::endl;
        } else {
            // Fallback to mock natives module
            nativesModuleCode = R"(
// Mock natives module for testing
export function getGameTimer() {
    return Date.now();
}

export function playerPedId() {
    return 1;
}

export function getEntityCoords(entity, alive) {
    return { x: 100.0, y: 200.0, z: 50.0 };
}

export function getEntityHeading(entity) {
    return 45.0;
}

export function setEntityCoords(entity, x, y, z, a, b, c, d) {
    console.log("setEntityCoords called: " + x + ", " + y + ", " + z);
}

export function setClockTime(h, m, s) {
    console.log("setClockTime called: " + h + ":" + m + ":" + s);
}

export function getClockHours() {
    return 12;
}

export function getClockMinutes() {
    return 30;
}

export function isPedInAnyVehicle(ped, flag) {
    return false;
}

export function getVehiclePedIsIn(ped, flag) {
    return 0;
}

export function setVehicleForwardSpeed(vehicle, speed) {
    console.log("setVehicleForwardSpeed called: " + speed);
}
)";
            std::cout << "[Module] Using MOCK natives module (" << nativesModuleCode.size() << " bytes)" << std::endl;
        }
        provideModule(globalObject, "natives", nativesModuleCode);

        // Also register the main module source so it can be fetched
        provideModule(globalObject, scriptPath, code);

        WTF::String sourceString = WTF::String::fromUTF8(code.c_str());
        WTF::String moduleKeyString = WTF::String::fromUTF8(scriptPath.c_str());

        // Use the module loader's loadModule to get the module record first
        JSModuleLoader* moduleLoader = globalObject->moduleLoader();

        // First, load the module (returns promise that resolves to module record)
        std::cout << "[Debug] Step 1: Loading module..." << std::endl;
        JSInternalPromise* loadPromise = moduleLoader->loadModule(globalObject, jsString(vm, moduleKeyString), jsUndefined(), jsUndefined());
        vm.drainMicrotasks();

        if (!loadPromise || loadPromise->status() == JSPromise::Status::Rejected) {
            JSValue reason = loadPromise ? loadPromise->result() : jsUndefined();
            std::cerr << "[Error] Module load failed: " << formatException(ctx, toRef(globalObject, reason)) << std::endl;
            exitCode = 1;
        } else if (loadPromise->status() == JSPromise::Status::Pending) {
            std::cerr << "[Error] Module load did not complete (promise still pending)" << std::endl;
            exitCode = 1;
        } else {
            // loadModule returns the module record
            JSValue moduleRecordValue = loadPromise->result();
            std::cout << "[Debug] loadModule result - isObject=" << moduleRecordValue.isObject()
                      << " isUndefined=" << moduleRecordValue.isUndefined()
                      << " isCell=" << moduleRecordValue.isCell()
                      << " isString=" << moduleRecordValue.isString()
                      << " isSymbol=" << moduleRecordValue.isSymbol() << std::endl;

            // If it's a cell, try to get more info
            if (moduleRecordValue.isCell()) {
                JSCell* cell = moduleRecordValue.asCell();
                std::cout << "[Debug] Cell type: " << cell->type() << std::endl;

                // Try casting to JSObject anyway (cells that are objects)
                if (cell->isObject()) {
                    JSObject* obj = static_cast<JSObject*>(cell);
                    std::cout << "[Debug] Cell is object, class: " << obj->className() << std::endl;
                }
            }

            if (moduleRecordValue.isObject()) {
                JSObject* moduleRecordObj = moduleRecordValue.getObject();
                std::cout << "[Debug] Module record class: " << moduleRecordObj->className() << std::endl;
            }

            // Now link and evaluate the module
            std::cout << "[Debug] Step 2: Linking and evaluating module..." << std::endl;
            JSValue evalResult = moduleLoader->linkAndEvaluateModule(globalObject, jsString(vm, moduleKeyString), jsUndefined());
            vm.drainMicrotasks();

            std::cout << "[Debug] linkAndEvaluateModule result - isObject=" << evalResult.isObject()
                      << " isUndefined=" << evalResult.isUndefined() << std::endl;

            std::cout << "[Success] Module loaded and evaluated successfully" << std::endl;

            // Now try to get the namespace from the stored module record
            std::cout << "[Debug] Step 3: Getting module namespace from stored module record..." << std::endl;
            JSObjectRef ns = nullptr;

            // Check if we have a stored module record from the evaluate callback
            JSValue storedModuleRecord;
            {
                std::lock_guard<std::mutex> lock(g_lastModuleRecordMutex);
                auto it = g_lastModuleRecord.find(globalObject);
                if (it != g_lastModuleRecord.end()) {
                    storedModuleRecord = it->second;
                }
            }

            if (storedModuleRecord && !storedModuleRecord.isUndefined()) {
                std::cout << "[Debug] Found stored module record, getting namespace..." << std::endl;
                std::cout << "[Debug] Module record - isObject=" << storedModuleRecord.isObject()
                          << " isCell=" << storedModuleRecord.isCell() << std::endl;

                JSModuleNamespaceObject* namespaceObj = moduleLoader->getModuleNamespaceObject(globalObject, storedModuleRecord);
                if (namespaceObj) {
                    std::cout << "[Debug] Got namespace object from getModuleNamespaceObject!" << std::endl;
                    ns = toRef(namespaceObj);
                } else {
                    std::cout << "[Debug] getModuleNamespaceObject returned null" << std::endl;
                }
            } else {
                std::cout << "[Debug] No stored module record found" << std::endl;
            }

            // Try to call exported functions
            if (ns) {
                std::cout << "[Debug] Got namespace object, checking for exports..." << std::endl;

                // List all properties on the namespace
                JSPropertyNameArrayRef propNames = JSObjectCopyPropertyNames(ctx, ns);
                size_t propCount = JSPropertyNameArrayGetCount(propNames);
                std::cout << "[Debug] Namespace has " << propCount << " properties:" << std::endl;
                for (size_t i = 0; i < propCount && i < 20; i++) {
                    JSStringRef propName = JSPropertyNameArrayGetNameAtIndex(propNames, i);
                    size_t maxSize = JSStringGetMaximumUTF8CStringSize(propName);
                    std::vector<char> buffer(maxSize);
                    JSStringGetUTF8CString(propName, buffer.data(), maxSize);
                    std::cout << "[Debug]   - " << buffer.data() << std::endl;
                }
                JSPropertyNameArrayRelease(propNames);

                // Try calling init
                JSStringRef initName = JSStringCreateWithUTF8CString("init");
                JSValueRef initVal = JSObjectGetProperty(ctx, ns, initName, nullptr);
                JSStringRelease(initName);

                if (initVal && JSValueIsObject(ctx, initVal)) {
                    JSObjectRef initFunc = JSValueToObject(ctx, initVal, nullptr);
                    if (initFunc && JSObjectIsFunction(ctx, initFunc)) {
                        std::cout << "\n[Test] Calling exported init()..." << std::endl;
                        JSValueRef exception = nullptr;
                        JSObjectCallAsFunction(ctx, initFunc, nullptr, 0, nullptr, &exception);
                        if (exception) {
                            std::cerr << "[Error] init() failed: " << formatException(ctx, exception) << std::endl;
                        } else {
                            std::cout << "[Success] init() called successfully!" << std::endl;
                        }
                    }
                } else {
                    std::cout << "[Debug] init is not a function or not found" << std::endl;
                }

                // Try calling tick
                JSStringRef tickName = JSStringCreateWithUTF8CString("tick");
                JSValueRef tickVal = JSObjectGetProperty(ctx, ns, tickName, nullptr);
                JSStringRelease(tickName);

                if (tickVal && JSValueIsObject(ctx, tickVal)) {
                    JSObjectRef tickFunc = JSValueToObject(ctx, tickVal, nullptr);
                    if (tickFunc && JSObjectIsFunction(ctx, tickFunc)) {
                        std::cout << "\n[Test] Calling exported tick()..." << std::endl;
                        JSValueRef exception = nullptr;
                        JSObjectCallAsFunction(ctx, tickFunc, nullptr, 0, nullptr, &exception);
                        if (exception) {
                            std::cerr << "[Error] tick() failed: " << formatException(ctx, exception) << std::endl;
                        } else {
                            std::cout << "[Success] tick() called successfully!" << std::endl;
                        }
                    }
                } else {
                    std::cout << "[Debug] tick is not a function or not found" << std::endl;
                }
            } else {
                std::cout << "[Warning] Could not get module namespace object" << std::endl;
            }
        }
    } else {
        // Execute as regular script
        std::cout << "[Exec] Evaluating script..." << std::endl;

        JSStringRef script = JSStringCreateWithUTF8CString(code.c_str());
        JSStringRef sourceURL = JSStringCreateWithUTF8CString(scriptPath.c_str());

        JSValueRef exception = nullptr;
        JSValueRef result = JSEvaluateScript(ctx, script, nullptr, sourceURL, 1, &exception);

        JSStringRelease(script);
        JSStringRelease(sourceURL);

        if (exception) {
            std::cerr << "[Error] Script error: " << formatException(ctx, exception) << std::endl;
            exitCode = 1;
        } else {
            std::cout << "[Success] Script executed successfully" << std::endl;
            if (result && !JSValueIsUndefined(ctx, result)) {
                std::cout << "[Result] " << formatException(ctx, result) << std::endl;
            }
        }
    }

    // Drain any remaining microtasks
    vm.drainMicrotasks();

    // Run event loop to process timers (for Promise support with setTimeout)
    {
        std::lock_guard<std::mutex> lock(g_timersMutex);
        if (!g_timers.empty()) {
            std::cout << "\n[EventLoop] Processing " << g_timers.size() << " pending timer(s)..." << std::endl;
        }
    }

    int loopIterations = 0;
    const int maxIterations = 10000;  // Safety limit
    while (processTimers(vm) && loopIterations < maxIterations) {
        loopIterations++;
        // Small sleep to prevent busy-waiting
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }

    if (loopIterations > 0) {
        std::cout << "[EventLoop] Completed after " << loopIterations << " iteration(s)" << std::endl;
    }

    // Final drain
    vm.drainMicrotasks();

    // Cleanup module sources
    {
        std::lock_guard<std::mutex> lock(g_contextModuleSourcesMutex);
        g_contextModuleSources.erase(globalObject);
    }

    std::cout << "\n[Done] Exiting with code " << exitCode << std::endl;
    return exitCode;
}
