cmake_minimum_required(VERSION 3.21)

project(rdr2hook
    VERSION 1.0.0
    LANGUAGES CXX C
    DESCRIPTION "RDR2 Script Hook"
)

# Set C++20 standard globally
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Only Debug and Release
set(CMAKE_CONFIGURATION_TYPES Debug Release CACHE STRING "" FORCE)

# Output directories
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/LIB)
if(NOT CMAKE_RUNTIME_OUTPUT_DIRECTORY)
  set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/BIN)
endif()

# Project paths
set(RDRHOOK_SHARED ${CMAKE_CURRENT_SOURCE_DIR}/shared)
set(RDRHOOK_DEPS ${CMAKE_CURRENT_SOURCE_DIR}/third-party)

# Enable folder organization in IDEs
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

# Output all used commands when building
set(CMAKE_VERBOSE_MAKEFILE ON)

# vcpkg integration - find MinHook
find_package(minhook CONFIG REQUIRED)

# Find Python for code generation
find_package(Python3 COMPONENTS Interpreter REQUIRED)

# Keep the Rust WASM SDK bindings in sync with nativedb.
add_custom_target(generate_natives
    COMMAND ${Python3_EXECUTABLE} "${CMAKE_CURRENT_SOURCE_DIR}/tools/codegen/generate_natives.py"
    WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
    COMMENT "Generating Rust native bindings from nativedb..."
    SOURCES
        "${CMAKE_CURRENT_SOURCE_DIR}/tools/codegen/generate_natives.py"
        "${CMAKE_CURRENT_SOURCE_DIR}/nativedb/natives.json"
    BYPRODUCTS
        "${CMAKE_CURRENT_SOURCE_DIR}/scripts/rdr2-wasm/src/natives.rs"
)

# Build the JavaScript example through the pinned Javy toolchain on demand.
add_custom_target(javy_example
    COMMAND powershell -NoProfile -ExecutionPolicy Bypass
            -File "${CMAKE_CURRENT_SOURCE_DIR}/scripts/javy-wasm/build.ps1"
    WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
    COMMENT "Building the Javy JavaScript WASM example..."
    SOURCES
        "${CMAKE_CURRENT_SOURCE_DIR}/scripts/javy-wasm/build.ps1"
        "${CMAKE_CURRENT_SOURCE_DIR}/scripts/javy-wasm/exports.wit"
        "${CMAKE_CURRENT_SOURCE_DIR}/scripts/javy-wasm/javy-9.1-runtime.patch"
        "${CMAKE_CURRENT_SOURCE_DIR}/scripts/javy-wasm/plugin/Cargo.toml"
        "${CMAKE_CURRENT_SOURCE_DIR}/scripts/javy-wasm/plugin/src/lib.rs"
        "${CMAKE_CURRENT_SOURCE_DIR}/scripts/javy-wasm/src/catalog.js"
        "${CMAKE_CURRENT_SOURCE_DIR}/scripts/javy-wasm/src/entrypoints.js"
        "${CMAKE_CURRENT_SOURCE_DIR}/scripts/javy-wasm/src/main.js"
    BYPRODUCTS
        "${CMAKE_CURRENT_SOURCE_DIR}/shared/static/mods/javy-trainer/main.wasm"
)

# Build the C# example into a self-contained WASI Preview 1 core module.
add_custom_target(dotnet_example
    COMMAND powershell -NoProfile -ExecutionPolicy Bypass
            -File "${CMAKE_CURRENT_SOURCE_DIR}/scripts/dotnet-wasm/build.ps1"
            -Configuration Release -LockedMode
    WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
    COMMENT "Building the .NET NativeAOT-LLVM WASM example..."
    SOURCES
        "${CMAKE_CURRENT_SOURCE_DIR}/scripts/dotnet-wasm/build.ps1"
        "${CMAKE_CURRENT_SOURCE_DIR}/scripts/dotnet-wasm/bootstrap-wasi-sdk.ps1"
        "${CMAKE_CURRENT_SOURCE_DIR}/scripts/dotnet-wasm/example/Entrypoints.cs"
        "${CMAKE_CURRENT_SOURCE_DIR}/scripts/dotnet-wasm/Rdr2.Wasm.Sdk/Host.cs"
    BYPRODUCTS
        "${CMAKE_CURRENT_SOURCE_DIR}/shared/static/mods/dotnet-example/main.wasm"
)

# Lua compilation remains owned by the external compiler; this target only verifies the
# pinned, synchronized artifact consumed by ScriptHook.
add_custom_target(lua_example_verify
    COMMAND powershell -NoProfile -ExecutionPolicy Bypass
            -File "${CMAKE_CURRENT_SOURCE_DIR}/scripts/lua-wasm/verify.ps1"
    WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
    COMMENT "Verifying the synchronized Lua WASM artifact..."
    SOURCES
        "${CMAKE_CURRENT_SOURCE_DIR}/scripts/lua-wasm/compiler.lock.json"
        "${CMAKE_CURRENT_SOURCE_DIR}/scripts/lua-wasm/verify.ps1"
        "${CMAKE_CURRENT_SOURCE_DIR}/shared/static/mods/lua-example/main.wasm"
)

# Target selection
set(TARGET_PROJECT "all" CACHE STRING "Project to CMake")
 
if(TARGET_PROJECT STREQUAL "all")
    set(rdrhook ON)
    set(launcher ON)
    set(launcher-hook ON)
    set(wasmtest ON)
    add_subdirectory(rdrhook rdrhook)
    add_subdirectory(launcher launcher)
    add_subdirectory(launcher-hook launcher-hook)
    add_subdirectory(wasmtest wasmtest)
else()
    set(${TARGET_PROJECT} ON)
    add_subdirectory(${TARGET_PROJECT} ${TARGET_PROJECT})
endif()
