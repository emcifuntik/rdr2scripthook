#pragma once

// These macros may be defined by CMake, use guards to avoid warnings
#ifndef NOMINMAX
#define NOMINMAX
#endif
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>

#include "Logger.h"
#include <CMemory.h>
#include <filesystem>
