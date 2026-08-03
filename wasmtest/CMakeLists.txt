cmake_minimum_required(VERSION 3.21)

project(wasmtest)

find_package(spdlog CONFIG REQUIRED)
find_package(wasmtime CONFIG REQUIRED)

add_executable(${PROJECT_NAME}
  src/main.cpp
  ../rdrhook/src/wasm/Bindings.cpp
  ../rdrhook/src/wasm/GuestProfile.cpp
  ../rdrhook/src/wasm/Mod.cpp
  ../rdrhook/src/wasm/Runtime.cpp
  ../rdrhook/src/input/InputBindingManager.cpp
)

file(TO_CMAKE_PATH "${RDRHOOK_SHARED}/static/mods/example-mod" WASM_EXAMPLE_MOD_PATH)
file(TO_CMAKE_PATH "${RDRHOOK_SHARED}/static/mods/javy-trainer" WASM_JAVY_MOD_PATH)
file(TO_CMAKE_PATH "${RDRHOOK_SHARED}/static/mods/dotnet-example" WASM_DOTNET_MOD_PATH)
file(TO_CMAKE_PATH "${RDRHOOK_SHARED}/static/mods/lua-example" WASM_LUA_MOD_PATH)

target_include_directories(${PROJECT_NAME} PRIVATE
  ../rdrhook/src
  ../rdrhook/src/wasm
  ${RDRHOOK_SHARED}/log
)
target_compile_definitions(${PROJECT_NAME} PRIVATE
  WASM_EXAMPLE_MOD_PATH="${WASM_EXAMPLE_MOD_PATH}"
  WASM_JAVY_MOD_PATH="${WASM_JAVY_MOD_PATH}"
  WASM_DOTNET_MOD_PATH="${WASM_DOTNET_MOD_PATH}"
  WASM_LUA_MOD_PATH="${WASM_LUA_MOD_PATH}"
  RDR2_WASM_TEST
  UNICODE
  _UNICODE
  _CRT_SECURE_NO_WARNINGS
  NOMINMAX
  WIN32_LEAN_AND_MEAN
)
target_link_libraries(${PROJECT_NAME} PRIVATE
  spdlog::spdlog
  wasmtime::wasmtime
)

set_target_properties(${PROJECT_NAME} PROPERTIES
  CXX_STANDARD 20
  CXX_STANDARD_REQUIRED ON
  RUNTIME_OUTPUT_DIRECTORY_DEBUG "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Debug"
  RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Release"
)

add_dependencies(${PROJECT_NAME} generate_natives)
