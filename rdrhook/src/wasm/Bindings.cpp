#include "Bindings.h"

#include "Logger.h"
#include "Mod.h"
#include "Runtime.h"
#include "input/InputBindingManager.h"
#ifndef RDR2_WASM_TEST
#include "input/GameInputHook.h"
#include "webview/WebViewOverlay.h"
#endif

#include <Windows.h>

#include <algorithm>
#include <array>
#include <bit>
#include <cctype>
#include <cstdint>
#include <cstring>
#include <initializer_list>
#include <iterator>
#include <limits>
#include <optional>
#include <mutex>
#include <string_view>
#include <string>
#include <unordered_map>
#include <vector>

#include <wasmtime.h>

namespace rdr2wasm::bindings {

namespace {

constexpr char ImportModule[] = "rdr2";
constexpr size_t MaxGuestString = 1024 * 1024;

uint8_t g_keyStates[256]{};
uint8_t g_previousKeyStates[256]{};
std::array<std::uint8_t, 256> g_rageKeyStates{};
#ifndef RDR2_WASM_TEST
std::optional<std::string> g_pendingWebViewMessage;
std::mutex g_cursorOwnerMutex;
std::unordered_map<Mod*, std::uint32_t> g_cursorOwners;
Mod* g_webViewOwner = nullptr;
#endif

enum class NativeArgKind : uint32_t {
    Raw = 0,
    Float = 1,
    Utf8 = 2,
    Vector3 = 3,
    InOutBuffer = 4,
    OutBuffer = 5,
};

enum class NativeStatus : int32_t {
    Ok = 0,
    InvalidMemory = 1,
    InvalidArgument = 2,
    BridgeUnavailable = 3,
    NotFound = 4,
    Crashed = 5,
    TooManyArguments = 6,
};

struct GuestNativeArg {
    uint32_t kind;
    uint32_t data;
    uint64_t value;
};
static_assert(sizeof(GuestNativeArg) == 16);

struct GuestNativeResult {
    uint64_t value;
    float vector[3];
    uint32_t reserved;
};
static_assert(sizeof(GuestNativeResult) == 24);

struct NativeContext {
    uint64_t* retVal = stack;
    uint64_t argCount = 0;
    uint64_t* stackPtr = stack;
    uint64_t dataCount = 0;
    uint64_t spaceForResults[24]{};
    uint64_t stack[24]{};

    void Reset() {
        retVal = stack;
        stackPtr = stack;
        argCount = 0;
        dataCount = 0;
        std::memset(spaceForResults, 0, sizeof(spaceForResults));
        std::memset(stack, 0, sizeof(stack));
    }

    bool Push(uint64_t value) {
        if (argCount >= std::size(stack)) return false;
        stack[argCount++] = value;
        return true;
    }

    void CopyResults() {
        uint64_t context = reinterpret_cast<uint64_t>(this);
        uint64_t result;
        for (; *reinterpret_cast<uint32_t*>(context + 24);
             *reinterpret_cast<uint32_t*>(
                 *reinterpret_cast<uint64_t*>(context + 8i64 *
                     *reinterpret_cast<int32_t*>(context + 24) + 32) + 16i64) = result) {
            --*reinterpret_cast<uint32_t*>(context + 24);
            **reinterpret_cast<uint32_t**>(context + 8i64 *
                *reinterpret_cast<int32_t*>(context + 24) + 32) =
                *reinterpret_cast<uint32_t*>(context + 16 *
                    (*reinterpret_cast<int32_t*>(context + 24) + 4i64));
            *reinterpret_cast<uint32_t*>(
                *reinterpret_cast<uint64_t*>(context + 8i64 *
                    *reinterpret_cast<int32_t*>(context + 24) + 32) + 8i64) =
                *reinterpret_cast<uint32_t*>(context + 16i64 *
                    *reinterpret_cast<int32_t*>(context + 24) + 68);
            result = *reinterpret_cast<uint32_t*>(context + 16i64 *
                *reinterpret_cast<int32_t*>(context + 24) + 72);
        }
        --*reinterpret_cast<uint32_t*>(context + 24);
    }
};

using NativeHandler = void(__cdecl*)(NativeContext* context);

bool SafeCall(NativeHandler handler, NativeContext* context) {
    __try {
        handler(context);
        return true;
    } __except (EXCEPTION_EXECUTE_HANDLER) {
        return false;
    }
}

#ifndef RDR2_WASM_TEST
bool InvokeGameNative(uint64_t hash,
                      std::initializer_list<uint64_t> arguments) {
    const auto getNativeAddress = GetNativeAddrFn();
    if (!getNativeAddress) return false;

    const uintptr_t address = getNativeAddress(hash);
    if (!address) return false;

    NativeContext context;
    context.Reset();
    for (const uint64_t argument : arguments) {
        if (!context.Push(argument)) return false;
    }
    return SafeCall(reinterpret_cast<NativeHandler>(address), &context);
}
#endif

void SafeCopyResults(NativeContext* context) {
    __try {
        context->CopyResults();
    } __except (EXCEPTION_EXECUTE_HANDLER) {
    }
}

struct GuestMemory {
    wasmtime_context_t* context = nullptr;
    uint8_t* data = nullptr;
    size_t size = 0;

    bool Range(int32_t offset, size_t length, uint8_t*& out) const {
        if (offset < 0) return false;
        const size_t start = static_cast<uint32_t>(offset);
        if (start > size || length > size - start) return false;
        out = data + start;
        return true;
    }
};

bool GetGuestMemory(wasmtime_caller_t* caller, GuestMemory& out) {
    wasmtime_extern_t memory;
    if (!wasmtime_caller_export_get(caller, "memory", 6, &memory) ||
        memory.kind != WASMTIME_EXTERN_MEMORY) {
        return false;
    }

    out.context = wasmtime_caller_context(caller);
    out.data = wasmtime_memory_data(out.context, &memory.of.memory);
    out.size = wasmtime_memory_data_size(out.context, &memory.of.memory);
    return out.data != nullptr;
}

std::string_view GuestString(wasmtime_caller_t* caller, int32_t offset,
                             int32_t length, bool& valid) {
    valid = false;
    if (length < 0 || static_cast<size_t>(length) > MaxGuestString) return {};

    GuestMemory memory;
    uint8_t* bytes = nullptr;
    if (!GetGuestMemory(caller, memory) ||
        !memory.Range(offset, static_cast<size_t>(length), bytes)) {
        return {};
    }

    valid = true;
    return { reinterpret_cast<const char*>(bytes), static_cast<size_t>(length) };
}

uint32_t JoaatHash(std::string_view value) {
    uint32_t hash = 0;
    for (unsigned char character : value) {
        hash += static_cast<unsigned char>(std::tolower(character));
        hash += hash << 10;
        hash ^= hash >> 6;
    }
    hash += hash << 3;
    hash ^= hash >> 11;
    hash += hash << 15;
    return hash;
}

wasm_functype_t* MakeFunctionType(
    std::initializer_list<wasm_valkind_t> parameters,
    std::initializer_list<wasm_valkind_t> results) {
    wasm_valtype_vec_t parameterTypes;
    wasm_valtype_vec_new_uninitialized(&parameterTypes, parameters.size());
    size_t index = 0;
    for (auto kind : parameters) {
        parameterTypes.data[index++] = wasm_valtype_new(kind);
    }

    wasm_valtype_vec_t resultTypes;
    wasm_valtype_vec_new_uninitialized(&resultTypes, results.size());
    index = 0;
    for (auto kind : results) {
        resultTypes.data[index++] = wasm_valtype_new(kind);
    }
    return wasm_functype_new(&parameterTypes, &resultTypes);
}

bool Define(wasmtime_linker_t* linker, Mod& mod, const char* name,
            std::initializer_list<wasm_valkind_t> parameters,
            std::initializer_list<wasm_valkind_t> results,
            wasmtime_func_callback_t callback) {
    wasm_functype_t* type = MakeFunctionType(parameters, results);
    if (!type) {
        spdlog::error("[WASM:{}] Failed to create import type for {}",
                      mod.Manifest().name, name);
        return false;
    }

    auto* error = wasmtime_linker_define_func(
        linker, ImportModule, std::strlen(ImportModule), name, std::strlen(name),
        type, callback, &mod, nullptr);
    wasm_functype_delete(type);
    if (!error) return true;

    spdlog::error("[WASM:{}] Failed to define import '{}': {}",
                  mod.Manifest().name, name, TakeError(error));
    return false;
}

wasm_trap_t* HostLog(void* environment, wasmtime_caller_t* caller,
                     const wasmtime_val_t* args, size_t,
                     wasmtime_val_t*, size_t) {
    auto& mod = *static_cast<Mod*>(environment);
    bool valid = false;
    auto message = GuestString(caller, args[1].of.i32, args[2].of.i32, valid);
    if (!valid) {
        spdlog::warn("[WASM:{}] Ignored a log message with an invalid range",
                     mod.Manifest().name);
        return nullptr;
    }

    const std::string text(message);
    switch (args[0].of.i32) {
        case 1: spdlog::warn("[WASM:{}] {}", mod.Manifest().name, text); break;
        case 2: spdlog::error("[WASM:{}] {}", mod.Manifest().name, text); break;
        case 3: spdlog::debug("[WASM:{}] {}", mod.Manifest().name, text); break;
        default: spdlog::info("[WASM:{}] {}", mod.Manifest().name, text); break;
    }
    return nullptr;
}

wasm_trap_t* HostJoaat(void*, wasmtime_caller_t* caller,
                       const wasmtime_val_t* args, size_t,
                       wasmtime_val_t* results, size_t) {
    bool valid = false;
    auto value = GuestString(caller, args[0].of.i32, args[1].of.i32, valid);
    results[0].kind = WASMTIME_I32;
    results[0].of.i32 = valid ? static_cast<int32_t>(JoaatHash(value)) : 0;
    return nullptr;
}

wasm_trap_t* HostModInfo(void* environment, wasmtime_caller_t* caller,
                         const wasmtime_val_t* args, size_t,
                         wasmtime_val_t* results, size_t) {
    const auto& manifest = static_cast<Mod*>(environment)->Manifest();
    const std::string* value = nullptr;
    const std::string path = manifest.modPath.string();
    switch (args[0].of.i32) {
        case 0: value = &manifest.name; break;
        case 1: value = &manifest.version; break;
        case 2: value = &manifest.author; break;
        case 3: value = &manifest.description; break;
        case 4: value = &path; break;
        default: break;
    }

    int32_t length = value ? static_cast<int32_t>(value->size()) : -1;
    const int32_t capacity = args[2].of.i32;
    if (value && capacity > 0) {
        GuestMemory memory;
        uint8_t* destination = nullptr;
        if (!GetGuestMemory(caller, memory) ||
            !memory.Range(args[1].of.i32, static_cast<size_t>(capacity), destination)) {
            length = -1;
        } else {
            const size_t copyLength = std::min(value->size(),
                                               static_cast<size_t>(capacity - 1));
            std::memcpy(destination, value->data(), copyLength);
            destination[copyLength] = 0;
        }
    }

    results[0].kind = WASMTIME_I32;
    results[0].of.i32 = length;
    return nullptr;
}

struct ScratchBuffer {
    int32_t guestOffset;
    std::vector<uint8_t> bytes;
};

wasm_trap_t* HostNativeInvoke(void* environment, wasmtime_caller_t* caller,
                              const wasmtime_val_t* args, size_t,
                              wasmtime_val_t* results, size_t) {
    auto& mod = *static_cast<Mod*>(environment);
    auto finish = [&](NativeStatus status) {
        results[0].kind = WASMTIME_I32;
        results[0].of.i32 = static_cast<int32_t>(status);
        return static_cast<wasm_trap_t*>(nullptr);
    };

    auto getNativeAddress = GetNativeAddrFn();
    if (!getNativeAddress) return finish(NativeStatus::BridgeUnavailable);

    const int32_t argumentCount = args[2].of.i32;
    if (argumentCount < 0 || argumentCount > 24) {
        return finish(NativeStatus::TooManyArguments);
    }

    GuestMemory memory;
    uint8_t* argumentBytes = nullptr;
    uint8_t* resultBytes = nullptr;
    const size_t argumentsSize = static_cast<size_t>(argumentCount) *
                                 sizeof(GuestNativeArg);
    if (!GetGuestMemory(caller, memory) ||
        !memory.Range(args[1].of.i32, argumentsSize, argumentBytes) ||
        !memory.Range(args[3].of.i32, sizeof(GuestNativeResult), resultBytes)) {
        return finish(NativeStatus::InvalidMemory);
    }

    const uint64_t hash = static_cast<uint64_t>(args[0].of.i64);
    const uintptr_t address = getNativeAddress(hash);
    if (!address) return finish(NativeStatus::NotFound);

    NativeContext nativeContext;
    nativeContext.Reset();
    std::vector<std::string> strings;
    std::vector<ScratchBuffer> buffers;
    strings.reserve(argumentCount);
    buffers.reserve(argumentCount);

    for (int32_t index = 0; index < argumentCount; ++index) {
        GuestNativeArg argument{};
        std::memcpy(&argument,
                    argumentBytes + static_cast<size_t>(index) * sizeof(argument),
                    sizeof(argument));

        switch (static_cast<NativeArgKind>(argument.kind)) {
            case NativeArgKind::Raw:
                if (!nativeContext.Push(argument.value))
                    return finish(NativeStatus::TooManyArguments);
                break;

            case NativeArgKind::Float:
                if (!nativeContext.Push(static_cast<uint32_t>(argument.value)))
                    return finish(NativeStatus::TooManyArguments);
                break;

            case NativeArgKind::Utf8: {
                uint8_t* source = nullptr;
                if (argument.data > MaxGuestString ||
                    !memory.Range(static_cast<int32_t>(argument.value),
                                  argument.data, source)) {
                    return finish(NativeStatus::InvalidMemory);
                }
                strings.emplace_back(reinterpret_cast<char*>(source), argument.data);
                if (!nativeContext.Push(reinterpret_cast<uint64_t>(strings.back().c_str())))
                    return finish(NativeStatus::TooManyArguments);
                break;
            }

            case NativeArgKind::Vector3: {
                const uint32_t x = argument.data;
                const uint32_t y = static_cast<uint32_t>(argument.value);
                const uint32_t z = static_cast<uint32_t>(argument.value >> 32);
                if (!nativeContext.Push(x) || !nativeContext.Push(y) ||
                    !nativeContext.Push(z)) {
                    return finish(NativeStatus::TooManyArguments);
                }
                break;
            }

            case NativeArgKind::InOutBuffer:
            case NativeArgKind::OutBuffer: {
                if (argument.data == 0 || argument.data > MaxGuestString) {
                    return finish(NativeStatus::InvalidArgument);
                }
                uint8_t* guestBuffer = nullptr;
                if (!memory.Range(static_cast<int32_t>(argument.value),
                                  argument.data, guestBuffer)) {
                    return finish(NativeStatus::InvalidMemory);
                }
                buffers.push_back({ static_cast<int32_t>(argument.value),
                                    std::vector<uint8_t>(argument.data) });
                if (static_cast<NativeArgKind>(argument.kind) ==
                    NativeArgKind::InOutBuffer) {
                    std::memcpy(buffers.back().bytes.data(), guestBuffer,
                                argument.data);
                }
                if (!nativeContext.Push(reinterpret_cast<uint64_t>(
                        buffers.back().bytes.data()))) {
                    return finish(NativeStatus::TooManyArguments);
                }
                break;
            }

            default:
                return finish(NativeStatus::InvalidArgument);
        }
    }

    if (!SafeCall(reinterpret_cast<NativeHandler>(address), &nativeContext)) {
        spdlog::error(
            "[WASM:{}] Game native 0x{:016X} raised an exception with {} argument(s)",
            mod.Manifest().name, hash, argumentCount);
        return finish(NativeStatus::Crashed);
    }
    SafeCopyResults(&nativeContext);

    for (const auto& buffer : buffers) {
        uint8_t* destination = nullptr;
        if (!memory.Range(buffer.guestOffset, buffer.bytes.size(), destination)) {
            return finish(NativeStatus::InvalidMemory);
        }
        std::memcpy(destination, buffer.bytes.data(), buffer.bytes.size());
    }

    GuestNativeResult nativeResult{};
    nativeResult.value = *nativeContext.retVal;
    nativeResult.vector[0] = *reinterpret_cast<float*>(
        reinterpret_cast<uintptr_t>(nativeContext.retVal) + 0);
    nativeResult.vector[1] = *reinterpret_cast<float*>(
        reinterpret_cast<uintptr_t>(nativeContext.retVal) + 8);
    nativeResult.vector[2] = *reinterpret_cast<float*>(
        reinterpret_cast<uintptr_t>(nativeContext.retVal) + 16);
    std::memcpy(resultBytes, &nativeResult, sizeof(nativeResult));
    return finish(NativeStatus::Ok);
}

wasm_trap_t* HostNativeStringRead(void*, wasmtime_caller_t* caller,
                                  const wasmtime_val_t* args, size_t,
                                  wasmtime_val_t* results, size_t) {
    const auto pointer = static_cast<uint64_t>(args[0].of.i64);
    const int32_t capacity = args[2].of.i32;
    int32_t result = -1;

    if (pointer && capacity >= 0) {
        __try {
            const char* source = reinterpret_cast<const char*>(pointer);
            size_t length = 0;
            while (length < MaxGuestString && source[length] != '\0') ++length;
            if (length < MaxGuestString) {
                result = static_cast<int32_t>(length);
                if (capacity > 0) {
                    GuestMemory memory;
                    uint8_t* destination = nullptr;
                    if (!GetGuestMemory(caller, memory) ||
                        !memory.Range(args[1].of.i32,
                                      static_cast<size_t>(capacity), destination)) {
                        result = -1;
                    } else {
                        const size_t copyLength = std::min(
                            length, static_cast<size_t>(capacity - 1));
                        std::memcpy(destination, source, copyLength);
                        destination[copyLength] = 0;
                    }
                }
            }
        } __except (EXCEPTION_EXECUTE_HANDLER) {
            result = -1;
        }
    }

    results[0].kind = WASMTIME_I32;
    results[0].of.i32 = result;
    return nullptr;
}

wasm_trap_t* HostGlobalGetInt(void*, wasmtime_caller_t*,
                              const wasmtime_val_t* args, size_t,
                              wasmtime_val_t* results, size_t) {
    int32_t value = 0;
    if (auto getGlobal = GetGlobalPtrFn()) {
        __try {
            if (void* pointer = getGlobal(static_cast<uint32_t>(args[0].of.i32)))
                value = *static_cast<int32_t*>(pointer);
        } __except (EXCEPTION_EXECUTE_HANDLER) {
            value = 0;
        }
    }
    results[0].kind = WASMTIME_I32;
    results[0].of.i32 = value;
    return nullptr;
}

wasm_trap_t* HostGlobalSetInt(void*, wasmtime_caller_t*,
                              const wasmtime_val_t* args, size_t,
                              wasmtime_val_t*, size_t) {
    if (auto getGlobal = GetGlobalPtrFn()) {
        __try {
            if (void* pointer = getGlobal(static_cast<uint32_t>(args[0].of.i32)))
                *static_cast<int32_t*>(pointer) = args[1].of.i32;
        } __except (EXCEPTION_EXECUTE_HANDLER) {
        }
    }
    return nullptr;
}

wasm_trap_t* HostGlobalGetFloat(void*, wasmtime_caller_t*,
                                const wasmtime_val_t* args, size_t,
                                wasmtime_val_t* results, size_t) {
    float value = 0.0f;
    if (auto getGlobal = GetGlobalPtrFn()) {
        __try {
            if (void* pointer = getGlobal(static_cast<uint32_t>(args[0].of.i32)))
                value = *static_cast<float*>(pointer);
        } __except (EXCEPTION_EXECUTE_HANDLER) {
            value = 0.0f;
        }
    }
    results[0].kind = WASMTIME_F32;
    results[0].of.f32 = std::bit_cast<uint32_t>(value);
    return nullptr;
}

wasm_trap_t* HostGlobalSetFloat(void*, wasmtime_caller_t*,
                                const wasmtime_val_t* args, size_t,
                                wasmtime_val_t*, size_t) {
    if (auto getGlobal = GetGlobalPtrFn()) {
        __try {
            if (void* pointer = getGlobal(static_cast<uint32_t>(args[0].of.i32)))
                *static_cast<float*>(pointer) = std::bit_cast<float>(args[1].of.f32);
        } __except (EXCEPTION_EXECUTE_HANDLER) {
        }
    }
    return nullptr;
}

wasm_trap_t* HostGameTime(void*, wasmtime_caller_t*, const wasmtime_val_t*,
                          size_t, wasmtime_val_t* results, size_t) {
    results[0].kind = WASMTIME_I32;
    results[0].of.i32 = static_cast<int32_t>(GetTickCount());
    return nullptr;
}

wasm_trap_t* HostIsKeyPressed(void*, wasmtime_caller_t*,
                              const wasmtime_val_t* args, size_t,
                              wasmtime_val_t* results, size_t) {
    const int32_t key = args[0].of.i32;
    results[0].kind = WASMTIME_I32;
    results[0].of.i32 = key >= 0 && key < 256 && g_keyStates[key] ? 1 : 0;
    return nullptr;
}

wasm_trap_t* HostIsKeyJustPressed(void*, wasmtime_caller_t*,
                                  const wasmtime_val_t* args, size_t,
                                  wasmtime_val_t* results, size_t) {
    const int32_t key = args[0].of.i32;
    results[0].kind = WASMTIME_I32;
    results[0].of.i32 = key >= 0 && key < 256 && g_keyStates[key] &&
                        !g_previousKeyStates[key] ? 1 : 0;
    return nullptr;
}

void SetI32Result(wasmtime_val_t* results, int32_t value) {
    results[0].kind = WASMTIME_I32;
    results[0].of.i32 = value;
}

std::string BindingOwnerName(const Mod& mod) {
    std::string directory = mod.Manifest().modPath.filename().string();
    if (directory.empty()) directory = mod.Manifest().name;
    return directory + "/" + mod.Manifest().name;
}

wasm_trap_t* HostInputRegisterBinding(
    void* environment, wasmtime_caller_t* caller,
    const wasmtime_val_t* args, size_t, wasmtime_val_t* results, size_t) {
    bool idValid = false;
    bool descriptionValid = false;
    bool mapperValid = false;
    bool parameterValid = false;
    const auto id = GuestString(caller, args[0].of.i32, args[1].of.i32,
                                idValid);
    const auto description = GuestString(caller, args[2].of.i32,
                                         args[3].of.i32, descriptionValid);
    const auto mapper = GuestString(caller, args[4].of.i32, args[5].of.i32,
                                    mapperValid);
    const auto parameter = GuestString(caller, args[6].of.i32,
                                       args[7].of.i32, parameterValid);
    if (!idValid || !descriptionValid || !mapperValid || !parameterValid) {
        SetI32Result(results, static_cast<int32_t>(
            rdr2::input::BindingStatus::InvalidArgument));
        return nullptr;
    }

    auto& mod = *static_cast<Mod*>(environment);
    const auto registration = rdr2::input::InputBindingManager::Instance()
        .Register(&mod, BindingOwnerName(mod), std::string(id),
                  std::string(description), std::string(mapper),
                  std::string(parameter));
    if (registration.status == rdr2::input::BindingStatus::Ok ||
        registration.status == rdr2::input::BindingStatus::AlreadyRegistered) {
        SetI32Result(results, registration.handle);
    } else {
        SetI32Result(results, static_cast<int32_t>(registration.status));
    }
    return nullptr;
}

wasm_trap_t* HostInputUnregisterBinding(
    void* environment, wasmtime_caller_t*, const wasmtime_val_t* args, size_t,
    wasmtime_val_t* results, size_t) {
    const auto status = rdr2::input::InputBindingManager::Instance().Unregister(
        environment, args[0].of.i32);
    SetI32Result(results, static_cast<int32_t>(status));
    return nullptr;
}

wasm_trap_t* HostInputPollBindingEvent(
    void* environment, wasmtime_caller_t*, const wasmtime_val_t* args, size_t,
    wasmtime_val_t* results, size_t) {
    auto& manager = rdr2::input::InputBindingManager::Instance();
    std::string mapper;
    std::string parameter;
    const auto status = manager.GetMapping(environment, args[0].of.i32,
                                           mapper, parameter);
    if (status != rdr2::input::BindingStatus::Ok) {
        SetI32Result(results, static_cast<int32_t>(status));
    } else {
        SetI32Result(results, static_cast<int32_t>(
            manager.PollEvent(environment, args[0].of.i32)));
    }
    return nullptr;
}

wasm_trap_t* HostInputIsBindingDown(
    void* environment, wasmtime_caller_t*, const wasmtime_val_t* args, size_t,
    wasmtime_val_t* results, size_t) {
    SetI32Result(results,
        rdr2::input::InputBindingManager::Instance().IsDown(
            environment, args[0].of.i32));
    return nullptr;
}

wasm_trap_t* HostInputGetBindingParameter(
    void* environment, wasmtime_caller_t* caller,
    const wasmtime_val_t* args, size_t, wasmtime_val_t* results, size_t) {
    std::string mapper;
    std::string parameter;
    const auto status = rdr2::input::InputBindingManager::Instance().GetMapping(
        environment, args[0].of.i32, mapper, parameter);
    if (status != rdr2::input::BindingStatus::Ok) {
        SetI32Result(results, static_cast<int32_t>(status));
        return nullptr;
    }

    const int32_t capacity = args[2].of.i32;
    if (capacity < 0) {
        SetI32Result(results, static_cast<int32_t>(
            rdr2::input::BindingStatus::InvalidArgument));
        return nullptr;
    }
    if (capacity > 0) {
        GuestMemory memory;
        uint8_t* destination = nullptr;
        if (!GetGuestMemory(caller, memory) ||
            !memory.Range(args[1].of.i32, static_cast<size_t>(capacity),
                          destination)) {
            SetI32Result(results, static_cast<int32_t>(
                rdr2::input::BindingStatus::InvalidArgument));
            return nullptr;
        }
        const size_t length = std::min(
            parameter.size(), static_cast<size_t>(capacity - 1));
        std::memcpy(destination, parameter.data(), length);
        destination[length] = 0;
    }
    SetI32Result(results, static_cast<int32_t>(parameter.size()));
    return nullptr;
}

wasm_trap_t* HostInputSetBinding(
    void* environment, wasmtime_caller_t* caller,
    const wasmtime_val_t* args, size_t, wasmtime_val_t* results, size_t) {
    bool mapperValid = false;
    bool parameterValid = false;
    const auto mapper = GuestString(caller, args[1].of.i32, args[2].of.i32,
                                    mapperValid);
    const auto parameter = GuestString(caller, args[3].of.i32,
                                       args[4].of.i32, parameterValid);
    if (!mapperValid || !parameterValid) {
        SetI32Result(results, static_cast<int32_t>(
            rdr2::input::BindingStatus::InvalidArgument));
        return nullptr;
    }
    const auto status = rdr2::input::InputBindingManager::Instance().SetMapping(
        environment, args[0].of.i32, std::string(mapper),
        std::string(parameter));
    SetI32Result(results, static_cast<int32_t>(status));
    return nullptr;
}

wasm_trap_t* HostInputResetBinding(
    void* environment, wasmtime_caller_t*, const wasmtime_val_t* args, size_t,
    wasmtime_val_t* results, size_t) {
    const auto status = rdr2::input::InputBindingManager::Instance().ResetMapping(
        environment, args[0].of.i32);
    SetI32Result(results, static_cast<int32_t>(status));
    return nullptr;
}

#ifndef RDR2_WASM_TEST
std::uint32_t CursorReferencesOwnedBy(Mod& mod) {
    std::lock_guard lock(g_cursorOwnerMutex);
    const auto owner = g_cursorOwners.find(&mod);
    return owner == g_cursorOwners.end() ? 0 : owner->second;
}

void ReleaseCursorReferencesOwnedBy(Mod& mod) {
    std::uint32_t references = 0;
    {
        std::lock_guard lock(g_cursorOwnerMutex);
        const auto owner = g_cursorOwners.find(&mod);
        if (owner == g_cursorOwners.end()) return;
        references = owner->second;
        g_cursorOwners.erase(owner);
    }

    while (references-- != 0)
        rdr2::input::GameInputHook::ReleaseCursorVisibility();
}

wasm_trap_t* HostWebViewOpen(void* environment, wasmtime_caller_t* caller,
                             const wasmtime_val_t* args, size_t,
                             wasmtime_val_t* results, size_t) {
    bool valid = false;
    auto url = GuestString(caller, args[0].of.i32, args[1].of.i32, valid);
    const int32_t width = args[2].of.i32;
    const int32_t height = args[3].of.i32;
    if (!valid || width < 0 || height < 0) {
        SetI32Result(results, static_cast<int32_t>(
            rdr2::webview::OverlayStatus::InvalidArgument));
        return nullptr;
    }

    const auto status = rdr2::webview::WebViewOverlay::Instance().Open(
        std::string(url), static_cast<uint32_t>(width),
        static_cast<uint32_t>(height));
    if (status == rdr2::webview::OverlayStatus::Ok)
        g_webViewOwner = static_cast<Mod*>(environment);
    SetI32Result(results, static_cast<int32_t>(status));
    return nullptr;
}

wasm_trap_t* HostWebViewClose(void* environment, wasmtime_caller_t*,
                              const wasmtime_val_t*, size_t,
                              wasmtime_val_t*, size_t) {
    ReleaseCursorReferencesOwnedBy(*static_cast<Mod*>(environment));
    rdr2::webview::WebViewOverlay::Instance().Close();
    g_webViewOwner = nullptr;
    g_pendingWebViewMessage.reset();
    return nullptr;
}

wasm_trap_t* HostWebViewIsFocused(void*, wasmtime_caller_t*,
                                  const wasmtime_val_t*, size_t,
                                  wasmtime_val_t* results, size_t) {
    SetI32Result(results,
        rdr2::webview::WebViewOverlay::Instance().IsFocused() ? 1 : 0);
    return nullptr;
}

wasm_trap_t* HostWebViewSetVisible(void*, wasmtime_caller_t*,
                                   const wasmtime_val_t* args, size_t,
                                   wasmtime_val_t* results, size_t) {
    auto& overlay = rdr2::webview::WebViewOverlay::Instance();
    if (!overlay.IsOpen()) {
        SetI32Result(results, -6);
        return nullptr;
    }
    overlay.SetVisible(args[0].of.i32 != 0);
    SetI32Result(results, 0);
    return nullptr;
}

wasm_trap_t* HostWebViewIsVisible(void*, wasmtime_caller_t*,
                                  const wasmtime_val_t*, size_t,
                                  wasmtime_val_t* results, size_t) {
    SetI32Result(results,
        rdr2::webview::WebViewOverlay::Instance().IsVisible() ? 1 : 0);
    return nullptr;
}

wasm_trap_t* HostWebViewShowCursor(void* environment, wasmtime_caller_t*,
                                   const wasmtime_val_t*, size_t,
                                   wasmtime_val_t* results, size_t) {
    auto& overlay = rdr2::webview::WebViewOverlay::Instance();
    if (!overlay.IsOpen()) {
        SetI32Result(results, -6);
        return nullptr;
    }

    auto& mod = *static_cast<Mod*>(environment);
    {
        std::lock_guard lock(g_cursorOwnerMutex);
        auto& references = g_cursorOwners[&mod];
        if (references == static_cast<std::uint32_t>(
                std::numeric_limits<std::int32_t>::max())) {
            SetI32Result(results, -8);
            return nullptr;
        }
        ++references;
    }
    rdr2::input::GameInputHook::AcquireCursorVisibility();
    SetI32Result(results, 0);
    return nullptr;
}

wasm_trap_t* HostWebViewHideCursor(void* environment, wasmtime_caller_t*,
                                   const wasmtime_val_t*, size_t,
                                   wasmtime_val_t* results, size_t) {
    auto& mod = *static_cast<Mod*>(environment);
    {
        std::lock_guard lock(g_cursorOwnerMutex);
        const auto owner = g_cursorOwners.find(&mod);
        if (owner == g_cursorOwners.end() || owner->second == 0) {
            SetI32Result(results, -7);
            return nullptr;
        }
        if (--owner->second == 0) g_cursorOwners.erase(owner);
    }
    rdr2::input::GameInputHook::ReleaseCursorVisibility();
    SetI32Result(results, 0);
    return nullptr;
}

wasm_trap_t* HostWebViewIsCursorVisible(void*, wasmtime_caller_t*,
                                        const wasmtime_val_t*, size_t,
                                        wasmtime_val_t* results, size_t) {
    SetI32Result(results,
        rdr2::input::GameInputHook::CursorVisibilityRequests() != 0 ? 1 : 0);
    return nullptr;
}

wasm_trap_t* HostWebViewCursorRefCount(void* environment, wasmtime_caller_t*,
                                       const wasmtime_val_t*, size_t,
                                       wasmtime_val_t* results, size_t) {
    SetI32Result(results, static_cast<std::int32_t>(CursorReferencesOwnedBy(
        *static_cast<Mod*>(environment))));
    return nullptr;
}

wasm_trap_t* HostWebViewIsOpen(void*, wasmtime_caller_t*,
                               const wasmtime_val_t*, size_t,
                               wasmtime_val_t* results, size_t) {
    SetI32Result(results,
        rdr2::webview::WebViewOverlay::Instance().IsOpen() ? 1 : 0);
    return nullptr;
}

wasm_trap_t* HostWebViewSetFocus(void*, wasmtime_caller_t*,
                                 const wasmtime_val_t* args, size_t,
                                 wasmtime_val_t* results, size_t) {
    auto& overlay = rdr2::webview::WebViewOverlay::Instance();
    if (!overlay.IsOpen()) {
        SetI32Result(results, -6);
        return nullptr;
    }
    overlay.SetFocused(args[0].of.i32 != 0);
    SetI32Result(results, 0);
    return nullptr;
}

wasm_trap_t* HostWebViewNavigate(void*, wasmtime_caller_t* caller,
                                 const wasmtime_val_t* args, size_t,
                                 wasmtime_val_t* results, size_t) {
    bool valid = false;
    auto url = GuestString(caller, args[0].of.i32, args[1].of.i32, valid);
    auto& overlay = rdr2::webview::WebViewOverlay::Instance();
    if (!valid || url.empty()) {
        SetI32Result(results, -1);
        return nullptr;
    }
    if (!overlay.IsOpen()) {
        SetI32Result(results, -6);
        return nullptr;
    }
    overlay.Navigate(std::string(url));
    SetI32Result(results, 0);
    return nullptr;
}

wasm_trap_t* HostWebViewPostJson(void*, wasmtime_caller_t* caller,
                                 const wasmtime_val_t* args, size_t,
                                 wasmtime_val_t* results, size_t) {
    bool valid = false;
    auto json = GuestString(caller, args[0].of.i32, args[1].of.i32, valid);
    auto& overlay = rdr2::webview::WebViewOverlay::Instance();
    if (!valid || json.empty()) {
        SetI32Result(results, -1);
        return nullptr;
    }
    if (!overlay.IsOpen()) {
        SetI32Result(results, -6);
        return nullptr;
    }
    overlay.PostJson(std::string(json));
    SetI32Result(results, 0);
    return nullptr;
}

wasm_trap_t* HostWebViewPollJson(void*, wasmtime_caller_t* caller,
                                 const wasmtime_val_t* args, size_t,
                                 wasmtime_val_t* results, size_t) {
    if (!g_pendingWebViewMessage) {
        std::string message;
        if (!rdr2::webview::WebViewOverlay::Instance().PollMessage(message)) {
            SetI32Result(results, -1);
            return nullptr;
        }
        g_pendingWebViewMessage = std::move(message);
    }

    const size_t length = g_pendingWebViewMessage->size();
    if (length > static_cast<size_t>(std::numeric_limits<int32_t>::max())) {
        g_pendingWebViewMessage.reset();
        SetI32Result(results, -2);
        return nullptr;
    }

    const int32_t capacity = args[1].of.i32;
    if (capacity < 0) {
        SetI32Result(results, -2);
        return nullptr;
    }
    if (capacity == 0 || static_cast<size_t>(capacity) < length) {
        if (length == 0) g_pendingWebViewMessage.reset();
        SetI32Result(results, static_cast<int32_t>(length));
        return nullptr;
    }

    GuestMemory memory;
    uint8_t* destination = nullptr;
    if (!GetGuestMemory(caller, memory) ||
        !memory.Range(args[0].of.i32, length, destination)) {
        SetI32Result(results, -2);
        return nullptr;
    }
    std::memcpy(destination, g_pendingWebViewMessage->data(), length);
    g_pendingWebViewMessage.reset();
    SetI32Result(results, static_cast<int32_t>(length));
    return nullptr;
}
#endif

} // namespace

void PumpGameUiInput() {
#ifndef RDR2_WASM_TEST
    // Use only ordinary RDR2 script natives: no Win32 cursor state, import
    // table, DirectInput device, or protected RAGE function is modified.
    static bool cursorWasRequested = false;
    static bool cursorFailureReported = false;

    if (rdr2::input::GameInputHook::IsBlocked())
        InvokeGameNative(0x5F4B6931816E599B, {0});

    const bool cursorRequested =
        rdr2::input::GameInputHook::CursorVisibilityRequests() != 0;
    if (cursorRequested) {
        if (!InvokeGameNative(0xF12E4CCAF249DC10, {})) {
            if (!cursorFailureReported) {
                cursorFailureReported = true;
                spdlog::error(
                    "[Input] Could not invoke the RDR2 in-game cursor native");
            }
        } else {
            cursorFailureReported = false;
        }
    }

    if (cursorRequested != cursorWasRequested) {
        spdlog::info("[Input] RDR2 in-game cursor {}",
                     cursorRequested ? "enabled" : "disabled");
        cursorWasRequested = cursorRequested;
    }
#endif
}

#ifdef RDR2_WASM_TEST
bool g_testWebViewOpen = false;
bool g_testWebViewVisible = false;
bool g_testWebViewFocused = false;
std::uint32_t g_testCursorReferences = 0;

void SetTestResult(wasmtime_val_t* results, int32_t value) {
    results[0].kind = WASMTIME_I32;
    results[0].of.i32 = value;
}

wasm_trap_t* HostTestWebViewOpen(void*, wasmtime_caller_t*,
                                 const wasmtime_val_t*, size_t,
                                 wasmtime_val_t* results, size_t) {
    g_testWebViewOpen = true;
    g_testWebViewVisible = true;
    SetTestResult(results, 0);
    return nullptr;
}

wasm_trap_t* HostTestWebViewClose(void*, wasmtime_caller_t*,
                                  const wasmtime_val_t*, size_t,
                                  wasmtime_val_t*, size_t) {
    g_testWebViewOpen = false;
    g_testWebViewVisible = false;
    g_testWebViewFocused = false;
    g_testCursorReferences = 0;
    return nullptr;
}

wasm_trap_t* HostTestWebViewIsOpen(void*, wasmtime_caller_t*,
                                   const wasmtime_val_t*, size_t,
                                   wasmtime_val_t* results, size_t) {
    SetTestResult(results, g_testWebViewOpen ? 1 : 0);
    return nullptr;
}

wasm_trap_t* HostTestWebViewSetVisible(void*, wasmtime_caller_t*,
                                       const wasmtime_val_t* args, size_t,
                                       wasmtime_val_t* results, size_t) {
    if (!g_testWebViewOpen) {
        SetTestResult(results, -6);
        return nullptr;
    }
    g_testWebViewVisible = args[0].of.i32 != 0;
    SetTestResult(results, 0);
    return nullptr;
}

wasm_trap_t* HostTestWebViewIsVisible(void*, wasmtime_caller_t*,
                                      const wasmtime_val_t*, size_t,
                                      wasmtime_val_t* results, size_t) {
    SetTestResult(results, g_testWebViewVisible ? 1 : 0);
    return nullptr;
}

wasm_trap_t* HostTestWebViewSetFocus(void*, wasmtime_caller_t*,
                                     const wasmtime_val_t* args, size_t,
                                     wasmtime_val_t* results, size_t) {
    if (!g_testWebViewOpen) {
        SetTestResult(results, -6);
        return nullptr;
    }
    g_testWebViewFocused = args[0].of.i32 != 0;
    SetTestResult(results, 0);
    return nullptr;
}

wasm_trap_t* HostTestWebViewIsFocused(void*, wasmtime_caller_t*,
                                      const wasmtime_val_t*, size_t,
                                      wasmtime_val_t* results, size_t) {
    SetTestResult(results, g_testWebViewFocused ? 1 : 0);
    return nullptr;
}

wasm_trap_t* HostTestWebViewShowCursor(void*, wasmtime_caller_t*,
                                       const wasmtime_val_t*, size_t,
                                       wasmtime_val_t* results, size_t) {
    if (!g_testWebViewOpen) {
        SetTestResult(results, -6);
        return nullptr;
    }
    ++g_testCursorReferences;
    SetTestResult(results, 0);
    return nullptr;
}

wasm_trap_t* HostTestWebViewHideCursor(void*, wasmtime_caller_t*,
                                       const wasmtime_val_t*, size_t,
                                       wasmtime_val_t* results, size_t) {
    if (g_testCursorReferences == 0) {
        SetTestResult(results, -7);
        return nullptr;
    }
    --g_testCursorReferences;
    SetTestResult(results, 0);
    return nullptr;
}

wasm_trap_t* HostTestWebViewCursorStatus(void*, wasmtime_caller_t*,
                                         const wasmtime_val_t*, size_t,
                                         wasmtime_val_t* results, size_t) {
    SetTestResult(results, g_testCursorReferences != 0 ? 1 : 0);
    return nullptr;
}

wasm_trap_t* HostTestWebViewCursorRefCount(void*, wasmtime_caller_t*,
                                           const wasmtime_val_t*, size_t,
                                           wasmtime_val_t* results, size_t) {
    SetTestResult(results, static_cast<int32_t>(g_testCursorReferences));
    return nullptr;
}

wasm_trap_t* HostTestWebViewCommand(void*, wasmtime_caller_t*,
                                    const wasmtime_val_t*, size_t,
                                    wasmtime_val_t* results, size_t) {
    SetTestResult(results, g_testWebViewOpen ? 0 : -6);
    return nullptr;
}

wasm_trap_t* HostTestWebViewPoll(void*, wasmtime_caller_t*,
                                 const wasmtime_val_t*, size_t,
                                 wasmtime_val_t* results, size_t) {
    SetTestResult(results, -1);
    return nullptr;
}
#endif

bool DefineAll(Mod& mod, wasmtime_linker_t* linker) {
    bool valid = true;
    valid &= Define(linker, mod, "log", { WASM_I32, WASM_I32, WASM_I32 }, {}, HostLog);
    valid &= Define(linker, mod, "joaat", { WASM_I32, WASM_I32 }, { WASM_I32 }, HostJoaat);
    valid &= Define(linker, mod, "mod_info", { WASM_I32, WASM_I32, WASM_I32 }, { WASM_I32 }, HostModInfo);
    valid &= Define(linker, mod, "native_invoke", { WASM_I64, WASM_I32, WASM_I32, WASM_I32 }, { WASM_I32 }, HostNativeInvoke);
    valid &= Define(linker, mod, "native_string_read", { WASM_I64, WASM_I32, WASM_I32 }, { WASM_I32 }, HostNativeStringRead);
    valid &= Define(linker, mod, "global_get_i32", { WASM_I32 }, { WASM_I32 }, HostGlobalGetInt);
    valid &= Define(linker, mod, "global_set_i32", { WASM_I32, WASM_I32 }, {}, HostGlobalSetInt);
    valid &= Define(linker, mod, "global_get_f32", { WASM_I32 }, { WASM_F32 }, HostGlobalGetFloat);
    valid &= Define(linker, mod, "global_set_f32", { WASM_I32, WASM_F32 }, {}, HostGlobalSetFloat);
    valid &= Define(linker, mod, "game_time", {}, { WASM_I32 }, HostGameTime);
    valid &= Define(linker, mod, "is_key_pressed", { WASM_I32 }, { WASM_I32 }, HostIsKeyPressed);
    valid &= Define(linker, mod, "is_key_just_pressed", { WASM_I32 }, { WASM_I32 }, HostIsKeyJustPressed);
    valid &= Define(linker, mod, "input_register_binding",
                    { WASM_I32, WASM_I32, WASM_I32, WASM_I32,
                      WASM_I32, WASM_I32, WASM_I32, WASM_I32 },
                    { WASM_I32 }, HostInputRegisterBinding);
    valid &= Define(linker, mod, "input_unregister_binding", { WASM_I32 },
                    { WASM_I32 }, HostInputUnregisterBinding);
    valid &= Define(linker, mod, "input_poll_binding_event", { WASM_I32 },
                    { WASM_I32 }, HostInputPollBindingEvent);
    valid &= Define(linker, mod, "input_is_binding_down", { WASM_I32 },
                    { WASM_I32 }, HostInputIsBindingDown);
    valid &= Define(linker, mod, "input_get_binding_parameter",
                    { WASM_I32, WASM_I32, WASM_I32 }, { WASM_I32 },
                    HostInputGetBindingParameter);
    valid &= Define(linker, mod, "input_set_binding",
                    { WASM_I32, WASM_I32, WASM_I32, WASM_I32, WASM_I32 },
                    { WASM_I32 }, HostInputSetBinding);
    valid &= Define(linker, mod, "input_reset_binding", { WASM_I32 },
                    { WASM_I32 }, HostInputResetBinding);
#ifndef RDR2_WASM_TEST
    valid &= Define(linker, mod, "webview_open",
                    { WASM_I32, WASM_I32, WASM_I32, WASM_I32 },
                    { WASM_I32 }, HostWebViewOpen);
    valid &= Define(linker, mod, "webview_close", {}, {}, HostWebViewClose);
    valid &= Define(linker, mod, "webview_is_open", {}, { WASM_I32 },
                    HostWebViewIsOpen);
    valid &= Define(linker, mod, "webview_set_visible", { WASM_I32 },
                    { WASM_I32 }, HostWebViewSetVisible);
    valid &= Define(linker, mod, "webview_is_visible", {}, { WASM_I32 },
                    HostWebViewIsVisible);
    valid &= Define(linker, mod, "webview_set_focus", { WASM_I32 },
                    { WASM_I32 }, HostWebViewSetFocus);
    valid &= Define(linker, mod, "webview_is_focused", {}, { WASM_I32 },
                    HostWebViewIsFocused);
    valid &= Define(linker, mod, "webview_show_cursor", {}, { WASM_I32 },
                    HostWebViewShowCursor);
    valid &= Define(linker, mod, "webview_hide_cursor", {}, { WASM_I32 },
                    HostWebViewHideCursor);
    valid &= Define(linker, mod, "webview_is_cursor_visible", {},
                    { WASM_I32 }, HostWebViewIsCursorVisible);
    valid &= Define(linker, mod, "webview_cursor_ref_count", {},
                    { WASM_I32 }, HostWebViewCursorRefCount);
    valid &= Define(linker, mod, "webview_navigate", { WASM_I32, WASM_I32 },
                    { WASM_I32 }, HostWebViewNavigate);
    valid &= Define(linker, mod, "webview_post_json", { WASM_I32, WASM_I32 },
                    { WASM_I32 }, HostWebViewPostJson);
    valid &= Define(linker, mod, "webview_poll_json", { WASM_I32, WASM_I32 },
                    { WASM_I32 }, HostWebViewPollJson);
#else
    // The Javy plugin imports its entire bridge eagerly. Headless smoke tests
    // do not create a game window, but still need these imports to instantiate.
    valid &= Define(linker, mod, "webview_open",
                    { WASM_I32, WASM_I32, WASM_I32, WASM_I32 },
                    { WASM_I32 }, HostTestWebViewOpen);
    valid &= Define(linker, mod, "webview_close", {}, {},
                    HostTestWebViewClose);
    valid &= Define(linker, mod, "webview_is_open", {}, { WASM_I32 },
                    HostTestWebViewIsOpen);
    valid &= Define(linker, mod, "webview_set_visible", { WASM_I32 },
                    { WASM_I32 }, HostTestWebViewSetVisible);
    valid &= Define(linker, mod, "webview_is_visible", {}, { WASM_I32 },
                    HostTestWebViewIsVisible);
    valid &= Define(linker, mod, "webview_set_focus", { WASM_I32 },
                    { WASM_I32 }, HostTestWebViewSetFocus);
    valid &= Define(linker, mod, "webview_is_focused", {}, { WASM_I32 },
                    HostTestWebViewIsFocused);
    valid &= Define(linker, mod, "webview_show_cursor", {}, { WASM_I32 },
                    HostTestWebViewShowCursor);
    valid &= Define(linker, mod, "webview_hide_cursor", {}, { WASM_I32 },
                    HostTestWebViewHideCursor);
    valid &= Define(linker, mod, "webview_is_cursor_visible", {},
                    { WASM_I32 }, HostTestWebViewCursorStatus);
    valid &= Define(linker, mod, "webview_cursor_ref_count", {},
                    { WASM_I32 }, HostTestWebViewCursorRefCount);
    valid &= Define(linker, mod, "webview_navigate",
                    { WASM_I32, WASM_I32 }, { WASM_I32 },
                    HostTestWebViewCommand);
    valid &= Define(linker, mod, "webview_post_json",
                    { WASM_I32, WASM_I32 }, { WASM_I32 },
                    HostTestWebViewCommand);
    valid &= Define(linker, mod, "webview_poll_json",
                    { WASM_I32, WASM_I32 }, { WASM_I32 },
                    HostTestWebViewPoll);
#endif
    return valid;
}

void ReleaseOwnedResources(Mod& mod) {
    rdr2::input::InputBindingManager::Instance().ReleaseOwner(&mod);
#ifndef RDR2_WASM_TEST
    ReleaseCursorReferencesOwnedBy(mod);
    if (g_webViewOwner == &mod) {
        g_webViewOwner = nullptr;
        rdr2::webview::WebViewOverlay::Instance().Close();
        g_pendingWebViewMessage.reset();
    }
#else
    (void)mod;
#endif
}

void BuildRageKeyboardStateFromVirtualKeys()
{
    g_rageKeyStates.fill(0);
    for (std::uint32_t virtualKey = 0; virtualKey < 256; ++virtualKey) {
        if (!g_keyStates[virtualKey]) continue;
        std::uint8_t rageKey = 0;
        if (rdr2::input::InputBindingManager::VirtualKeyToRageKey(
                static_cast<std::uint8_t>(virtualKey), rageKey))
            g_rageKeyStates[rageKey] = 1;
    }
}

void PollKeyboard() {
    std::memcpy(g_previousKeyStates, g_keyStates, sizeof(g_keyStates));
    for (int key = 0; key < 256; ++key)
        g_keyStates[key] = (GetAsyncKeyState(key) & 0x8000) ? 1 : 0;
#ifndef RDR2_WASM_TEST
    std::span<std::uint8_t, 256> state(g_rageKeyStates);
    if (!rdr2::input::GameInputHook::CopyKeyboardState(state)) {
        BuildRageKeyboardStateFromVirtualKeys();
    }
#else
    BuildRageKeyboardStateFromVirtualKeys();
#endif
    rdr2::input::InputBindingManager::Instance().UpdateKeyboardState(
        std::span<const std::uint8_t, 256>(g_rageKeyStates));
}

#ifdef RDR2_WASM_TEST
void SetKeyStateForTesting(uint32_t key, bool pressed) {
    if (key >= std::size(g_keyStates)) return;
    std::memcpy(g_previousKeyStates, g_keyStates, sizeof(g_keyStates));
    g_keyStates[key] = pressed ? 1 : 0;
    BuildRageKeyboardStateFromVirtualKeys();
    rdr2::input::InputBindingManager::Instance().UpdateKeyboardState(
        std::span<const std::uint8_t, 256>(g_rageKeyStates));
}
#endif

} // namespace rdr2wasm::bindings
