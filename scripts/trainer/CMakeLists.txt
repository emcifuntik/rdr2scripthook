cmake_minimum_required (VERSION 3.10)

project(simple-trainer)

include(cotire)

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

include_directories(
  ${PROJECT_BINARY_DIR}
  ./
  ./src

  ${RDRHOOK_SHARED}/log
)

set(CMAKE_SHARED_LIBRARY_SUFFIX ".asi")

add_library(
  ${PROJECT_NAME} SHARED
  ${PROJECT_SOURCE_FILES}
  ${SHARED_SOURCE_FILES}
)

add_definitions(
  -DCXX_COMPILER_ID="${CMAKE_CXX_COMPILER_ID}"
  -DUNICODE
  -D_UNICODE

  -D_CRT_SECURE_NO_WARNINGS
)

set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /MT /Zi")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /MTd")

set(CMAKE_SHARED_LINKER_FLAGS_RELEASE "${CMAKE_SHARED_LINKER_FLAGS_RELEASE} /OPT:REF /OPT:ICF")

set_target_properties(${PROJECT_NAME} PROPERTIES
  CXX_STANDARD 17
  RUNTIME_OUTPUT_DIRECTORY_DEBUG "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Debug/scripts"
  RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Release/scripts"
  COTIRE_CXX_PREFIX_HEADER_INIT src/pch.h
)

cotire(${PROJECT_NAME})

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
