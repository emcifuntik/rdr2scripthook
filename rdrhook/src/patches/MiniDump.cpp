#include "stdafx.h"
#include "CMemory.h"
#include <DbgHelp.h>
#include <atomic>
#include <ctime>
#include <iomanip>
#include <sstream>

#pragma comment(lib, "dbghelp.lib")

// Module directory (defined in dllmain.cpp)
extern std::wstring _moduleDir;

namespace {

LPTOP_LEVEL_EXCEPTION_FILTER g_previousExceptionFilter = nullptr;
std::atomic_flag g_dumpStarted = ATOMIC_FLAG_INIT;

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

bool WriteFullMemoryDump(EXCEPTION_POINTERS* exceptionInfo)
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

    MINIDUMP_EXCEPTION_INFORMATION dumpException = {};
    dumpException.ThreadId = GetCurrentThreadId();
    dumpException.ExceptionPointers = exceptionInfo;
    dumpException.ClientPointers = FALSE;

    const MINIDUMP_TYPE dumpType = static_cast<MINIDUMP_TYPE>(
        MiniDumpWithFullMemory |
        MiniDumpWithFullMemoryInfo |
        MiniDumpWithHandleData |
        MiniDumpWithUnloadedModules |
        MiniDumpWithProcessThreadData |
        MiniDumpWithThreadInfo |
        MiniDumpIgnoreInaccessibleMemory
    );

    spdlog::info("[MiniDump] Writing full-memory crash dump...");

    const BOOL success = MiniDumpWriteDump(
        GetCurrentProcess(),
        GetCurrentProcessId(),
        hFile,
        dumpType,
        &dumpException,
        nullptr,
        nullptr
    );
    const DWORD writeError = success ? ERROR_SUCCESS : GetLastError();

    CloseHandle(hFile);

    if (success)
    {
        spdlog::info("[MiniDump] Crash dump saved to: {}",
            std::filesystem::path(dumpPath).string());
    }
    else
    {
        spdlog::error("[MiniDump] Failed to write dump: {}", writeError);
    }

    return success != FALSE;
}

LONG WINAPI UnhandledExceptionHandler(EXCEPTION_POINTERS* exceptionInfo)
{
    LONG disposition = EXCEPTION_CONTINUE_SEARCH;
    if (g_previousExceptionFilter &&
        g_previousExceptionFilter != UnhandledExceptionHandler)
    {
        disposition = g_previousExceptionFilter(exceptionInfo);
    }

    // A previous filter can recover by explicitly continuing execution.
    if (disposition == EXCEPTION_CONTINUE_EXECUTION ||
        !exceptionInfo || !exceptionInfo->ExceptionRecord)
    {
        return disposition;
    }

    // A terminal process can have multiple faulting threads. Capture only the
    // first unrecoverable exception so dumps cannot overwrite one another.
    if (!g_dumpStarted.test_and_set())
    {
        spdlog::error("[MiniDump] Unrecoverable exception caught: 0x{:08X} at 0x{:016X}",
            exceptionInfo->ExceptionRecord->ExceptionCode,
            reinterpret_cast<uintptr_t>(exceptionInfo->ExceptionRecord->ExceptionAddress));
        WriteFullMemoryDump(exceptionInfo);
    }

    return disposition;
}

CMemory::Hook _CrashHandlerHook([]() {
    g_previousExceptionFilter =
        SetUnhandledExceptionFilter(UnhandledExceptionHandler);
    spdlog::info("[MiniDump] Unhandled exception filter registered");
});

} // anonymous namespace
