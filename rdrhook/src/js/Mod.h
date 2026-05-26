#pragma once

// Per-mod state. Each loaded mod owns one ejsc::Context, a TimerManager,
// callback registries (tick / key down / key up), and a Vector3 class binding.
//
// Native dispatch (Native.invoke, the generated natives table, _Core etc.)
// looks up the owning Mod for a given JSC global context via ModFromContext.

#include <ejsc/ejsc.h>
#include <ejsc/extra/timer_manager.h>

#include <JavaScriptCore/JSBase.h>

#include <cstdint>
#include <filesystem>
#include <memory>
#include <string>
#include <vector>

namespace rdr2js {

struct Vec3 {
    double x{}, y{}, z{};
};

struct ModManifest {
    std::string name;
    std::string version;
    std::string author;
    std::string description;
    std::string entrypoint;
    std::filesystem::path modPath;
    bool IsValid() const { return !name.empty() && !entrypoint.empty(); }
};

struct CallbackEntry {
    uint32_t id;
    ejsc::Value callback;
};

class Mod {
public:
    Mod(ejsc::Runtime& runtime, ModManifest manifest);
    ~Mod();

    Mod(const Mod&) = delete;
    Mod& operator=(const Mod&) = delete;

    // Read manifest.entrypoint and run it as an ES module. Returns false on
    // exception (which is logged).
    bool LoadEntrypoint();

    // Per-frame entry points. Called by ModLoader from the game's main loop.
    void Tick();                       // tick callbacks + drains timers + drains microtasks
    void OnKeyDown(uint32_t key);
    void OnKeyUp(uint32_t key);

    // core module helpers, called from the JS-side closures.
    uint32_t AddTickCallback(ejsc::Value cb);
    bool     RemoveTickCallback(uint32_t id);
    uint32_t AddKeyDownCallback(ejsc::Value cb);
    bool     RemoveKeyDownCallback(uint32_t id);
    uint32_t AddKeyUpCallback(ejsc::Value cb);
    bool     RemoveKeyUpCallback(uint32_t id);

    // Accessors.
    ejsc::Context&     Context() noexcept       { return m_ctx; }
    const ModManifest& Manifest() const noexcept { return m_manifest; }
    ejsc::Class<Vec3>& Vec3Class() noexcept     { return m_vec3Cls; }

    JSGlobalContextRef GlobalContextRef() const;

private:
    void   DispatchCallbacks(std::vector<CallbackEntry>& list, const ejsc::Value& arg);
    void   InstallBindings();          // calls Bindings::InstallAll(*this)

    ModManifest m_manifest;
    ejsc::Context m_ctx;
    ejsc::extra::TimerManager m_timers;
    ejsc::Value m_moduleNamespace;
    ejsc::Class<Vec3> m_vec3Cls;
    std::vector<CallbackEntry> m_tickCallbacks;
    std::vector<CallbackEntry> m_keyDownCallbacks;
    std::vector<CallbackEntry> m_keyUpCallbacks;
    uint32_t m_nextCallbackId = 1;
};

// Singleton runtime accessor (one Runtime for the process).
ejsc::Runtime& GetRuntime();

// Game-side function pointers used by Native.invoke / Global.*. Install once
// from CScriptManager before any mod is loaded.
using GetNativeAddressFunc = uintptr_t(*)(uint64_t hash);
using GetGlobalPointerFunc = void*(*)(uint32_t globalVarId);

void InstallGameBridge(GetNativeAddressFunc getNativeAddr,
                       GetGlobalPointerFunc getGlobalPtr);
GetNativeAddressFunc GetNativeAddrFn();
GetGlobalPointerFunc GetGlobalPtrFn();

// Mod registry. Used by the generated native dispatcher to find the owning
// Mod when it only has a JS context handle in scope.
void RegisterMod(JSGlobalContextRef gctx, Mod* mod);
void UnregisterMod(JSGlobalContextRef gctx);
Mod* ModFromContext(JSGlobalContextRef gctx);

} // namespace rdr2js
