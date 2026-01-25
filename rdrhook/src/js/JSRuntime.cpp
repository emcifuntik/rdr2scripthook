#include "stdafx.h"
#include "JSRuntime.h"
#include "JSNativeBindings.h"
#include "Logger.h"

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
#include <JavaScriptCore/JSModuleRecord.h>
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

namespace rdr2js {

// Global runtime instance
static std::unique_ptr<JSRuntime> g_runtime;

// Static map: JSGlobalObject* -> module sources for that context
// This is needed for cleanup when context is destroyed
static std::unordered_map<JSC::JSGlobalObject*, std::unordered_map<std::string, std::string>> g_contextModuleSources;
static std::mutex g_contextModuleSourcesMutex;

// Store the last evaluated module record for namespace retrieval
static std::unordered_map<JSC::JSGlobalObject*, JSC::JSValue> g_lastModuleRecord;
static std::mutex g_lastModuleRecordMutex;

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
// For bare specifiers like 'natives', we just return them as-is
static JSC::Identifier customModuleLoaderResolve(
    JSC::JSGlobalObject* globalObject,
    JSC::JSModuleLoader*,
    JSC::JSValue keyValue,
    JSC::JSValue referrerValue,
    JSC::JSValue)
{
    JSC::VM& vm = globalObject->vm();

    // Handle different key types
    if (keyValue.isSymbol()) {
        // For symbols, return the symbol as an identifier directly
        JSC::Symbol* symbol = JSC::asSymbol(keyValue);
        const auto& privateName = symbol->privateName();
        spdlog::debug("[JSRuntime] moduleLoaderResolve: symbol key");
        return JSC::Identifier::fromUid(privateName);
    }

    WTF::String key;
    if (keyValue.isString()) {
        key = keyValue.toWTFString(globalObject);
    } else {
        // For other types, try to get string representation
        // but this might fail for symbols, so we handle that above
        spdlog::debug("[JSRuntime] moduleLoaderResolve: unknown key type, isObject={}", keyValue.isObject());
        return JSC::Identifier();
    }

    WTF::String referrer;
    if (referrerValue.isString()) {
        referrer = referrerValue.toWTFString(globalObject);
    } else if (referrerValue.isSymbol()) {
        // For symbols, we can't easily get a string representation
        referrer = "<symbol>"_s;
    }

    spdlog::debug("[JSRuntime] moduleLoaderResolve: key='{}' referrer='{}'",
        key.utf8().data(), referrer.utf8().data());

    if (key.isEmpty()) {
        spdlog::error("[JSRuntime] moduleLoaderResolve: empty key!");
        return JSC::Identifier();
    }

    return JSC::Identifier::fromString(vm, key);
}

// Fetch a module's source code
// This looks up the module in our provided modules map
static JSC::JSInternalPromise* customModuleLoaderFetch(
    JSC::JSGlobalObject* globalObject,
    JSC::JSModuleLoader*,
    JSC::JSValue keyValue,
    JSC::JSValue parametersValue,
    JSC::JSValue scriptFetcher)
{
    JSC::VM& vm = globalObject->vm();

    WTF::String key;
    if (keyValue.isSymbol()) {
        // Symbol keys are used by JSC for internal module tracking
        // We shouldn't try to fetch these - the main module source is already
        // provided inline via loadAndEvaluateModule, so reject symbol fetches
        spdlog::debug("[JSRuntime] moduleLoaderFetch: symbol key - rejecting (main module already provided inline)");
        auto* promise = JSC::JSInternalPromise::create(vm, globalObject->internalPromiseStructure());
        promise->reject(vm, globalObject, JSC::createError(globalObject, "Cannot fetch module by symbol key"_s));
        return promise;
    } else if (keyValue.isString()) {
        key = keyValue.toWTFString(globalObject);
        spdlog::debug("[JSRuntime] moduleLoaderFetch: string key='{}'", key.utf8().data());
    } else {
        spdlog::error("[JSRuntime] moduleLoaderFetch: unknown key type isObject={}", keyValue.isObject());
        auto* promise = JSC::JSInternalPromise::create(vm, globalObject->internalPromiseStructure());
        promise->reject(vm, globalObject, JSC::createError(globalObject, "Invalid module key type"_s));
        return promise;
    }

    // Look up the module source in our map
    std::string moduleKey = key.utf8().data();
    std::string moduleSource;

    // Handle built-in 'natives' module
    // Provides 7000+ game native functions via ES6 import syntax
    if (moduleKey == "natives") {
        spdlog::debug("[JSRuntime] Providing built-in 'natives' module");
        // The __natives__ object contains all native functions (internal, not a public global)
        // This synthetic module exports it for clean ES6 imports
        // Usage: import natives from 'natives'; natives.getGameTimer();
        moduleSource = R"(
// Built-in natives module - all 7000+ game native functions
export default __natives__;
)";
    }
    // Handle built-in 'core' module - provides callback registration API
    else if (moduleKey == "core") {
        spdlog::debug("[JSRuntime] Providing built-in 'core' module");
        // The __core__ object provides dynamic callback registration (internal, not a public global)
        // This synthetic module re-exports its functions for clean ES6 named imports
        moduleSource = R"(
// Built-in core module - callback registration API
const _core = __core__;

// Tick callbacks
export const addTickCallback = _core.addTickCallback.bind(_core);
export const removeTickCallback = _core.removeTickCallback.bind(_core);

// Key callbacks
export const addKeyDownCallback = _core.addKeyDownCallback.bind(_core);
export const removeKeyDownCallback = _core.removeKeyDownCallback.bind(_core);
export const addKeyUpCallback = _core.addKeyUpCallback.bind(_core);
export const removeKeyUpCallback = _core.removeKeyUpCallback.bind(_core);

// Utilities
export const getGameTime = _core.getGameTime.bind(_core);

// Default export
export default _core;
)";
    }

    {
        std::lock_guard<std::mutex> lock(g_contextModuleSourcesMutex);
        auto contextIt = g_contextModuleSources.find(globalObject);
        if (contextIt != g_contextModuleSources.end()) {
            // Log available modules
            spdlog::debug("[JSRuntime] Available modules for this context:");
            for (const auto& [name, src] : contextIt->second) {
                spdlog::debug("[JSRuntime]   - '{}' ({} bytes)", name, src.size());
            }

            auto moduleIt = contextIt->second.find(moduleKey);
            if (moduleIt != contextIt->second.end()) {
                moduleSource = moduleIt->second;
                spdlog::debug("[JSRuntime] Found module '{}' ({} bytes)", moduleKey, moduleSource.size());
            } else if (moduleSource.empty()) {
                spdlog::debug("[JSRuntime] Module '{}' NOT found in map", moduleKey);
            }
        } else if (moduleSource.empty()) {
            spdlog::error("[JSRuntime] No modules registered for this context!");
        }
    }

    if (moduleSource.empty()) {
        spdlog::error("[JSRuntime] Module not found: {}", moduleKey);
        auto* promise = JSC::JSInternalPromise::create(vm, globalObject->internalPromiseStructure());
        WTF::String errorMsg = WTF::makeString("Module not found: "_s, key);
        promise->reject(vm, globalObject, JSC::createError(globalObject, errorMsg));
        return promise;
    }

    // Create source code for the module
    // Note: Use std::span for proper handling of large buffers
    WTF::String sourceString = WTF::String::fromUTF8(std::span<const char>(moduleSource.data(), moduleSource.size()));
    if (sourceString.isNull()) {
        spdlog::error("[JSRuntime] Failed to create WTF::String from module source (size={})", moduleSource.size());
        auto* promise = JSC::JSInternalPromise::create(vm, globalObject->internalPromiseStructure());
        promise->reject(vm, globalObject, JSC::createError(globalObject, "Failed to convert module source to string"_s));
        return promise;
    }

    spdlog::debug("[JSRuntime] Created sourceString for '{}' (length={})", moduleKey, sourceString.length());

    // Log first 100 chars of source for debugging
    std::string sourcePreview = moduleSource.substr(0, std::min<size_t>(100, moduleSource.size()));
    // Replace newlines for logging
    for (char& c : sourcePreview) {
        if (c == '\n' || c == '\r') c = ' ';
    }
    spdlog::debug("[JSRuntime] Source preview: {}", sourcePreview);

    // Create a proper SourceOrigin for the module
    // Use a file:// URL scheme to ensure JSC recognizes it as a valid source URL
    WTF::String moduleURLString = WTF::makeString("file:///builtin/"_s, key, ".js"_s);
    WTF::URL moduleURL = WTF::URL(moduleURLString);
    JSC::SourceOrigin sourceOrigin(moduleURL);
    spdlog::debug("[JSRuntime] Module URL: {} (valid={})", moduleURL.string().utf8().data(), moduleURL.isValid());

    // Use the full URL as the filename for error reporting
    JSC::SourceCode sourceCode = JSC::makeSource(
        sourceString,
        sourceOrigin,
        JSC::SourceTaintedOrigin::Untainted,
        moduleURLString,  // Use full URL as filename
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
    spdlog::debug("[JSRuntime] moduleLoaderEvaluate: storing module record");

    // Store the module record for later namespace retrieval
    {
        std::lock_guard<std::mutex> lock(g_lastModuleRecordMutex);
        g_lastModuleRecord[globalObject] = moduleRecord;
    }

    // Call the default evaluator
    return moduleLoader->evaluateNonVirtual(globalObject, key, moduleRecord, scriptFetcher, sentValue, resumeMode);
}

// Custom GlobalObjectMethodTable with our module loader callbacks
// Required callbacks (cannot be nullptr): supportsRichSourceInfo, shouldInterruptScript,
// javaScriptRuntimeFlags, shouldInterruptScriptBeforeTimeout, trustedScriptStructure
static const JSC::GlobalObjectMethodTable s_customGlobalObjectMethodTable = {
    &customSupportsRichSourceInfo,
    &customShouldInterruptScript,
    &customJavaScriptRuntimeFlags,
    nullptr, // queueMicrotaskToEventLoop
    &customShouldInterruptScriptBeforeTimeout,
    nullptr, // moduleLoaderImportModule
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

JSRuntime& GetJSRuntime() {
    if (!g_runtime) {
        g_runtime = std::make_unique<JSRuntime>();
    }
    return *g_runtime;
}

// Helper to convert std::string to JSStringRef
class JSStringWrapper {
public:
    explicit JSStringWrapper(const std::string& str)
        : m_string(JSStringCreateWithUTF8CString(str.c_str())) {}

    ~JSStringWrapper() {
        if (m_string) {
            JSStringRelease(m_string);
        }
    }

    JSStringRef get() const { return m_string; }
    operator JSStringRef() const { return m_string; }

private:
    JSStringRef m_string;
};

// Helper to convert JSStringRef to std::string
std::string JSStringToStdString(JSStringRef jsString) {
    if (!jsString) return "";

    size_t maxSize = JSStringGetMaximumUTF8CStringSize(jsString);
    std::vector<char> buffer(maxSize);
    JSStringGetUTF8CString(jsString, buffer.data(), maxSize);
    return std::string(buffer.data());
}

// ============================================================================
// JSRuntime implementation
// ============================================================================

JSRuntime::JSRuntime() = default;

JSRuntime::~JSRuntime() {
    Shutdown();
}

bool JSRuntime::Initialize(GetNativeAddressFunc getNativeAddr, GetGlobalPointerFunc getGlobalPtr) {
    if (m_initialized) {
        return true;
    }

    m_getNativeAddr = getNativeAddr;
    m_getGlobalPtr = getGlobalPtr;

    if (!m_getNativeAddr || !m_getGlobalPtr) {
        m_lastError = "Invalid function pointers provided";
        return false;
    }

    WTF::setDataFile("D:/jsc.log");

    // Enable restricted options first - must be called before any other JSC API
    // This is required for proper JSC configuration
    JSC::Config::enableRestrictedOptions();
    spdlog::debug("[JSRuntime] JSC restricted options enabled");

    // Initialize main thread - required for proper GC behavior
    // This must be called before JSC::initialize()
    WTF::initializeMainThread();
    spdlog::debug("[JSRuntime] Main thread initialized");

    // Initialize JSC threading and other subsystems
    JSC::initialize();
    spdlog::debug("[JSRuntime] JSC initialized");

    // Configure JSC options (must be done after initialize but before creating VMs)
    {
        JSC::Options::AllowUnfinalizedAccessScope scope;

        JSC::Options::useConcurrentJIT() = true;
        // JSC::Options::useSigillCrashAnalyzer() = true;
        JSC::Options::useWasm() = true;
        JSC::Options::useSourceProviderCache() = true;
        // JSC::Options::useUnlinkedCodeBlockJettisoning() = false;
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
        spdlog::debug("[JSRuntime] JSC options configured");
    }

    m_initialized = true;
    spdlog::info("[JSRuntime] JavaScript runtime initialized");
    return true;
}

void JSRuntime::Shutdown() {
    // Clean up all context module sources
    {
        std::lock_guard<std::mutex> lock(g_contextModuleSourcesMutex);
        g_contextModuleSources.clear();
    }

    m_initialized = false;
    m_getNativeAddr = nullptr;
    m_getGlobalPtr = nullptr;
    spdlog::info("[JSRuntime] JavaScript runtime shutdown");
}

std::unique_ptr<JSMod> JSRuntime::CreateModContext(const std::string& modName) {
    if (!m_initialized) {
        m_lastError = "Runtime not initialized";
        return nullptr;
    }

    using namespace JSC;

    // Create VM using C++ API (like Bun does)
    // Use tryCreate for better error handling
    RefPtr<VM> vmPtr = VM::tryCreate(HeapType::Large);
    if (!vmPtr) {
        m_lastError = "Failed to create JavaScript VM";
        spdlog::error("[JSRuntime] {}", m_lastError);
        return nullptr;
    }
    // Add extra reference to prevent destruction (matches Bun's pattern)
    vmPtr->refSuppressingSaferCPPChecking();
    VM& vm = *vmPtr;

    // Reset DoesGC expectation to allow GC before calling acquireAccess
    // This prevents DoesGC validation failure in debug builds
    vm.setDoesGCExpectation(true, DFG::DoesGCCheck::Special::Uninitialized);

    // Acquire heap access BEFORE taking the lock (crucial for proper GC state)
    vm.heap.acquireAccess();

    // Now take the lock
    JSLockHolder locker(vm);

    // Create the global object with custom module loader callbacks
    Structure* structure = JSGlobalObject::createStructure(vm, jsNull());
    JSGlobalObject* globalObject = JSGlobalObject::createWithCustomMethodTable(vm, structure, &s_customGlobalObjectMethodTable);

    // Convert to C API reference
    JSGlobalContextRef ctx = toGlobalRef(globalObject);
    if (!ctx) {
        m_lastError = "Failed to create JavaScript context";
        spdlog::error("[JSRuntime] {}", m_lastError);
        return nullptr;
    }

    // Register native bindings
    RegisterNativeBindings(ctx);

    auto mod = std::make_unique<JSMod>(ctx, modName, this);
    spdlog::debug("[JSRuntime] Created context for mod: {}", modName);
    return mod;
}

bool JSRuntime::ExecuteScript(JSContextRef ctx, const std::string& code, const std::string& sourceURL) {
    JSStringWrapper script(code);
    JSStringWrapper source(sourceURL.empty() ? "script" : sourceURL);

    JSValueRef exception = nullptr;
    JSEvaluateScript(ctx, script, nullptr, source, 1, &exception);

    if (exception) {
        m_lastError = "Script error: " + FormatException(ctx, exception);
        spdlog::error("[JSRuntime] {}", m_lastError);
        return false;
    }

    return true;
}

JSValueRef JSRuntime::CallFunction(JSContextRef ctx, const std::string& functionName,
    const std::vector<JSValueRef>& args) {

    JSObjectRef globalObj = JSContextGetGlobalObject(ctx);
    JSStringWrapper funcName(functionName);

    JSValueRef funcVal = JSObjectGetProperty(ctx, globalObj, funcName, nullptr);
    if (!funcVal || !JSValueIsObject(ctx, funcVal)) {
        return nullptr;
    }

    JSObjectRef funcObj = JSValueToObject(ctx, funcVal, nullptr);
    if (!funcObj || !JSObjectIsFunction(ctx, funcObj)) {
        return nullptr;
    }

    JSValueRef exception = nullptr;
    JSValueRef result = JSObjectCallAsFunction(ctx, funcObj, nullptr,
        args.size(), args.empty() ? nullptr : args.data(), &exception);

    if (exception) {
        m_lastError = "Function call error: " + FormatException(ctx, exception);
        spdlog::error("[JSRuntime] {}", m_lastError);
        return nullptr;
    }

    return result;
}

bool JSRuntime::HasFunction(JSContextRef ctx, const std::string& functionName) {
    JSObjectRef globalObj = JSContextGetGlobalObject(ctx);
    JSStringWrapper funcName(functionName);

    JSValueRef funcVal = JSObjectGetProperty(ctx, globalObj, funcName, nullptr);
    if (!funcVal || JSValueIsUndefined(ctx, funcVal)) {
        return false;
    }

    if (!JSValueIsObject(ctx, funcVal)) {
        return false;
    }

    JSObjectRef funcObj = JSValueToObject(ctx, funcVal, nullptr);
    return funcObj && JSObjectIsFunction(ctx, funcObj);
}

void JSRuntime::RegisterNativeBindings(JSGlobalContextRef ctx) {
    RegisterAllBindings(ctx, this);
}

void JSRuntime::RegisterModuleSource(const std::string& moduleKey, const std::string& code) {
    m_moduleSources[moduleKey] = code;
    spdlog::debug("[JSRuntime] Registered module source: {}", moduleKey);
}

void JSRuntime::ProvideModule(JSContextRef ctx, const std::string& moduleKey, const std::string& code) {
    // Store module source in the static map for this context
    JSC::JSGlobalObject* globalObject = toJS(ctx);

    {
        std::lock_guard<std::mutex> lock(g_contextModuleSourcesMutex);
        g_contextModuleSources[globalObject][moduleKey] = code;
    }

    spdlog::debug("[JSRuntime] Provided module '{}' for context ({} bytes)", moduleKey, code.size());
}

const std::string* JSRuntime::GetModuleSource(const std::string& moduleKey) const {
    auto it = m_moduleSources.find(moduleKey);
    if (it != m_moduleSources.end()) {
        return &it->second;
    }
    return nullptr;
}

void JSRuntime::DrainMicrotasks(JSContextRef ctx) {
    JSC::JSGlobalObject* globalObject = toJS(ctx);
    JSC::VM& vm = globalObject->vm();
    JSC::JSLockHolder locker(vm);
    vm.drainMicrotasks();
}

JSObjectRef JSRuntime::ExecuteModule(JSContextRef ctx, const std::string& code, const std::string& moduleKey) {
    using namespace JSC;

    JSGlobalObject* globalObject = toJS(ctx);
    VM& vm = globalObject->vm();

    // Must hold the API lock for all JSC operations
    JSLockHolder locker(vm);

    // Normalize path separators for Windows (replace forward slashes with backslashes)
    std::string normalizedKey = moduleKey;
    std::replace(normalizedKey.begin(), normalizedKey.end(), '/', '\\');

    // Register main module in our map (for debugging/reference, though JSC has it inline)
    {
        std::lock_guard<std::mutex> lock(g_contextModuleSourcesMutex);
        g_contextModuleSources[globalObject][normalizedKey] = code;
    }
    spdlog::debug("[JSRuntime] ExecuteModule: registered main module '{}' ({} bytes)", normalizedKey, code.size());

    // Convert main module strings to WTF::String
    WTF::String sourceString = WTF::String::fromUTF8(code.c_str());
    WTF::String moduleKeyString = WTF::String::fromUTF8(normalizedKey.c_str());

    // Create SourceOrigin for the module
    SourceOrigin sourceOrigin(WTF::URL::fileURLWithFileSystemPath(moduleKeyString));
    spdlog::debug("[JSRuntime] Main module SourceOrigin URL: {}", sourceOrigin.url().string().utf8().data());

    // Create SourceCode as a module
    SourceCode sourceCode = makeSource(
        sourceString,
        sourceOrigin,
        SourceTaintedOrigin::Untainted,
        moduleKeyString,
        WTF::TextPosition(),
        SourceProviderSourceType::Module
    );

    // Load and evaluate the module - returns a promise
    JSInternalPromise* promise = loadAndEvaluateModule(globalObject, sourceCode, jsUndefined());

    // Drain microtasks to allow the promise to settle
    vm.drainMicrotasks();

    // Check promise state
    if (promise) {
        JSPromise::Status status = promise->status();
        if (status == JSPromise::Status::Rejected) {
            JSValue reason = promise->result();
            m_lastError = "Module rejected: " + FormatException(ctx, toRef(globalObject, reason));
            spdlog::error("[JSRuntime] {}", m_lastError);
            return nullptr;
        }
        if (status == JSPromise::Status::Pending) {
            m_lastError = "Module evaluation did not complete (promise still pending)";
            spdlog::error("[JSRuntime] {}", m_lastError);
            return nullptr;
        }
        // Promise fulfilled - the result is the module namespace object
        JSValue namespaceValue = promise->result();
        spdlog::debug("[JSRuntime] Promise result - isObject={} isUndefined={} isNull={} isCell={}",
            namespaceValue.isObject(), namespaceValue.isUndefined(),
            namespaceValue.isNull(), namespaceValue.isCell());

        if (namespaceValue.isObject()) {
            spdlog::debug("[JSRuntime] Module loaded successfully: {}", moduleKey);
            return toRef(namespaceValue.getObject());
        }

        // Module executed but loadAndEvaluateModule returns undefined in this JSC version
        // Get the namespace using the stored module record from the evaluate callback
        if (namespaceValue.isUndefined()) {
            spdlog::debug("[JSRuntime] loadAndEvaluateModule returned undefined, getting namespace from stored module record...");

            // Check if we have a stored module record from the evaluate callback
            JSValue storedModuleRecord;
            {
                std::lock_guard<std::mutex> lock(g_lastModuleRecordMutex);
                auto it = g_lastModuleRecord.find(globalObject);
                if (it != g_lastModuleRecord.end()) {
                    storedModuleRecord = it->second;
                }
            }

            if (storedModuleRecord && !storedModuleRecord.isUndefined() && storedModuleRecord.isObject()) {
                spdlog::debug("[JSRuntime] Found stored module record, getting namespace...");

                JSModuleLoader* moduleLoader = globalObject->moduleLoader();
                if (moduleLoader) {
                    JSModuleNamespaceObject* ns = moduleLoader->getModuleNamespaceObject(globalObject, storedModuleRecord);
                    if (ns) {
                        spdlog::debug("[JSRuntime] Module loaded successfully with namespace: {}", moduleKey);
                        return toRef(ns);
                    }
                    spdlog::debug("[JSRuntime] getModuleNamespaceObject returned null");
                }
            } else {
                spdlog::debug("[JSRuntime] No stored module record found");
            }

            // Fallback: return empty object
            spdlog::debug("[JSRuntime] Module executed successfully (no namespace available): {}", moduleKey);
            JSObjectRef emptyObj = JSObjectMake(ctx, nullptr, nullptr);
            return emptyObj;
        }
    }

    m_lastError = "Module evaluation failed: no namespace returned";
    spdlog::error("[JSRuntime] {}", m_lastError);
    return nullptr;
}

void JSRuntime::CleanupContext(JSContextRef ctx) {
    // Remove module sources for this context
    JSC::JSGlobalObject* globalObject = toJS(ctx);

    {
        std::lock_guard<std::mutex> lock(g_contextModuleSourcesMutex);
        g_contextModuleSources.erase(globalObject);
    }

    // Remove stored module record for this context
    {
        std::lock_guard<std::mutex> lock(g_lastModuleRecordMutex);
        g_lastModuleRecord.erase(globalObject);
    }

    spdlog::debug("[JSRuntime] Cleaned up module sources for context");
}

std::string JSRuntime::JSValueToString(JSContextRef ctx, JSValueRef value) {
    if (!value) return "(null)";

    JSValueRef exception = nullptr;
    JSStringRef jsStr = JSValueToStringCopy(ctx, value, &exception);
    if (!jsStr) return "(error converting to string)";

    std::string result = JSStringToStdString(jsStr);
    JSStringRelease(jsStr);
    return result;
}

// Helper to get a numeric property from a JS object
static int GetIntProperty(JSContextRef ctx, JSObjectRef obj, const char* propName) {
    JSStringWrapper prop(propName);
    JSValueRef val = JSObjectGetProperty(ctx, obj, prop, nullptr);
    if (val && JSValueIsNumber(ctx, val)) {
        return static_cast<int>(JSValueToNumber(ctx, val, nullptr));
    }
    return -1;
}

// Helper to get a string property from a JS object
static std::string GetStringProperty(JSContextRef ctx, JSObjectRef obj, const char* propName) {
    JSStringWrapper prop(propName);
    JSValueRef val = JSObjectGetProperty(ctx, obj, prop, nullptr);
    if (val && !JSValueIsUndefined(ctx, val) && !JSValueIsNull(ctx, val)) {
        JSStringRef str = JSValueToStringCopy(ctx, val, nullptr);
        if (str) {
            std::string result = JSStringToStdString(str);
            JSStringRelease(str);
            return result;
        }
    }
    return "";
}

std::string JSRuntime::FormatException(JSContextRef ctx, JSValueRef exception) {
    if (!exception) return "(null exception)";

    // Try to extract detailed info if it's an Error object
    if (JSValueIsObject(ctx, exception)) {
        JSObjectRef errObj = JSValueToObject(ctx, exception, nullptr);
        if (errObj) {
            std::string message = GetStringProperty(ctx, errObj, "message");
            std::string name = GetStringProperty(ctx, errObj, "name");
            int line = GetIntProperty(ctx, errObj, "line");
            int column = GetIntProperty(ctx, errObj, "column");
            std::string sourceURL = GetStringProperty(ctx, errObj, "sourceURL");
            std::string stack = GetStringProperty(ctx, errObj, "stack");

            std::ostringstream oss;

            // Error type and message
            if (!name.empty()) {
                oss << name << ": ";
            }
            if (!message.empty()) {
                oss << message;
            } else {
                oss << JSValueToString(ctx, exception);
            }

            // Location info
            if (line > 0) {
                oss << "\n  at ";
                if (!sourceURL.empty()) {
                    oss << sourceURL;
                } else {
                    oss << "<script>";
                }
                oss << ":" << line;
                if (column > 0) {
                    oss << ":" << column;
                }
            }

            // Stack trace (if available and different from what we already have)
            if (!stack.empty() && stack.find('\n') != std::string::npos) {
                oss << "\nStack trace:\n" << stack;
            }

            return oss.str();
        }
    }

    // Fallback to simple string conversion
    return JSValueToString(ctx, exception);
}

// ============================================================================
// JSMod implementation
// ============================================================================

JSMod::JSMod(JSGlobalContextRef ctx, const std::string& name, JSRuntime* runtime)
    : m_context(ctx), m_name(name), m_runtime(runtime) {
}

JSMod::~JSMod() {
    if (m_context) {
        // Get VM and acquire lock - all JSC operations require the API lock
        JSC::JSGlobalObject* globalObject = toJS(m_context);
        JSC::VM& vm = globalObject->vm();
        JSC::JSLockHolder locker(vm);

        // Unprotect the module namespace before releasing context
        if (m_moduleNamespace) {
            JSValueUnprotect(m_context, m_moduleNamespace);
            m_moduleNamespace = nullptr;
        }

        // Clean up context-specific module sources
        m_runtime->CleanupContext(m_context);

        // Drain any pending microtasks before releasing the context
        // This is important when using module loading APIs
        vm.drainMicrotasks();

        JSGlobalContextRelease(m_context);
        m_context = nullptr;
    }
}

bool JSMod::LoadScript(const std::string& code, const std::string& sourceURL) {
    std::string url = sourceURL.empty() ? m_name + ".js" : sourceURL;
    return m_runtime->ExecuteScript(m_context, code, url);
}

bool JSMod::LoadModule(const std::string& code, const std::string& moduleKey) {
    std::string key = moduleKey.empty() ? m_name + ".js" : moduleKey;
    JSObjectRef ns = m_runtime->ExecuteModule(m_context, code, key);
    if (ns) {
        m_moduleNamespace = ns;
        JSValueProtect(m_context, m_moduleNamespace);  // Prevent GC
        return true;
    }
    return false;
}

JSValueRef JSMod::CallExportedFunction(const std::string& name, const std::vector<JSValueRef>& args) {
    if (!m_moduleNamespace) {
        return nullptr;
    }

    JSStringRef funcName = JSStringCreateWithUTF8CString(name.c_str());
    JSValueRef funcVal = JSObjectGetProperty(m_context, m_moduleNamespace, funcName, nullptr);
    JSStringRelease(funcName);

    if (!funcVal || !JSValueIsObject(m_context, funcVal)) {
        return nullptr;
    }

    JSObjectRef funcObj = JSValueToObject(m_context, funcVal, nullptr);
    if (!funcObj || !JSObjectIsFunction(m_context, funcObj)) {
        return nullptr;
    }

    JSValueRef exception = nullptr;
    JSValueRef result = JSObjectCallAsFunction(m_context, funcObj, nullptr,
        args.size(), args.empty() ? nullptr : args.data(), &exception);

    if (exception) {
        spdlog::error("[JSMod] Error calling {}: {}", name, m_runtime->GetLastError());
        return nullptr;
    }

    return result;
}

bool JSMod::CallInit() {
    if (!HasCallback("init")) {
        return true; // Not having init is OK
    }

    CallExportedFunction("init");
    return true;
}

bool JSMod::CallTick() {
    if (!HasCallback("tick")) {
        return true;
    }

    CallExportedFunction("tick");
    return true;
}

bool JSMod::CallOnKeyDown(uint32_t key) {
    if (!HasCallback("onKeyDown")) {
        return true;
    }

    JSValueRef keyVal = JSValueMakeNumber(m_context, static_cast<double>(key));
    CallExportedFunction("onKeyDown", { keyVal });
    return true;
}

bool JSMod::CallOnKeyUp(uint32_t key) {
    if (!HasCallback("onKeyUp")) {
        return true;
    }

    JSValueRef keyVal = JSValueMakeNumber(m_context, static_cast<double>(key));
    CallExportedFunction("onKeyUp", { keyVal });
    return true;
}

bool JSMod::HasCallback(const std::string& name) {
    if (!m_moduleNamespace) {
        // Fallback to global scope for scripts loaded via LoadScript
        return m_runtime->HasFunction(m_context, name);
    }

    // Check if the function exists in module exports
    JSStringRef funcName = JSStringCreateWithUTF8CString(name.c_str());
    JSValueRef funcVal = JSObjectGetProperty(m_context, m_moduleNamespace, funcName, nullptr);
    JSStringRelease(funcName);

    if (!funcVal || JSValueIsUndefined(m_context, funcVal)) {
        return false;
    }

    if (!JSValueIsObject(m_context, funcVal)) {
        return false;
    }

    JSObjectRef funcObj = JSValueToObject(m_context, funcVal, nullptr);
    return funcObj && JSObjectIsFunction(m_context, funcObj);
}

} // namespace rdr2js
