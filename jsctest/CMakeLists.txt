cmake_minimum_required(VERSION 3.21)

project(jsctest)

# Source files
set(PROJECT_SOURCE_FILES
    src/main.cpp
)

# Create executable
add_executable(${PROJECT_NAME} ${PROJECT_SOURCE_FILES})

# Include directories
target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

# Find vcpkg packages
find_package(JavaScriptCore CONFIG REQUIRED)
find_package(spdlog CONFIG REQUIRED)

# Find cmakeconfig.h file (full path including filename)
find_file(JSC_CMAKE_CONFIG_FILE NAMES cmakeconfig.h)

# Link libraries from vcpkg
target_link_libraries(${PROJECT_NAME} PRIVATE
    JavaScriptCore::JavaScriptCore
    JavaScriptCore::WTF
    JavaScriptCore::bmalloc
    JavaScriptCore::icu-dt
    JavaScriptCore::icu-in
    JavaScriptCore::icu-uc
    spdlog::spdlog
    # System libraries required by JavaScriptCore
    ws2_32
    winmm
    dbghelp
    Synchronization

    clang_rt.builtins-x86_64.lib
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
    # JavaScriptCore static linking macros
    STATICALLY_LINKED_WITH_JavaScriptCore
    STATICALLY_LINKED_WITH_WTF
    STATICALLY_LINKED_WITH_bmalloc
)

# Set force include cmakeconfig.h to all source files
# Note: clang-cl uses /FI like MSVC, not -include
target_compile_options(${PROJECT_NAME} PRIVATE
    /FI${JSC_CMAKE_CONFIG_FILE}
)

# Target properties
set_target_properties(${PROJECT_NAME} PROPERTIES
    CXX_STANDARD 20
    CXX_STANDARD_REQUIRED ON
    RUNTIME_OUTPUT_DIRECTORY_DEBUG "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Debug"
    RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Release"
)
