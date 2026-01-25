cmake_minimum_required(VERSION 3.21)

project(simple-trainer)

file(GLOB_RECURSE PROJECT_SOURCE_FILES "src/*.h" "src/*.hpp" "src/*.cpp" "src/*.c")
file(GLOB_RECURSE SHARED_SOURCE_FILES
  #Shared
  "${RDRHOOK_SHARED}/log/*.h"
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

# ASI extension for plugin
set(CMAKE_SHARED_LIBRARY_SUFFIX ".asi")

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
)

# Compiler definitions
target_compile_definitions(${PROJECT_NAME} PRIVATE
  CXX_COMPILER_ID="${CMAKE_CXX_COMPILER_ID}"
  UNICODE
  _UNICODE
  _CRT_SECURE_NO_WARNINGS
  NOMINMAX
  WIN32_LEAN_AND_MEAN
  _MT
  $<$<CONFIG:Debug>:_DEBUG>
)

# Target properties
set_target_properties(${PROJECT_NAME} PROPERTIES
  CXX_STANDARD 20
  CXX_STANDARD_REQUIRED ON
  RUNTIME_OUTPUT_DIRECTORY_DEBUG "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Debug/scripts"
  RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Release/scripts"
  LINK_FLAGS_RELEASE "/OPT:REF /OPT:ICF"
)

# Precompiled headers (native CMake PCH)
target_precompile_headers(${PROJECT_NAME} PRIVATE src/pch.h)

# Depend on generate_natives target to ensure NativeNames.h is up-to-date
add_dependencies(${PROJECT_NAME} generate_natives)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
