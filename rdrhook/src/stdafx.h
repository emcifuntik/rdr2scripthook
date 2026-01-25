#pragma once

// These macros may be defined by CMake, use guards to avoid warnings
#ifndef NOMINMAX
#define NOMINMAX
#endif
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>

// _beginthreadex is needed by MSVC's <thread> header with clang-cl
// We can't include <process.h> directly because we have a local file with that name
// The function is provided by UCRT, we just need the declaration
extern "C" {
    _ACRTIMP uintptr_t __cdecl _beginthreadex(
        void* _Security,
        unsigned _StackSize,
        unsigned (__stdcall* _StartAddress)(void*),
        void* _ArgList,
        unsigned _InitFlag,
        unsigned* _ThrdAddr
    );
    _ACRTIMP void __cdecl _endthreadex(unsigned _ReturnCode);
}

#include <spdlog/spdlog.h>
#include <CMemory.h>
#include <chrono>
#include <filesystem>
#include "rage/CSysAllocator.h"

namespace fs = std::filesystem;
