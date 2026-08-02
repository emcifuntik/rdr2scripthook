#pragma once

#define VALIDATE_SIZE(structure, size) \
    static_assert(sizeof(structure) == size, "Invalid structure size of " #structure)
#define VALIDATE_OFFSET(structure, member, offset)                              \
    static_assert(offsetof(structure, member) == offset,                        \
                  "Invalid offset of " #member " in " #structure)
