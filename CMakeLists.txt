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
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/BIN)

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

# Custom target to generate natives.js from nativedb
add_custom_target(generate_natives
    COMMAND ${Python3_EXECUTABLE} "${CMAKE_CURRENT_SOURCE_DIR}/tools/codegen/generate_natives.py"
    WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
    COMMENT "Generating natives.js, natives_hashes.js, and NativeNames.h from nativedb..."
    SOURCES
        "${CMAKE_CURRENT_SOURCE_DIR}/tools/codegen/generate_natives.py"
        "${CMAKE_CURRENT_SOURCE_DIR}/nativedb/natives.json"
    BYPRODUCTS
        "${CMAKE_CURRENT_SOURCE_DIR}/scripts/trainer/src/core/NativeNames.h"
        "${CMAKE_CURRENT_SOURCE_DIR}/rdrhook/src/js/JSNativesGenerated.cpp"
)

# Target selection
set(TARGET_PROJECT "all" CACHE STRING "Project to CMake")
 
if(TARGET_PROJECT STREQUAL "all")
    set(rdrhook ON)
    set(launcher ON)
    set(launcher-hook ON)
    set(trainer ON)
    set(jsctest ON)
    add_subdirectory(rdrhook rdrhook)
    add_subdirectory(launcher launcher)
    add_subdirectory(launcher-hook launcher-hook)
    add_subdirectory(scripts/trainer scripts/trainer)
    add_subdirectory(jsctest jsctest)
else()
    set(${TARGET_PROJECT} ON)
    add_subdirectory(${TARGET_PROJECT} ${TARGET_PROJECT})
endif()
