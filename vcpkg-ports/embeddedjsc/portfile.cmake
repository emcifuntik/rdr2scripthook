# EmbeddedJSC — C++20 embedding library for WebKit's JavaScriptCore.
# Source: https://github.com/emcifuntik/EmbeddedJSC

vcpkg_check_linkage(ONLY_STATIC_LIBRARY)

if(NOT VCPKG_TARGET_IS_WINDOWS OR NOT VCPKG_TARGET_ARCHITECTURE STREQUAL "x64")
    message(FATAL_ERROR "embeddedjsc currently only supports x64-windows")
endif()

# Resolve clang-cl. JavaScriptCore in this overlay is built with clang-cl, and
# its headers have clang-specific predicates (e.g. wtf/PlatformCPU.h) that
# don't compile cleanly under MSVC's cl.exe. vcpkg's default windows toolchain
# uses cl.exe, so we override here. Search the usual install locations and
# fall back to PATH.
find_program(EJSC_CLANG_CL clang-cl
    PATHS
        "$ENV{ProgramFiles}/LLVM/bin"
        "$ENV{ProgramFiles\(x86\)}/LLVM/bin"
        "$ENV{ProgramW6432}/LLVM/bin"
)
if(NOT EJSC_CLANG_CL)
    message(FATAL_ERROR
        "embeddedjsc: clang-cl not found. Install LLVM "
        "(https://releases.llvm.org/) so clang-cl.exe is reachable, or set "
        "the EJSC_CLANG_CL variable before configure.")
endif()
message(STATUS "embeddedjsc: using clang-cl at ${EJSC_CLANG_CL}")

vcpkg_from_github(
    OUT_SOURCE_PATH SOURCE_PATH
    REPO emcifuntik/EmbeddedJSC
    REF f7ec66f9485f1184afe28316e2d1dc1ed3020251
    SHA512 314900b1585ea7c0598620656f429b902a2183abc91c9fa5de9436d8ddb57e41166f23ac4924640bdae30f1b29f95f741043fd914c92dd572b5311014cd7d593
    HEAD_REF master
)

vcpkg_cmake_configure(
    SOURCE_PATH "${SOURCE_PATH}"
    OPTIONS
        "-DCMAKE_C_COMPILER=${EJSC_CLANG_CL}"
        "-DCMAKE_CXX_COMPILER=${EJSC_CLANG_CL}"
        -DEJSC_BUILD_EXAMPLES=OFF
        -DEJSC_BUILD_TESTS=OFF
        -DEJSC_INSTALL=ON
)

vcpkg_cmake_install()

vcpkg_cmake_config_fixup(
    PACKAGE_NAME ejsc
    CONFIG_PATH lib/cmake/ejsc
)

# Headers belong in /include only; remove the duplicate debug headers.
file(REMOVE_RECURSE "${CURRENT_PACKAGES_DIR}/debug/include")

file(INSTALL "${SOURCE_PATH}/LICENSE"
    DESTINATION "${CURRENT_PACKAGES_DIR}/share/${PORT}"
    RENAME copyright)
