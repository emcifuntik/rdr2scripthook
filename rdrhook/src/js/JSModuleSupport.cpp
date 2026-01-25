#include "stdafx.h"
#include "JSModuleSupport.h"
#include "Logger.h"

#include <JavaScriptCore/JavaScript.h>
#include <fstream>
#include <sstream>

namespace rdr2js {

std::string JSModuleSupport::s_emptyString;

// Thread-local for current module base path
static thread_local std::filesystem::path g_currentModPath;

// Read file contents
static std::string ReadFile(const std::filesystem::path& path) {
    std::ifstream file(path, std::ios::binary);
    if (!file.is_open()) {
        return "";
    }
    std::ostringstream ss;
    ss << file.rdbuf();
    return ss.str();
}

JSModuleSupport& JSModuleSupport::Get() {
    static JSModuleSupport instance;
    return instance;
}

void JSModuleSupport::Initialize() {
    if (m_initialized) return;
    m_initialized = true;
    spdlog::info("[JSModuleSupport] Initialized");
}

void JSModuleSupport::RegisterModule(const std::string& name, const std::string& source) {
    m_modules[name] = source;
    spdlog::info("[JSModuleSupport] Registered module: {} ({} bytes)", name, source.size());
}

void JSModuleSupport::LoadModulesFromPath(const std::filesystem::path& modulesPath) {
    spdlog::info("[JSModuleSupport] Loading modules from: {}", modulesPath.string());

    // Load core.js
    auto corePath = modulesPath / "core.js";
    if (std::filesystem::exists(corePath)) {
        std::string source = ReadFile(corePath);
        if (!source.empty()) {
            RegisterModule("core", source);
        }
    }

    // Load natives.js
    auto nativesPath = modulesPath / "natives.js";
    if (std::filesystem::exists(nativesPath)) {
        std::string source = ReadFile(nativesPath);
        if (!source.empty()) {
            RegisterModule("natives", source);
        }
    }

    // Load natives_hashes.js
    auto hashesPath = modulesPath / "natives_hashes.js";
    if (std::filesystem::exists(hashesPath)) {
        std::string source = ReadFile(hashesPath);
        if (!source.empty()) {
            RegisterModule("natives_hashes", source);
        }
    }
}

void JSModuleSupport::SetModBasePath(const std::filesystem::path& path) {
    m_modBasePath = path;
    g_currentModPath = path;
}

bool JSModuleSupport::EvaluateModule(JSGlobalContextRef ctx, const std::string& source, const std::string& sourceURL) {
    // For now, we use the script evaluation path since direct module API access
    // requires internal JSC headers that are incompatible with external builds.
    // The transform approach in ModuleRegistry handles import/export syntax.

    JSStringRef sourceStr = JSStringCreateWithUTF8CString(source.c_str());
    JSStringRef sourceURLStr = JSStringCreateWithUTF8CString(sourceURL.c_str());

    JSValueRef exception = nullptr;
    JSValueRef result = JSEvaluateScript(ctx, sourceStr, nullptr, sourceURLStr, 1, &exception);

    JSStringRelease(sourceStr);
    JSStringRelease(sourceURLStr);

    if (exception) {
        JSStringRef exStr = JSValueToStringCopy(ctx, exception, nullptr);
        size_t bufSize = JSStringGetMaximumUTF8CStringSize(exStr);
        std::string exMsg(bufSize, '\0');
        JSStringGetUTF8CString(exStr, exMsg.data(), bufSize);
        JSStringRelease(exStr);

        spdlog::error("[JSModuleSupport] Evaluation error: {}", exMsg.c_str());
        return false;
    }

    spdlog::debug("[JSModuleSupport] Evaluated script: {}", sourceURL);
    return true;
}

bool JSModuleSupport::IsBuiltinModule(const std::string& name) const {
    return m_modules.find(name) != m_modules.end();
}

const std::string& JSModuleSupport::GetModuleSource(const std::string& name) const {
    auto it = m_modules.find(name);
    if (it != m_modules.end()) {
        return it->second;
    }
    return s_emptyString;
}

} // namespace rdr2js
