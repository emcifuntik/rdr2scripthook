#include "stdafx.h"
#include "ModuleRegistry.h"
#include "JSNativeBindings.h"
#include "JSCoreModule.h"
#include "JSNativesGenerated.h"
#include "JSRuntime.h"
#include "Logger.h"

namespace rdr2js {

ModuleRegistry::~ModuleRegistry() {
    Cleanup();
}

void ModuleRegistry::Initialize(JSGlobalContextRef ctx, JSRuntime* runtime) {
    m_context = ctx;

    // Create the 'natives' object with all 7000+ native functions
    JSObjectRef nativesObj = JSObjectMake(ctx, nullptr, nullptr);
    RegisterGeneratedNatives(ctx, nativesObj);
    JSValueProtect(ctx, nativesObj);
    m_modules["natives"] = nativesObj;

    spdlog::info("[ModuleRegistry] Created 'natives' module with {} functions", GetRegisteredNativeCount());

    // Create the 'core' object with callback registration API
    JSObjectRef coreObj = CreateCoreModuleObject(ctx);
    JSValueProtect(ctx, coreObj);
    m_modules["core"] = coreObj;

    spdlog::info("[ModuleRegistry] Created 'core' module");
}

JSObjectRef ModuleRegistry::GetModule(const std::string& name) const {
    auto it = m_modules.find(name);
    if (it != m_modules.end()) {
        return it->second;
    }
    return nullptr;
}

bool ModuleRegistry::HasModule(const std::string& name) const {
    return m_modules.find(name) != m_modules.end();
}

void ModuleRegistry::Cleanup() {
    if (m_context) {
        for (auto& [name, obj] : m_modules) {
            if (obj) {
                JSValueUnprotect(m_context, obj);
            }
        }
    }
    m_modules.clear();
    m_context = nullptr;
}

} // namespace rdr2js
