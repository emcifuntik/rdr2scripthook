cmake_minimum_required(VERSION 3.21)

project(rdrhook)

file(GLOB_RECURSE PROJECT_SOURCE_FILES "src/*.h" "src/*.hpp" "src/*.cpp" "src/*.c")
file(GLOB_RECURSE SHARED_SOURCE_FILES
  # Shared headers
  "${RDRHOOK_SHARED}/log/*.h"
  "${RDRHOOK_SHARED}/memory/*.h"
  "${RDRHOOK_SHARED}/utils/*.h"
)

macro(GroupSources curdir groupindex)
  file(GLOB children RELATIVE ${curdir} ${curdir}/*)

  foreach(child ${children})
    if(IS_DIRECTORY ${curdir}/${child})
      GroupSources(${curdir}/${child} ${groupindex}/${child})
    else()

      string(REPLACE "/" "\\" groupname ${groupindex})

      source_group(${groupname} FILES ${curdir}/${child})
    endif()
  endforeach()
endmacro()

GroupSources(${PROJECT_SOURCE_DIR}/src "Source Files")
GroupSources(${PROJECT_SOURCE_DIR}/resources "Resource Files")
GroupSources(${RDRHOOK_SHARED}/src "Shared")

# Create library
add_library(${PROJECT_NAME} SHARED
  ${PROJECT_SOURCE_FILES}
  ${SHARED_SOURCE_FILES}
)

# Include directories
target_include_directories(${PROJECT_NAME} PRIVATE
  ${PROJECT_BINARY_DIR}
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${CMAKE_CURRENT_SOURCE_DIR}/src
  ${RDRHOOK_SHARED}/log
  ${RDRHOOK_SHARED}/memory
  ${RDRHOOK_SHARED}/utils
)

find_package(toml11 CONFIG REQUIRED)
find_package(spdlog CONFIG REQUIRED)
find_package(wasmtime CONFIG REQUIRED)
find_package(unofficial-webview2 CONFIG REQUIRED)
find_package(VulkanHeaders CONFIG REQUIRED)
find_path(JITASM_INCLUDE_DIR jitasm.h REQUIRED)

target_include_directories(${PROJECT_NAME} PRIVATE ${JITASM_INCLUDE_DIR})

target_link_libraries(${PROJECT_NAME} PRIVATE
  minhook::minhook
  toml11::toml11
  spdlog::spdlog
  wasmtime::wasmtime
  unofficial::webview2::webview2
  Vulkan::Headers
  d3d11
  d3d12
  dxgi
  dxguid
  windowsapp
)

# Compiler definitions
target_compile_definitions(${PROJECT_NAME} PRIVATE
  CXX_COMPILER_ID="${CMAKE_CXX_COMPILER_ID}"
  UNICODE
  _UNICODE
  _CRT_SECURE_NO_WARNINGS
  # Prevent Windows.h from defining min/max macros
  NOMINMAX
  WIN32_LEAN_AND_MEAN
  $<$<CONFIG:Debug>:_DEBUG>
)

# jitasm intentionally uses x86 mnemonic names such as `and`. Clang treats
# alternative operator spellings as keywords unless this compatibility mode is
# disabled (MSVC, jitasm's primary Windows compiler, already accepts them).
target_compile_options(${PROJECT_NAME} PRIVATE
  $<$<CXX_COMPILER_ID:Clang>:-Xclang -fno-operator-names>
)

# Target properties
set_target_properties(${PROJECT_NAME} PROPERTIES
  CXX_STANDARD 20
  CXX_STANDARD_REQUIRED ON
  RUNTIME_OUTPUT_DIRECTORY_DEBUG "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Debug"
  RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Release"
  LINK_FLAGS_RELEASE "/OPT:REF /OPT:ICF"
)

# Precompiled headers (disabled for clang-cl compatibility)
# target_precompile_headers(${PROJECT_NAME} PRIVATE src/stdafx.h)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Keep the SDK bindings current whenever the main project is built.
add_dependencies(${PROJECT_NAME} generate_natives)

# Copy static files at configure time.
file(COPY "${RDRHOOK_SHARED}/static/" DESTINATION "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Release")
file(COPY "${RDRHOOK_SHARED}/static/" DESTINATION "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Debug")

# Remove files deployed by the retired JavaScript runtime from existing build
# trees. This also keeps incremental builds from trying to load stale JS mods.
file(REMOVE_RECURSE
  "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Release/scripts"
  "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Debug/scripts"
  "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Release/mods/trainer"
  "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Debug/mods/trainer"
)
file(REMOVE
  "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Release/wasmtime.dll"
  "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Debug/wasmtime.dll"
  "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Release/mods/example-mod/main.js"
  "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Debug/mods/example-mod/main.js"
  "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Release/mods/tsconfig.json"
  "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Debug/mods/tsconfig.json"
)
