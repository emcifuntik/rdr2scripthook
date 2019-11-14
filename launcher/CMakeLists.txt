cmake_minimum_required (VERSION 3.10)

project(launcher)
set(PROJECT_NAME launcher)

include(cotire)

include_directories(${PROJECT_BINARY_DIR})

file(GLOB_RECURSE PROJECT_SOURCE_FILES "src/*.h" "src/*.hpp" "src/*.cpp")
file(GLOB_RECURSE SHARED_SOURCE_FILES "${RDRHOOK_SHARED}/*.h")

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
GroupSources(${RDRHOOK_SHARED}/src "Shared")

file(GLOB children RELATIVE ${RDRHOOK_SHARED} ${RDRHOOK_SHARED}/*.h)

foreach(child ${children})
  source_group(Shared FILES ${RDRHOOK_SHARED}/${child})
endforeach()

include_directories(
  ./src
  ${RDRHOOK_SHARED}/log
  ${RDRHOOK_SHARED}/utils
)

add_executable(
  ${PROJECT_NAME}
  ${PROJECT_SOURCE_FILES}
  ${SHARED_SOURCE_FILES}
)

add_definitions(
  -DCXX_COMPILER_ID="${CMAKE_CXX_COMPILER_ID}"
  -DUNICODE
  -D_UNICODE
)

set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /MT /Zi")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /MTd")

set_target_properties(${PROJECT_NAME} PROPERTIES
  CXX_STANDARD 17
  RUNTIME_OUTPUT_DIRECTORY_DEBUG "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Debug"
  RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Release"
  COTIRE_CXX_PREFIX_HEADER_INIT "src/stdafx.h"
)
set(COTIRE_UNITY_OUTPUT_DIRECTORY "")
