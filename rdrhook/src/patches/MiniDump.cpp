#include "stdafx.h"
#include "CMemory.h"
#include <DbgHelp.h>
#include <ctime>
#include <iomanip>
#include <sstream>

#pragma comment(lib, "dbghelp.lib")

// Module directory (defined in dllmain.cpp)
extern std::wstring _moduleDir;

namespace {

// Original function pointer
bool (*g_OriginalCrashHandler)() = nullptr;

// Create crashes folder if it doesn't exist
std::wstring GetCrashesFolder()
{
    std::wstring crashesPath = _moduleDir + L"\\crashes";
    CreateDirectoryW(crashesPath.c_str(), nullptr);
    return crashesPath;
}

// Generate timestamp string for filename
std::wstring GetTimestampString()
{
    auto now = std::time(nullptr);
    std::tm tm;
    localtime_s(&tm, &now);

    std::wostringstream wss;
    wss << std::put_time(&tm, L"%Y%m%d_%H%M%S");
    return wss.str();
}

// Write minidump file
bool WriteMiniDump(EXCEPTION_POINTERS* exceptionInfo = nullptr)
{
    std::wstring crashesFolder = GetCrashesFolder();
    std::wstring timestamp = GetTimestampString();
    std::wstring dumpPath = crashesFolder + L"\\crash_" + timestamp + L".dmp";

    HANDLE hFile = CreateFileW(
        dumpPath.c_str(),
        GENERIC_WRITE,
        0,
        nullptr,
        CREATE_ALWAYS,
        FILE_ATTRIBUTE_NORMAL,
        nullptr
    );

    if (hFile == INVALID_HANDLE_VALUE)
    {
        spdlog::error("[MiniDump] Failed to create dump file: {}", GetLastError());
        return false;
    }

    MINIDUMP_EXCEPTION_INFORMATION mdei = {};
    MINIDUMP_EXCEPTION_INFORMATION* pMdei = nullptr;

    if (exceptionInfo)
    {
        mdei.ThreadId = GetCurrentThreadId();
        mdei.ExceptionPointers = exceptionInfo;
        mdei.ClientPointers = FALSE;
        pMdei = &mdei;
    }

    // Include useful information in the dump
    MINIDUMP_TYPE dumpType = static_cast<MINIDUMP_TYPE>(
        MiniDumpWithDataSegs |
        MiniDumpWithHandleData |
        MiniDumpWithIndirectlyReferencedMemory |
        MiniDumpWithProcessThreadData |
        MiniDumpWithThreadInfo
    );

    BOOL success = MiniDumpWriteDump(
        GetCurrentProcess(),
        GetCurrentProcessId(),
        hFile,
        dumpType,
        pMdei,
        nullptr,
        nullptr
    );

    CloseHandle(hFile);

    if (success)
    {
        spdlog::info("[MiniDump] Crash dump saved to: {}",
            std::filesystem::path(dumpPath).string());
    }
    else
    {
        spdlog::error("[MiniDump] Failed to write dump: {}", GetLastError());
    }

    return success != FALSE;
}

// Vectored exception handler for capturing crash context
LONG WINAPI VectoredExceptionHandler(EXCEPTION_POINTERS* exceptionInfo)
{
    // Only handle fatal exceptions
    switch (exceptionInfo->ExceptionRecord->ExceptionCode)
    {
    case EXCEPTION_ACCESS_VIOLATION:
    case EXCEPTION_ARRAY_BOUNDS_EXCEEDED:
    case EXCEPTION_DATATYPE_MISALIGNMENT:
    case EXCEPTION_FLT_DIVIDE_BY_ZERO:
    case EXCEPTION_FLT_OVERFLOW:
    case EXCEPTION_FLT_UNDERFLOW:
    case EXCEPTION_ILLEGAL_INSTRUCTION:
    case EXCEPTION_INT_DIVIDE_BY_ZERO:
    case EXCEPTION_INT_OVERFLOW:
    case EXCEPTION_PRIV_INSTRUCTION:
    case EXCEPTION_STACK_OVERFLOW:
        spdlog::error("[MiniDump] Fatal exception caught: 0x{:08X} at 0x{:016X}",
            exceptionInfo->ExceptionRecord->ExceptionCode,
            reinterpret_cast<uintptr_t>(exceptionInfo->ExceptionRecord->ExceptionAddress));
        WriteMiniDump(exceptionInfo);
        break;
    default:
        break;
    }

    return EXCEPTION_CONTINUE_SEARCH;
}

// Hooked crash handler - replaces game's crash handler
bool CrashHandler_Hook()
{
    spdlog::info("[MiniDump] Game crash handler triggered");

    // Write a minidump without exception info (game detected the crash internally)
    WriteMiniDump(nullptr);

    // Return true to indicate we handled it (prevents game's crash reporter)
    // Return false if you want the game to continue with its crash handling
    return true;
}

// Hook registration
CMemory::Hook _CrashHandlerHook([]() {
    // Pattern: 40 53 48 83 EC ? E8 ? ? ? ? 8A D8 84 C0
    // This is the game's internal crash detection function
    constexpr CMemory::Pattern crashHandlerPattern("40 53 48 83 EC ? E8 ? ? ? ? 8A D8 84 C0");
    CMemory crashHandler = crashHandlerPattern.Search();

    if (crashHandler.IsValid())
    {
        crashHandler.Detour(CrashHandler_Hook, &g_OriginalCrashHandler);
        spdlog::info("[MiniDump] Crash handler hooked successfully");

        // Also register a vectored exception handler for additional crash capture
        AddVectoredExceptionHandler(1, VectoredExceptionHandler);
        spdlog::info("[MiniDump] Vectored exception handler registered");
    }
    else
    {
        spdlog::warn("[MiniDump] Failed to find crash handler pattern");
    }
});

} // anonymous namespace
