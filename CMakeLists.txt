cmake_minimum_required (VERSION 3.10)

project(rdr2hook)

# Only Debug and Release
set(CMAKE_CONFIGURATION_TYPES Debug Release)

set(TARGET_PROJECT "all" CACHE STRING "Project to CMake")
set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake/cotire/CMake/)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/LIB)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/BIN)
set(RDRHOOK_SHARED ${CMAKE_CURRENT_SOURCE_DIR}/shared)
set(RDRHOOK_DEPS ${CMAKE_CURRENT_SOURCE_DIR}/third-party)
set_property(GLOBAL PROPERTY USE_FOLDERS ON)
 
if(TARGET_PROJECT STREQUAL "all")
    set(rdrhook ON)
    set(launcher ON)
    set(trainer ON)
    add_subdirectory(rdrhook rdrhook)
    add_subdirectory(launcher launcher)
    add_subdirectory(scripts/trainer scripts/trainer)
else()
    set(${TARGET_PROJECT} ON)
    add_subdirectory(${TARGET_PROJECT} ${TARGET_PROJECT})
endif()

# Output all used commands when building
set(CMAKE_VERBOSE_MAKEFILE ON)
