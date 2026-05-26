#include "stdafx.h"
#include "Mod.h"
#include "Bindings.h"
#include "Logger.h"

#include <fstream>
#include <mutex>
#include <sstream>
#include <unordered_map>

#include <JavaScriptCore/APICast.h>
#include <JavaScriptCore/JavaScript.h>

namespace rdr2js {

// ---------------------------------------------------------------------------
// Process-wide state: Runtime singleton, game bridge fn pointers, Mod registry.
// ---------------------------------------------------------------------------

ejsc::Runtime& GetRuntime() {
    static ejsc::Runtime runtime;
    return runtime;
}

namespace {

GetNativeAddressFunc g_getNativeAddr = nullptr;
GetGlobalPointerFunc g_getGlobalPtr  = nullptr;

std::unordered_map<JSGlobalContextRef, Mod*>& ModMap() {
    static std::unordered_map<JSGlobalContextRef, Mod*> m;
    return m;
}
std::mutex& ModMapMutex() {
    static std::mutex m;
    return m;
}

std::string ReadFileContents(const std::filesystem::path& path) {
    try {
        std::ifstream file(path, std::ios::binary);
        if (!file.is_open()) return {};
        std::ostringstream ss;
        ss << file.rdbuf();
        return ss.str();
    } catch (const std::exception& e) {
        spdlog::error("[Mod] Error reading file {}: {}", path.string(), e.what());
        return {};
    }
}

} // namespace

void InstallGameBridge(GetNativeAddressFunc getNativeAddr,
                       GetGlobalPointerFunc getGlobalPtr) {
    g_getNativeAddr = getNativeAddr;
    g_getGlobalPtr  = getGlobalPtr;
}

GetNativeAddressFunc GetNativeAddrFn() { return g_getNativeAddr; }
GetGlobalPointerFunc GetGlobalPtrFn()  { return g_getGlobalPtr; }

void RegisterMod(JSGlobalContextRef gctx, Mod* mod) {
    std::lock_guard<std::mutex> lock(ModMapMutex());
    ModMap()[gctx] = mod;
}

void UnregisterMod(JSGlobalContextRef gctx) {
    std::lock_guard<std::mutex> lock(ModMapMutex());
    ModMap().erase(gctx);
}

Mod* ModFromContext(JSGlobalContextRef gctx) {
    std::lock_guard<std::mutex> lock(ModMapMutex());
    auto it = ModMap().find(gctx);
    return it == ModMap().end() ? nullptr : it->second;
}

// ---------------------------------------------------------------------------
// Mod lifecycle
// ---------------------------------------------------------------------------

Mod::Mod(ejsc::Runtime& runtime, ModManifest manifest)
    : m_manifest(std::move(manifest))
    , m_ctx(runtime.NewContext())
    , m_timers(m_ctx) {
    RegisterMod(GlobalContextRef(), this);

    // Mod-info globals available to the entrypoint script.
    auto& c = m_ctx;
    c.SetGlobal("__MOD_NAME__",    ejsc::Value::String(c, m_manifest.name));
    c.SetGlobal("__MOD_VERSION__", ejsc::Value::String(c, m_manifest.version));
    c.SetGlobal("__MOD_AUTHOR__",  ejsc::Value::String(c, m_manifest.author));
    c.SetGlobal("__MOD_PATH__",    ejsc::Value::String(c, m_manifest.modPath.string()));

    InstallBindings();
}

Mod::~Mod() {
    UnregisterMod(GlobalContextRef());
}

JSGlobalContextRef Mod::GlobalContextRef() const {
    return static_cast<JSGlobalContextRef>(m_ctx.RawGlobalContextRef());
}

void Mod::InstallBindings() {
    bindings::InstallAll(*this);
}

bool Mod::LoadEntrypoint() {
    auto entrypointPath = m_manifest.modPath / m_manifest.entrypoint;
    if (!std::filesystem::exists(entrypointPath)) {
        spdlog::error("[Mod] '{}': entrypoint not found: {}",
                      m_manifest.name, entrypointPath.string());
        return false;
    }

    std::string code = ReadFileContents(entrypointPath);
    if (code.empty()) {
        spdlog::error("[Mod] '{}': empty or unreadable entrypoint", m_manifest.name);
        return false;
    }

    auto sourceUrl = (m_manifest.modPath / m_manifest.entrypoint).string();
    ejsc::Value ns = m_ctx.EvalModule(code, sourceUrl);
    if (m_ctx.HasException()) {
        auto exc = m_ctx.TakeException().ToString().value_or("<unknown>");
        spdlog::error("[Mod] '{}': entrypoint threw: {}", m_manifest.name, exc);
        return false;
    }
    m_moduleNamespace = ns;
    return true;
}

// ---------------------------------------------------------------------------
// Tick / key dispatch
// ---------------------------------------------------------------------------

void Mod::DispatchCallbacks(std::vector<CallbackEntry>& list, const ejsc::Value& arg) {
    // Snapshot to allow callbacks to mutate the list re-entrantly.
    std::vector<ejsc::Value> snapshot;
    snapshot.reserve(list.size());
    for (const auto& e : list) snapshot.push_back(e.callback);

    for (const auto& cb : snapshot) {
        ejsc::Value args[] = { arg };
        bool hasArg = !arg.IsUndefined();
        cb.Call(ejsc::Value::Undefined(m_ctx),
                std::span<const ejsc::Value>(args, hasArg ? 1 : 0));
        if (m_ctx.HasException()) {
            auto exc = m_ctx.TakeException().ToString().value_or("<unknown>");
            spdlog::error("[Mod] '{}': callback error: {}", m_manifest.name, exc);
        }
    }
}

void Mod::Tick() {
    auto und = ejsc::Value::Undefined(m_ctx);
    DispatchCallbacks(m_tickCallbacks, und);
    m_timers.Tick();
    m_ctx.DrainMicrotasks();
}

void Mod::OnKeyDown(uint32_t key) {
    auto k = ejsc::Value::Number(m_ctx, static_cast<double>(key));
    DispatchCallbacks(m_keyDownCallbacks, k);
    m_ctx.DrainMicrotasks();
}

void Mod::OnKeyUp(uint32_t key) {
    auto k = ejsc::Value::Number(m_ctx, static_cast<double>(key));
    DispatchCallbacks(m_keyUpCallbacks, k);
    m_ctx.DrainMicrotasks();
}

// ---------------------------------------------------------------------------
// core module callback registry
// ---------------------------------------------------------------------------

uint32_t Mod::AddTickCallback(ejsc::Value cb) {
    uint32_t id = m_nextCallbackId++;
    m_tickCallbacks.push_back({ id, std::move(cb) });
    return id;
}
bool Mod::RemoveTickCallback(uint32_t id) {
    auto it = std::find_if(m_tickCallbacks.begin(), m_tickCallbacks.end(),
                           [id](const CallbackEntry& e){ return e.id == id; });
    if (it == m_tickCallbacks.end()) return false;
    m_tickCallbacks.erase(it);
    return true;
}
uint32_t Mod::AddKeyDownCallback(ejsc::Value cb) {
    uint32_t id = m_nextCallbackId++;
    m_keyDownCallbacks.push_back({ id, std::move(cb) });
    return id;
}
bool Mod::RemoveKeyDownCallback(uint32_t id) {
    auto it = std::find_if(m_keyDownCallbacks.begin(), m_keyDownCallbacks.end(),
                           [id](const CallbackEntry& e){ return e.id == id; });
    if (it == m_keyDownCallbacks.end()) return false;
    m_keyDownCallbacks.erase(it);
    return true;
}
uint32_t Mod::AddKeyUpCallback(ejsc::Value cb) {
    uint32_t id = m_nextCallbackId++;
    m_keyUpCallbacks.push_back({ id, std::move(cb) });
    return id;
}
bool Mod::RemoveKeyUpCallback(uint32_t id) {
    auto it = std::find_if(m_keyUpCallbacks.begin(), m_keyUpCallbacks.end(),
                           [id](const CallbackEntry& e){ return e.id == id; });
    if (it == m_keyUpCallbacks.end()) return false;
    m_keyUpCallbacks.erase(it);
    return true;
}

} // namespace rdr2js
