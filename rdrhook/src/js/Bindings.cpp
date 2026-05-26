#include "stdafx.h"
#include "Bindings.h"
#include "Mod.h"
#include "Logger.h"
#include "JSNativesGenerated.h"

#include <cstdint>
#include <cstdio>
#include <cstring>
#include <sstream>
#include <string>

#include <JavaScriptCore/APICast.h>
#include <JavaScriptCore/JSCJSValue.h>
#include <JavaScriptCore/JSGlobalObject.h>
#include <JavaScriptCore/JavaScript.h>

#include <Windows.h>

namespace rdr2js::bindings {

// ---------------------------------------------------------------------------
// Shared keyboard / game-time state (visible to all mods via _Core globals).
// ---------------------------------------------------------------------------

namespace {
uint8_t  g_keyStates[256]     = {};
uint8_t  g_prevKeyStates[256] = {};
uint32_t g_gameTime           = 0;
} // namespace

void PollKeyboard() {
    std::memcpy(g_prevKeyStates, g_keyStates, sizeof(g_keyStates));
    for (int i = 0; i < 256; ++i) {
        g_keyStates[i] = (GetAsyncKeyState(i) & 0x8000) ? 1 : 0;
    }
}

void SetGameTime(unsigned long timeMs) {
    g_gameTime = static_cast<uint32_t>(timeMs);
}

// ---------------------------------------------------------------------------
// Helpers
// ---------------------------------------------------------------------------

namespace {

std::string ValueToString(const ejsc::Value& v) {
    return v.ToString().value_or("");
}

double ValueToNumber(const ejsc::Value& v) {
    return v.ToNumber().value_or(0.0);
}

JSGlobalContextRef RawCtx(ejsc::Context& ctx) {
    return static_cast<JSGlobalContextRef>(ctx.RawGlobalContextRef());
}
JSGlobalContextRef RawCtx(const ejsc::Context& ctx) {
    return static_cast<JSGlobalContextRef>(ctx.RawGlobalContextRef());
}

uint64_t ValueToUint64(ejsc::Context& ctx, const ejsc::Value& v) {
    // JSC C-API doesn't expose isBigInt cleanly; reach through the JSC++ API.
    JSC::JSGlobalObject* go = ::toJS(RawCtx(ctx));
    JSC::JSValue jv = ::toJS(go, v.GetRef());
    if (jv.isBigInt()) {
        return jv.toBigUInt64(go);
    }
    return static_cast<uint64_t>(ValueToNumber(v));
}

uint32_t JoaatHash(const std::string& str) {
    uint32_t hash = 0;
    for (char c : str) {
        hash += static_cast<unsigned char>(std::tolower(static_cast<unsigned char>(c)));
        hash += (hash << 10);
        hash ^= (hash >> 6);
    }
    hash += (hash << 3);
    hash ^= (hash >> 11);
    hash += (hash << 15);
    return hash;
}

// ---------------------------------------------------------------------------
// SEH-safe native handler invocation (lives here because Native.invoke needs
// it; the generated dispatcher has its own copy for tight inlining).
// ---------------------------------------------------------------------------

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
        std::memset(stack, 0, sizeof(stack));
    }

    void Push(uint64_t v) { stack[argCount++] = v; }

    template<typename T>
    T Result() { return *reinterpret_cast<T*>(retVal); }

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
typedef void(__cdecl* NativeHandler)(NativeContext* ctx);

static bool SafeCall(NativeHandler h, NativeContext* c) {
    __try { h(c); return true; }
    __except (EXCEPTION_EXECUTE_HANDLER) { return false; }
}
static bool SafeCopyResults(NativeContext* c) {
    __try { c->CopyResults(); return true; }
    __except (EXCEPTION_EXECUTE_HANDLER) { return false; }
}

// Push a JS arg onto the native call stack. Returns false to short-circuit.
void PushArg(ejsc::Context& ctx, NativeContext& nctx,
             const ejsc::Value& arg, std::vector<std::string>& stringStorage,
             Mod& mod) {
    JSGlobalContextRef gctx = RawCtx(ctx);
    JSValueRef raw = arg.GetRef();

    if (arg.IsNumber()) {
        double num = ValueToNumber(arg);
        if (num == static_cast<double>(static_cast<int64_t>(num))) {
            nctx.Push(static_cast<uint64_t>(static_cast<int64_t>(num)));
        } else {
            float f = static_cast<float>(num);
            nctx.Push(static_cast<uint64_t>(*reinterpret_cast<uint32_t*>(&f)));
        }
    } else if (arg.IsBool()) {
        nctx.Push(arg.ToBool().value_or(false) ? 1ULL : 0ULL);
    } else if (arg.IsString()) {
        stringStorage.push_back(ValueToString(arg));
        nctx.Push(reinterpret_cast<uint64_t>(stringStorage.back().c_str()));
    } else if (arg.IsObject()) {
        // Vector3 instance (real class binding) — preferred path.
        if (Vec3* v = mod.Vec3Class().Unwrap(arg)) {
            float x = static_cast<float>(v->x);
            float y = static_cast<float>(v->y);
            float z = static_cast<float>(v->z);
            nctx.Push(static_cast<uint64_t>(*reinterpret_cast<uint32_t*>(&x)));
            nctx.Push(static_cast<uint64_t>(*reinterpret_cast<uint32_t*>(&y)));
            nctx.Push(static_cast<uint64_t>(*reinterpret_cast<uint32_t*>(&z)));
            return;
        }

        // Native string handle ({ __nativePtr__: number }) — passed through
        // raw for chained native calls.
        JSObjectRef obj = JSValueToObject(gctx, raw, nullptr);
        JSStringRef ptrProp = JSStringCreateWithUTF8CString("__nativePtr__");
        if (JSObjectHasProperty(gctx, obj, ptrProp)) {
            double ptr = JSValueToNumber(gctx,
                JSObjectGetProperty(gctx, obj, ptrProp, nullptr), nullptr);
            nctx.Push(static_cast<uint64_t>(ptr));
            JSStringRelease(ptrProp);
            return;
        }
        JSStringRelease(ptrProp);

        // Duck-typed Vector3 fallback (plain { x, y, z } object).
        JSStringRef xProp = JSStringCreateWithUTF8CString("x");
        if (JSObjectHasProperty(gctx, obj, xProp)) {
            JSStringRef yProp = JSStringCreateWithUTF8CString("y");
            JSStringRef zProp = JSStringCreateWithUTF8CString("z");
            float x = static_cast<float>(JSValueToNumber(gctx, JSObjectGetProperty(gctx, obj, xProp, nullptr), nullptr));
            float y = static_cast<float>(JSValueToNumber(gctx, JSObjectGetProperty(gctx, obj, yProp, nullptr), nullptr));
            float z = static_cast<float>(JSValueToNumber(gctx, JSObjectGetProperty(gctx, obj, zProp, nullptr), nullptr));
            nctx.Push(static_cast<uint64_t>(*reinterpret_cast<uint32_t*>(&x)));
            nctx.Push(static_cast<uint64_t>(*reinterpret_cast<uint32_t*>(&y)));
            nctx.Push(static_cast<uint64_t>(*reinterpret_cast<uint32_t*>(&z)));
            JSStringRelease(yProp);
            JSStringRelease(zProp);
        } else {
            nctx.Push(0ULL);
        }
        JSStringRelease(xProp);
    } else {
        nctx.Push(0ULL);
    }
}

} // namespace

// ---------------------------------------------------------------------------
// Individual binding installers
// ---------------------------------------------------------------------------

namespace {

void InstallConsole(Mod& mod) {
    auto& c = mod.Context();
    auto consoleObj = ejsc::Value::Object(c);

    auto mkLog = [&c, name = mod.Manifest().name](spdlog::level::level_enum lvl, const char* tag) {
        return ejsc::Value::Function(c, tag,
            [name, lvl](ejsc::Context& cc, const ejsc::Value&,
                        std::span<const ejsc::Value> args) {
                std::ostringstream ss;
                for (size_t i = 0; i < args.size(); ++i) {
                    if (i > 0) ss << ' ';
                    ss << ValueToString(args[i]);
                }
                spdlog::log(lvl, "[JS:{}] {}", name, ss.str());
                return ejsc::Value::Undefined(cc);
            });
    };
    consoleObj.SetProperty("log",   mkLog(spdlog::level::info,  "log"));
    consoleObj.SetProperty("warn",  mkLog(spdlog::level::warn,  "warn"));
    consoleObj.SetProperty("error", mkLog(spdlog::level::err,   "error"));
    consoleObj.SetProperty("debug", mkLog(spdlog::level::debug, "debug"));
    c.SetGlobal("console", consoleObj);
}

void InstallHash(Mod& mod) {
    auto& c = mod.Context();
    auto hashObj = ejsc::Value::Object(c);
    hashObj.SetProperty("joaat", ejsc::Value::Function(c, "joaat",
        [](ejsc::Context& cc, const ejsc::Value&, std::span<const ejsc::Value> args) {
            if (args.empty()) return ejsc::Value::Number(cc, 0);
            std::string s = ValueToString(args[0]);
            return ejsc::Value::Number(cc, static_cast<double>(JoaatHash(s)));
        }));
    c.SetGlobal("Hash", hashObj);
}

void InstallVKConstants(Mod& mod) {
    auto& c = mod.Context();

    auto vk = [&](const char* name, int code) {
        c.SetGlobal(name, ejsc::Value::Number(c, static_cast<double>(code)));
    };

    vk("VK_BACK", 0x08); vk("VK_TAB", 0x09); vk("VK_RETURN", 0x0D);
    vk("VK_SHIFT", 0x10); vk("VK_CONTROL", 0x11); vk("VK_MENU", 0x12);
    vk("VK_PAUSE", 0x13); vk("VK_CAPITAL", 0x14); vk("VK_ESCAPE", 0x1B);
    vk("VK_SPACE", 0x20); vk("VK_PRIOR", 0x21); vk("VK_NEXT", 0x22);
    vk("VK_END", 0x23); vk("VK_HOME", 0x24);
    vk("VK_LEFT", 0x25); vk("VK_UP", 0x26); vk("VK_RIGHT", 0x27); vk("VK_DOWN", 0x28);
    vk("VK_INSERT", 0x2D); vk("VK_DELETE", 0x2E);

    char buf[16];
    for (int i = 0; i <= 9; ++i) {
        std::snprintf(buf, sizeof(buf), "VK_%d", i);
        vk(buf, 0x30 + i);
    }
    for (int i = 0; i < 26; ++i) {
        std::snprintf(buf, sizeof(buf), "VK_%c", 'A' + i);
        vk(buf, 0x41 + i);
    }
    for (int i = 1; i <= 12; ++i) {
        std::snprintf(buf, sizeof(buf), "VK_F%d", i);
        vk(buf, 0x6F + i);
    }
    for (int i = 0; i <= 9; ++i) {
        std::snprintf(buf, sizeof(buf), "VK_NUMPAD%d", i);
        vk(buf, 0x60 + i);
    }
    vk("VK_MULTIPLY", 0x6A); vk("VK_ADD", 0x6B); vk("VK_SUBTRACT", 0x6D);
    vk("VK_DECIMAL", 0x6E); vk("VK_DIVIDE", 0x6F);
}

void InstallVector3(Mod& mod) {
    auto& c = mod.Context();

    auto cls = c.NewClass<Vec3>("Vector3")
        .Constructor([](ejsc::Context&, std::span<const ejsc::Value> args) -> Vec3* {
            auto* v = new Vec3;
            if (args.size() > 0) v->x = ValueToNumber(args[0]);
            if (args.size() > 1) v->y = ValueToNumber(args[1]);
            if (args.size() > 2) v->z = ValueToNumber(args[2]);
            return v;
        })
        .Property("x",
            [](const Vec3& s, ejsc::Context& cc) { return ejsc::Value::Number(cc, s.x); },
            [](Vec3& s, ejsc::Context&, const ejsc::Value& v) { s.x = ValueToNumber(v); })
        .Property("y",
            [](const Vec3& s, ejsc::Context& cc) { return ejsc::Value::Number(cc, s.y); },
            [](Vec3& s, ejsc::Context&, const ejsc::Value& v) { s.y = ValueToNumber(v); })
        .Property("z",
            [](const Vec3& s, ejsc::Context& cc) { return ejsc::Value::Number(cc, s.z); },
            [](Vec3& s, ejsc::Context&, const ejsc::Value& v) { s.z = ValueToNumber(v); })
        .Property("length",
            [](const Vec3& s, ejsc::Context& cc) {
                double L = std::sqrt(s.x * s.x + s.y * s.y + s.z * s.z);
                return ejsc::Value::Number(cc, L);
            })
        .Method("add", [](Vec3& self, ejsc::Context& cc, std::span<const ejsc::Value> args) -> ejsc::Value {
            Mod* mod = ModFromContext(RawCtx(cc));
            if (!mod || args.empty()) return ejsc::Value::Undefined(cc);
            Vec3* other = mod->Vec3Class().Unwrap(args[0]);
            if (!other) return ejsc::Value::Undefined(cc);
            return mod->Vec3Class().New({
                ejsc::Value::Number(cc, self.x + other->x),
                ejsc::Value::Number(cc, self.y + other->y),
                ejsc::Value::Number(cc, self.z + other->z),
            });
        })
        .Method("sub", [](Vec3& self, ejsc::Context& cc, std::span<const ejsc::Value> args) -> ejsc::Value {
            Mod* mod = ModFromContext(RawCtx(cc));
            if (!mod || args.empty()) return ejsc::Value::Undefined(cc);
            Vec3* other = mod->Vec3Class().Unwrap(args[0]);
            if (!other) return ejsc::Value::Undefined(cc);
            return mod->Vec3Class().New({
                ejsc::Value::Number(cc, self.x - other->x),
                ejsc::Value::Number(cc, self.y - other->y),
                ejsc::Value::Number(cc, self.z - other->z),
            });
        })
        .Method("mul", [](Vec3& self, ejsc::Context& cc, std::span<const ejsc::Value> args) -> ejsc::Value {
            double s = args.empty() ? 1.0 : ValueToNumber(args[0]);
            Mod* mod = ModFromContext(RawCtx(cc));
            if (!mod) return ejsc::Value::Undefined(cc);
            return mod->Vec3Class().New({
                ejsc::Value::Number(cc, self.x * s),
                ejsc::Value::Number(cc, self.y * s),
                ejsc::Value::Number(cc, self.z * s),
            });
        })
        .Method("normalize", [](Vec3& self, ejsc::Context& cc, auto) -> ejsc::Value {
            double L = std::sqrt(self.x * self.x + self.y * self.y + self.z * self.z);
            Mod* mod = ModFromContext(RawCtx(cc));
            if (!mod) return ejsc::Value::Undefined(cc);
            if (L == 0.0) {
                return mod->Vec3Class().New({
                    ejsc::Value::Number(cc, 0), ejsc::Value::Number(cc, 0), ejsc::Value::Number(cc, 0),
                });
            }
            return mod->Vec3Class().New({
                ejsc::Value::Number(cc, self.x / L),
                ejsc::Value::Number(cc, self.y / L),
                ejsc::Value::Number(cc, self.z / L),
            });
        })
        .Method("toString", [](Vec3& self, ejsc::Context& cc, auto) {
            std::ostringstream ss;
            ss << "Vector3(" << self.x << ", " << self.y << ", " << self.z << ")";
            return ejsc::Value::String(cc, ss.str());
        })
        .Build();

    mod.Vec3Class() = cls;
    c.SetGlobal("Vector3", cls.ConstructorValue());
}

// Native object — low-level entry points for callers that don't want the
// generated by-name natives table.
void InstallNative(Mod& mod) {
    auto& c = mod.Context();
    auto nativeObj = ejsc::Value::Object(c);

    auto invokeImpl = [&mod, &c](std::span<const ejsc::Value> args, ejsc::Context& cc) -> uint64_t {
        if (args.empty()) {
            throw ejsc::Error("Native.invoke requires at least a hash argument");
        }
        auto getNativeAddr = GetNativeAddrFn();
        if (!getNativeAddr) {
            throw ejsc::Error("Native bridge not installed");
        }

        uint64_t hash = ValueToUint64(cc, args[0]);
        uintptr_t addr = getNativeAddr(hash);
        if (!addr) {
            throw ejsc::Error("Native function not found");
        }

        NativeHandler handler = reinterpret_cast<NativeHandler>(addr);
        static thread_local NativeContext nctx;
        static thread_local std::vector<std::string> stringStorage;
        nctx.Reset();
        stringStorage.clear();

        for (size_t i = 1; i < args.size(); ++i) {
            PushArg(cc, nctx, args[i], stringStorage, mod);
        }

        if (!SafeCall(handler, &nctx)) {
            throw ejsc::Error("Native function crashed");
        }
        SafeCopyResults(&nctx);
        return nctx.Result<uint64_t>();
    };

    nativeObj.SetProperty("invoke", ejsc::Value::Function(c, "invoke",
        [invokeImpl](ejsc::Context& cc, const ejsc::Value&, std::span<const ejsc::Value> args) {
            uint64_t r = invokeImpl(args, cc);
            return ejsc::Value::Number(cc, static_cast<double>(r));
        }));

    nativeObj.SetProperty("invokeFloat", ejsc::Value::Function(c, "invokeFloat",
        [invokeImpl](ejsc::Context& cc, const ejsc::Value&, std::span<const ejsc::Value> args) {
            uint64_t r = invokeImpl(args, cc);
            uint32_t bits = static_cast<uint32_t>(r);
            float f = *reinterpret_cast<float*>(&bits);
            return ejsc::Value::Number(cc, static_cast<double>(f));
        }));

    nativeObj.SetProperty("invokeVector3", ejsc::Value::Function(c, "invokeVector3",
        [&mod](ejsc::Context& cc, const ejsc::Value&, std::span<const ejsc::Value> args) -> ejsc::Value {
            if (args.empty()) {
                throw ejsc::Error("Native.invokeVector3 requires at least a hash argument");
            }
            auto getNativeAddr = GetNativeAddrFn();
            if (!getNativeAddr) throw ejsc::Error("Native bridge not installed");

            uint64_t hash = ValueToUint64(cc, args[0]);
            uintptr_t addr = getNativeAddr(hash);
            if (!addr) throw ejsc::Error("Native function not found");

            NativeHandler handler = reinterpret_cast<NativeHandler>(addr);
            static thread_local NativeContext nctx;
            static thread_local std::vector<std::string> stringStorage;
            nctx.Reset();
            stringStorage.clear();

            for (size_t i = 1; i < args.size(); ++i) {
                PushArg(cc, nctx, args[i], stringStorage, mod);
            }
            if (!SafeCall(handler, &nctx)) throw ejsc::Error("Native function crashed");
            SafeCopyResults(&nctx);

            float x = *reinterpret_cast<float*>(nctx.retVal + 0);
            float y = *reinterpret_cast<float*>((uintptr_t)nctx.retVal + 8);
            float z = *reinterpret_cast<float*>((uintptr_t)nctx.retVal + 16);
            return mod.Vec3Class().New({
                ejsc::Value::Number(cc, x),
                ejsc::Value::Number(cc, y),
                ejsc::Value::Number(cc, z),
            });
        }));

    mod.Context().SetGlobal("Native", nativeObj);
}

// Global object — read/write game globals by id.
void InstallGlobalAccessor(Mod& mod) {
    auto& c = mod.Context();
    auto globalObj = ejsc::Value::Object(c);

    auto requireBridge = []() {
        auto fn = GetGlobalPtrFn();
        if (!fn) throw ejsc::Error("Global pointer bridge not installed");
        return fn;
    };

    globalObj.SetProperty("getInt", ejsc::Value::Function(c, "getInt",
        [requireBridge](ejsc::Context& cc, const ejsc::Value&, std::span<const ejsc::Value> args) {
            if (args.empty()) return ejsc::Value::Number(cc, 0);
            uint32_t id = static_cast<uint32_t>(ValueToNumber(args[0]));
            void* p = requireBridge()(id);
            return ejsc::Value::Number(cc, p ? static_cast<double>(*static_cast<int32_t*>(p)) : 0.0);
        }));

    globalObj.SetProperty("setInt", ejsc::Value::Function(c, "setInt",
        [requireBridge](ejsc::Context& cc, const ejsc::Value&, std::span<const ejsc::Value> args) {
            if (args.size() < 2) return ejsc::Value::Undefined(cc);
            uint32_t id = static_cast<uint32_t>(ValueToNumber(args[0]));
            int32_t  v  = static_cast<int32_t>(ValueToNumber(args[1]));
            if (void* p = requireBridge()(id)) *static_cast<int32_t*>(p) = v;
            return ejsc::Value::Undefined(cc);
        }));

    globalObj.SetProperty("getFloat", ejsc::Value::Function(c, "getFloat",
        [requireBridge](ejsc::Context& cc, const ejsc::Value&, std::span<const ejsc::Value> args) {
            if (args.empty()) return ejsc::Value::Number(cc, 0);
            uint32_t id = static_cast<uint32_t>(ValueToNumber(args[0]));
            void* p = requireBridge()(id);
            return ejsc::Value::Number(cc, p ? static_cast<double>(*static_cast<float*>(p)) : 0.0);
        }));

    globalObj.SetProperty("setFloat", ejsc::Value::Function(c, "setFloat",
        [requireBridge](ejsc::Context& cc, const ejsc::Value&, std::span<const ejsc::Value> args) {
            if (args.size() < 2) return ejsc::Value::Undefined(cc);
            uint32_t id = static_cast<uint32_t>(ValueToNumber(args[0]));
            float    v  = static_cast<float>(ValueToNumber(args[1]));
            if (void* p = requireBridge()(id)) *static_cast<float*>(p) = v;
            return ejsc::Value::Undefined(cc);
        }));

    c.SetGlobal("Global", globalObj);
}

// _Core global — getGameTime / isKeyPressed / isKeyJustPressed.
void InstallCoreGlobal(Mod& mod) {
    auto& c = mod.Context();
    auto coreObj = ejsc::Value::Object(c);

    coreObj.SetProperty("getGameTime", ejsc::Value::Function(c, "getGameTime",
        [](ejsc::Context& cc, const ejsc::Value&, std::span<const ejsc::Value>) {
            return ejsc::Value::Number(cc, static_cast<double>(g_gameTime));
        }));

    coreObj.SetProperty("isKeyPressed", ejsc::Value::Function(c, "isKeyPressed",
        [](ejsc::Context& cc, const ejsc::Value&, std::span<const ejsc::Value> args) {
            if (args.empty()) return ejsc::Value::Bool(cc, false);
            int k = static_cast<int>(ValueToNumber(args[0]));
            if (k < 0 || k >= 256) return ejsc::Value::Bool(cc, false);
            return ejsc::Value::Bool(cc, g_keyStates[k] != 0);
        }));

    coreObj.SetProperty("isKeyJustPressed", ejsc::Value::Function(c, "isKeyJustPressed",
        [](ejsc::Context& cc, const ejsc::Value&, std::span<const ejsc::Value> args) {
            if (args.empty()) return ejsc::Value::Bool(cc, false);
            int k = static_cast<int>(ValueToNumber(args[0]));
            if (k < 0 || k >= 256) return ejsc::Value::Bool(cc, false);
            return ejsc::Value::Bool(cc, g_keyStates[k] != 0 && g_prevKeyStates[k] == 0);
        }));

    c.SetGlobal("_Core", coreObj);
}

// 'core' ES module — addTickCallback / addKeyDownCallback / etc.
void InstallCoreModule(Mod& mod) {
    auto& c = mod.Context();
    auto core = c.NewModule("core");

    auto requireFn = [](std::span<const ejsc::Value> args, const char* name) -> const ejsc::Value& {
        if (args.empty() || !args[0].IsFunction()) {
            throw ejsc::Error(std::string(name) + " requires a function argument");
        }
        return args[0];
    };

    Mod* modPtr = &mod;

    core.ExportFunction("addTickCallback",
        [modPtr, requireFn](ejsc::Context& cc, const ejsc::Value&, std::span<const ejsc::Value> args) {
            auto& fn = requireFn(args, "addTickCallback");
            return ejsc::Value::Number(cc,
                static_cast<double>(modPtr->AddTickCallback(fn)));
        });
    core.ExportFunction("removeTickCallback",
        [modPtr](ejsc::Context& cc, const ejsc::Value&, std::span<const ejsc::Value> args) {
            if (args.empty()) return ejsc::Value::Bool(cc, false);
            uint32_t id = static_cast<uint32_t>(ValueToNumber(args[0]));
            return ejsc::Value::Bool(cc, modPtr->RemoveTickCallback(id));
        });
    core.ExportFunction("addKeyDownCallback",
        [modPtr, requireFn](ejsc::Context& cc, const ejsc::Value&, std::span<const ejsc::Value> args) {
            auto& fn = requireFn(args, "addKeyDownCallback");
            return ejsc::Value::Number(cc,
                static_cast<double>(modPtr->AddKeyDownCallback(fn)));
        });
    core.ExportFunction("removeKeyDownCallback",
        [modPtr](ejsc::Context& cc, const ejsc::Value&, std::span<const ejsc::Value> args) {
            if (args.empty()) return ejsc::Value::Bool(cc, false);
            uint32_t id = static_cast<uint32_t>(ValueToNumber(args[0]));
            return ejsc::Value::Bool(cc, modPtr->RemoveKeyDownCallback(id));
        });
    core.ExportFunction("addKeyUpCallback",
        [modPtr, requireFn](ejsc::Context& cc, const ejsc::Value&, std::span<const ejsc::Value> args) {
            auto& fn = requireFn(args, "addKeyUpCallback");
            return ejsc::Value::Number(cc,
                static_cast<double>(modPtr->AddKeyUpCallback(fn)));
        });
    core.ExportFunction("removeKeyUpCallback",
        [modPtr](ejsc::Context& cc, const ejsc::Value&, std::span<const ejsc::Value> args) {
            if (args.empty()) return ejsc::Value::Bool(cc, false);
            uint32_t id = static_cast<uint32_t>(ValueToNumber(args[0]));
            return ejsc::Value::Bool(cc, modPtr->RemoveKeyUpCallback(id));
        });
    core.ExportFunction("getGameTime",
        [](ejsc::Context& cc, const ejsc::Value&, std::span<const ejsc::Value>) {
            return ejsc::Value::Number(cc, static_cast<double>(g_gameTime));
        });
    core.Build();
}

// 'natives' ES module — default-exports the big natives object populated by
// the generated registration helper.
void InstallNativesModule(Mod& mod) {
    auto& c = mod.Context();
    auto natives = c.NewModule("natives");

    auto nativesObj = ejsc::Value::Object(c);
    JSObjectRef rawNatives = JSValueToObject(RawCtx(c), nativesObj.GetRef(), nullptr);
    RegisterGeneratedNatives(RawCtx(c), rawNatives);

    natives.Export("default", nativesObj);
    natives.Build();
}

} // namespace

// ---------------------------------------------------------------------------
// Public entry point
// ---------------------------------------------------------------------------

void InstallAll(Mod& mod) {
    InstallConsole(mod);
    InstallHash(mod);
    InstallVKConstants(mod);
    InstallVector3(mod);
    InstallNative(mod);
    InstallGlobalAccessor(mod);
    InstallCoreGlobal(mod);
    InstallCoreModule(mod);
    InstallNativesModule(mod);
}

} // namespace rdr2js::bindings
