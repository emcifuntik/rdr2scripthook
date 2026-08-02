vcpkg_from_github(
    OUT_SOURCE_PATH SOURCE_PATH
    REPO citizenfx/jitasm
    REF 981a592990c9271567c7a7c142681f67a8e19d47
    SHA512 4cadc011092568c2c9b5ae0c22d171ba0de4f96cdeb205db979c453c6573abfe18d757f037594ce8d8ccfafa77ea587e25a026c66d5f995079c0860eaf2fe7e0
    HEAD_REF master
)

file(INSTALL "${SOURCE_PATH}/jitasm.h"
    DESTINATION "${CURRENT_PACKAGES_DIR}/include"
)
vcpkg_install_copyright(FILE_LIST "${SOURCE_PATH}/jitasm.h")
