cmake_minimum_required(VERSION 3.21)

project(launcher)

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

# Create executable
add_executable(${PROJECT_NAME}
  ${PROJECT_SOURCE_FILES}
  ${SHARED_SOURCE_FILES}
)

# Include directories
target_include_directories(${PROJECT_NAME} PRIVATE
  ${PROJECT_BINARY_DIR}
  ${CMAKE_CURRENT_SOURCE_DIR}/src
  ${RDRHOOK_SHARED}/log
  ${RDRHOOK_SHARED}/utils
)

# Compiler definitions
target_compile_definitions(${PROJECT_NAME} PRIVATE
  CXX_COMPILER_ID="${CMAKE_CXX_COMPILER_ID}"
  UNICODE
  _UNICODE
  NOMINMAX
  WIN32_LEAN_AND_MEAN
  _MT
  $<$<CONFIG:Debug>:_DEBUG>
)

# Target properties
set_target_properties(${PROJECT_NAME} PROPERTIES
  CXX_STANDARD 20
  CXX_STANDARD_REQUIRED ON
  RUNTIME_OUTPUT_DIRECTORY_DEBUG "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Debug"
  RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Release"
)
