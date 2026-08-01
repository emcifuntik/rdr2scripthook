# The upstream C API archive contains a native static library in addition to
# its DLL and import library. Package only the static artifact.

if(NOT VCPKG_TARGET_IS_WINDOWS OR NOT VCPKG_TARGET_ARCHITECTURE STREQUAL "x64")
    message(FATAL_ERROR "wasmtime: this overlay currently supports x64 Windows only")
endif()

set(WASMTIME_VERSION "47.0.3")
vcpkg_download_distfile(ARCHIVE
    URLS "https://github.com/bytecodealliance/wasmtime/releases/download/v${WASMTIME_VERSION}/wasmtime-v${WASMTIME_VERSION}-x86_64-windows-c-api.zip"
    FILENAME "wasmtime-v${WASMTIME_VERSION}-x86_64-windows-c-api.zip"
    SHA512 401a0a1fdcf820fb4d50d106be462c00a19746d619a08e17d9fe9b165e147124c59004fb022c211acc9aaf08036c06011ddb55504b4f6e9ed3e324aa0548365a
)

vcpkg_extract_source_archive_ex(
    OUT_SOURCE_PATH SOURCE_PATH
    ARCHIVE "${ARCHIVE}"
)

file(INSTALL "${SOURCE_PATH}/include/"
    DESTINATION "${CURRENT_PACKAGES_DIR}/include"
)

file(INSTALL "${SOURCE_PATH}/lib/wasmtime.lib"
    DESTINATION "${CURRENT_PACKAGES_DIR}/lib"
)
file(INSTALL "${SOURCE_PATH}/lib/wasmtime.lib"
    DESTINATION "${CURRENT_PACKAGES_DIR}/debug/lib"
)

configure_file(
    "${CMAKE_CURRENT_LIST_DIR}/wasmtimeConfig.cmake.in"
    "${CURRENT_PACKAGES_DIR}/share/${PORT}/wasmtimeConfig.cmake"
    @ONLY
)

vcpkg_install_copyright(FILE_LIST "${SOURCE_PATH}/LICENSE")
