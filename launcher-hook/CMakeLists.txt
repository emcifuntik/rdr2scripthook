cmake_minimum_required(VERSION 3.21)

project(launcher-hook)

file(GLOB_RECURSE PROJECT_SOURCE_FILES "src/*.h" "src/*.hpp" "src/*.cpp" "src/*.c")

# Create DLL
add_library(${PROJECT_NAME} SHARED
  ${PROJECT_SOURCE_FILES}
)

# Include directories
target_include_directories(${PROJECT_NAME} PRIVATE
  ${PROJECT_BINARY_DIR}
  ${CMAKE_CURRENT_SOURCE_DIR}/src
)

# Link MinHook from vcpkg
target_link_libraries(${PROJECT_NAME} PRIVATE
  minhook::minhook
)

# Compiler definitions
target_compile_definitions(${PROJECT_NAME} PRIVATE
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
  RUNTIME_OUTPUT_DIRECTORY_DEBUG "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Debug"
  RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Release"
  LINK_FLAGS_RELEASE "/OPT:REF /OPT:ICF"
)
