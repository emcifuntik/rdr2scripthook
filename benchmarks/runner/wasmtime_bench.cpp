#include <wasmtime.h>

#include <algorithm>
#include <charconv>
#include <chrono>
#include <cstdint>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <limits>
#include <memory>
#include <stdexcept>
#include <string>
#include <string_view>
#include <vector>

namespace {

constexpr std::uint32_t BenchmarkAbiVersion = 1;
constexpr std::uint64_t FuelPerCall =
    std::numeric_limits<std::uint64_t>::max() / 4;

struct Options {
    std::filesystem::path module;
    std::string backend;
    std::string workload;
    std::int32_t workloadId = -1;
    std::int32_t iterations = 0;
    std::int64_t seed = 0;
    std::int32_t warmup = 3;
    std::int32_t samples = 15;
    bool fuel = true;
};

std::string TakeError(wasmtime_error_t* error) {
    if (!error) return {};
    wasm_byte_vec_t message;
    wasmtime_error_message(error, &message);
    std::string result(message.data, message.size);
    wasm_byte_vec_delete(&message);
    wasmtime_error_delete(error);
    return result;
}

std::string TakeTrap(wasm_trap_t* trap) {
    if (!trap) return {};
    wasm_byte_vec_t message;
    wasm_trap_message(trap, &message);
    std::string result(message.data, message.size);
    wasm_byte_vec_delete(&message);
    wasm_trap_delete(trap);
    return result;
}

ptrdiff_t CaptureOutput(void* context, const unsigned char* bytes,
                        size_t length) {
    auto& output = *static_cast<std::string*>(context);
    constexpr std::size_t MaxCapturedOutput = 64 * 1024;
    const std::size_t available = MaxCapturedOutput -
        std::min(output.size(), MaxCapturedOutput);
    output.append(reinterpret_cast<const char*>(bytes),
                  std::min(length, available));
    return static_cast<ptrdiff_t>(length);
}

template <typename T>
T ParseInteger(std::string_view value, const char* name) {
    T result{};
    const auto parsed = std::from_chars(value.data(),
                                        value.data() + value.size(), result);
    if (parsed.ec != std::errc{} || parsed.ptr != value.data() + value.size())
        throw std::runtime_error(std::string("Invalid ") + name);
    return result;
}

Options ParseOptions(int argc, char** argv) {
    Options options;
    for (int index = 1; index < argc; index += 2) {
        if (index + 1 >= argc)
            throw std::runtime_error("Arguments must be --name value pairs");
        const std::string_view name(argv[index]);
        const std::string_view value(argv[index + 1]);
        if (name == "--module") options.module = value;
        else if (name == "--backend") options.backend = value;
        else if (name == "--workload") options.workload = value;
        else if (name == "--workload-id")
            options.workloadId = ParseInteger<std::int32_t>(value, "workload id");
        else if (name == "--iterations")
            options.iterations = ParseInteger<std::int32_t>(value, "iterations");
        else if (name == "--seed")
            options.seed = ParseInteger<std::int64_t>(value, "seed");
        else if (name == "--warmup")
            options.warmup = ParseInteger<std::int32_t>(value, "warmup count");
        else if (name == "--samples")
            options.samples = ParseInteger<std::int32_t>(value, "sample count");
        else if (name == "--fuel") {
            if (value == "on") options.fuel = true;
            else if (value == "off") options.fuel = false;
            else throw std::runtime_error("--fuel must be on or off");
        } else {
            throw std::runtime_error("Unknown option: " + std::string(name));
        }
    }
    if (options.module.empty() || options.backend.empty() ||
        options.workload.empty() || options.workloadId < 0 ||
        options.iterations <= 0 || options.warmup < 0 || options.samples <= 0)
        throw std::runtime_error("Missing or invalid benchmark options");
    return options;
}

std::vector<std::uint8_t> ReadFile(const std::filesystem::path& path) {
    std::ifstream stream(path, std::ios::binary);
    if (!stream) throw std::runtime_error("Could not open module: " + path.string());
    return {std::istreambuf_iterator<char>(stream),
            std::istreambuf_iterator<char>()};
}

bool FindFunction(wasmtime_context_t* context,
                  const wasmtime_instance_t& instance,
                  const char* name, bool required, wasmtime_func_t& output) {
    wasmtime_extern_t item{};
    if (!wasmtime_instance_export_get(context, &instance, name,
                                      std::char_traits<char>::length(name),
                                      &item)) {
        if (required)
            throw std::runtime_error(std::string("Missing export: ") + name);
        return false;
    }
    if (item.kind != WASMTIME_EXTERN_FUNC)
        throw std::runtime_error(std::string("Export is not a function: ") + name);
    output = item.of.func;
    return true;
}

void ResetFuel(wasmtime_context_t* context, bool enabled) {
    if (!enabled) return;
    if (auto* error = wasmtime_context_set_fuel(context, FuelPerCall))
        throw std::runtime_error("Could not reset fuel: " + TakeError(error));
}

void CallVoid(wasmtime_context_t* context, const wasmtime_func_t& function,
              bool fuel) {
    ResetFuel(context, fuel);
    wasm_trap_t* trap = nullptr;
    auto* error = wasmtime_func_call(context, &function, nullptr, 0,
                                     nullptr, 0, &trap);
    if (error || trap)
        throw std::runtime_error("Wasm call failed: " + TakeError(error) +
                                 TakeTrap(trap));
}

std::int64_t RunWorkload(wasmtime_context_t* context,
                         const wasmtime_func_t& function,
                         const Options& options,
                         const std::string* guestOutput,
                         std::string_view invocation,
                         std::uint64_t* fuelConsumed = nullptr) {
    ResetFuel(context, options.fuel);
    wasmtime_val_t arguments[3]{};
    arguments[0].kind = WASMTIME_I32;
    arguments[0].of.i32 = options.workloadId;
    arguments[1].kind = WASMTIME_I32;
    arguments[1].of.i32 = options.iterations;
    arguments[2].kind = WASMTIME_I64;
    arguments[2].of.i64 = options.seed;
    wasmtime_val_t result{};
    wasm_trap_t* trap = nullptr;
    auto* error = wasmtime_func_call(context, &function, arguments, 3,
                                     &result, 1, &trap);
    if (error || trap) {
        const std::string output = guestOutput && !guestOutput->empty()
            ? "\nguest output:\n" + *guestOutput
            : std::string{};
        throw std::runtime_error("bench_run failed during " +
                                 std::string(invocation) + ": " +
                                 TakeError(error) +
                                 TakeTrap(trap) + output);
    }
    if (result.kind != WASMTIME_I64)
        throw std::runtime_error("bench_run returned a non-i64 value");
    if (fuelConsumed) {
        *fuelConsumed = 0;
        if (options.fuel) {
            std::uint64_t remaining = 0;
            if (auto* fuelError = wasmtime_context_get_fuel(context, &remaining))
                throw std::runtime_error("Could not read fuel: " +
                                         TakeError(fuelError));
            *fuelConsumed = FuelPerCall - remaining;
        }
    }
    return result.of.i64;
}

std::string Csv(std::string_view value) {
    std::string result = "\"";
    for (const char character : value) {
        if (character == '\"') result += "\"\"";
        else result += character;
    }
    result += '\"';
    return result;
}

} // namespace

int main(int argc, char** argv) try {
    const Options options = ParseOptions(argc, argv);
    const auto bytes = ReadFile(options.module);

    wasm_config_t* rawConfig = wasm_config_new();
    if (!rawConfig) throw std::runtime_error("Could not create Wasmtime config");
    wasmtime_config_consume_fuel_set(rawConfig, options.fuel);
    wasmtime_config_cranelift_opt_level_set(rawConfig, WASMTIME_OPT_LEVEL_SPEED);
    std::unique_ptr<wasm_engine_t, decltype(&wasm_engine_delete)> engine(
        wasm_engine_new_with_config(rawConfig), wasm_engine_delete);
    if (!engine) throw std::runtime_error("Could not create Wasmtime engine");

    std::unique_ptr<wasmtime_store_t, decltype(&wasmtime_store_delete)> store(
        wasmtime_store_new(engine.get(), nullptr, nullptr), wasmtime_store_delete);
    if (!store) throw std::runtime_error("Could not create Wasmtime store");
    wasmtime_context_t* context = wasmtime_store_context(store.get());
    wasmtime_store_limiter(store.get(), 64 * 1024 * 1024, 10'000, 1, 4, 2);

    wasi_config_t* wasi = wasi_config_new();
    if (!wasi) throw std::runtime_error("Could not create WASI config");
    wasm_byte_vec_t stdinBytes;
    wasm_byte_vec_new_empty(&stdinBytes);
    wasi_config_set_stdin_bytes(wasi, &stdinBytes);
    std::string guestOutput;
    wasi_config_set_stdout_custom(wasi, CaptureOutput, &guestOutput, nullptr);
    wasi_config_set_stderr_custom(wasi, CaptureOutput, &guestOutput, nullptr);
    if (auto* error = wasmtime_context_set_wasi(context, wasi))
        throw std::runtime_error("Could not configure WASI: " + TakeError(error));

    wasmtime_module_t* rawModule = nullptr;
    if (auto* error = wasmtime_module_new(engine.get(), bytes.data(),
                                          bytes.size(), &rawModule))
        throw std::runtime_error("Could not compile module: " + TakeError(error));
    std::unique_ptr<wasmtime_module_t, decltype(&wasmtime_module_delete)> module(
        rawModule, wasmtime_module_delete);

    std::unique_ptr<wasmtime_linker_t, decltype(&wasmtime_linker_delete)> linker(
        wasmtime_linker_new(engine.get()), wasmtime_linker_delete);
    if (!linker) throw std::runtime_error("Could not create Wasmtime linker");
    if (auto* error = wasmtime_linker_define_wasi(linker.get()))
        throw std::runtime_error("Could not define WASI: " + TakeError(error));

    ResetFuel(context, options.fuel);
    wasmtime_instance_t instance{};
    wasm_trap_t* trap = nullptr;
    if (auto* error = wasmtime_linker_instantiate(
            linker.get(), context, module.get(), &instance, &trap))
        throw std::runtime_error("Could not instantiate module: " +
                                 TakeError(error) + TakeTrap(trap));
    if (trap)
        throw std::runtime_error("Could not instantiate module: " + TakeTrap(trap));

    wasmtime_func_t constructors{};
    if (FindFunction(context, instance, "__wasm_call_ctors", false,
                     constructors))
        CallVoid(context, constructors, options.fuel);

    // Reactor-style guests initialize their language runtime explicitly.
    // Plain benchmark modules omit this export and need no adapter code.
    wasmtime_func_t initialize{};
    const bool hasInitialize = FindFunction(
        context, instance, "rdr2_init", false, initialize);
    if (hasInitialize)
        CallVoid(context, initialize, options.fuel);

    wasmtime_func_t abi{};
    FindFunction(context, instance, "bench_abi_version", true, abi);
    ResetFuel(context, options.fuel);
    wasmtime_val_t abiResult{};
    trap = nullptr;
    if (auto* error = wasmtime_func_call(context, &abi, nullptr, 0,
                                         &abiResult, 1, &trap))
        throw std::runtime_error("ABI call failed: " + TakeError(error) +
                                 TakeTrap(trap));
    if (trap) throw std::runtime_error("ABI call trapped: " + TakeTrap(trap));
    if (abiResult.kind != WASMTIME_I32 ||
        static_cast<std::uint32_t>(abiResult.of.i32) != BenchmarkAbiVersion)
        throw std::runtime_error("Unsupported benchmark ABI version");

    wasmtime_func_t run{};
    FindFunction(context, instance, "bench_run", true, run);
    wasmtime_func_t collect{};
    const bool hasCollect = FindFunction(context, instance, "bench_collect",
                                         false, collect);
    std::int64_t checksum = 0;
    for (std::int32_t index = 0; index < options.warmup; ++index) {
        if (hasCollect) CallVoid(context, collect, options.fuel);
        const std::string invocation = "warmup #" + std::to_string(index);
        checksum = RunWorkload(context, run, options, &guestOutput,
                               invocation);
    }

    std::cout << "backend,engine,workload,iterations,sample,elapsed_ns,checksum,fuel_consumed\n";
    for (std::int32_t sample = 0; sample < options.samples; ++sample) {
        if (hasCollect) CallVoid(context, collect, options.fuel);
        std::uint64_t fuelConsumed = 0;
        const auto started = std::chrono::steady_clock::now();
        const std::string invocation = "sample #" + std::to_string(sample);
        checksum = RunWorkload(context, run, options, &guestOutput,
                               invocation,
                               &fuelConsumed);
        const auto stopped = std::chrono::steady_clock::now();
        const auto nanoseconds = std::chrono::duration_cast<
            std::chrono::nanoseconds>(stopped - started).count();
        std::cout << Csv(options.backend) << ','
                  << Csv(options.fuel ? "wasmtime-fuel" : "wasmtime") << ','
                  << Csv(options.workload) << ',' << options.iterations << ','
                  << sample << ',' << nanoseconds << ',' << checksum << ','
                  << fuelConsumed << '\n';
    }
    if (!guestOutput.empty())
        std::cerr << "guest output:\n" << guestOutput;
    return 0;
} catch (const std::exception& error) {
    std::cerr << "benchmark runner: " << error.what() << '\n';
    return 1;
}
