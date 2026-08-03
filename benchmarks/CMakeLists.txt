cmake_minimum_required(VERSION 3.21)

find_package(wasmtime CONFIG REQUIRED)

add_executable(wasm_bench_runner
    runner/wasmtime_bench.cpp
)

target_link_libraries(wasm_bench_runner PRIVATE wasmtime::wasmtime)
target_compile_definitions(wasm_bench_runner PRIVATE
    NOMINMAX
    WIN32_LEAN_AND_MEAN
)
set_target_properties(wasm_bench_runner PROPERTIES
    CXX_STANDARD 20
    CXX_STANDARD_REQUIRED ON
    RUNTIME_OUTPUT_DIRECTORY_DEBUG "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Debug"
    RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Release"
)
