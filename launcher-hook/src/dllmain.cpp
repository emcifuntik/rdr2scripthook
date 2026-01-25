#include <windows.h>
#include <MinHook.h>
#include <string>
#include <algorithm>

// Debug logging helper
static void DebugLog(const wchar_t* format, ...) {
    wchar_t buffer[1024];
    va_list args;
    va_start(args, format);
    vswprintf_s(buffer, format, args);
    va_end(args);

    std::wstring msg = L"[launcher-hook] ";
    msg += buffer;
    msg += L"\n";
    OutputDebugStringW(msg.c_str());
}

// Module handle for this DLL
static HMODULE g_hModule = nullptr;

// Original function pointers
static decltype(&CreateProcessW) CreateProcessW_Orig = nullptr;
static decltype(&CreateProcessA) CreateProcessA_Orig = nullptr;

// Check if the executable name matches RDR2 (ANSI version)
bool IsRDR2ProcessA(LPCSTR lpApplicationName, LPSTR lpCommandLine) {
    std::string exeName;

    if (lpApplicationName && strlen(lpApplicationName) > 0) {
        exeName = lpApplicationName;
    } else if (lpCommandLine && strlen(lpCommandLine) > 0) {
        std::string cmdLine = lpCommandLine;
        size_t start = 0;
        size_t end = 0;

        if (cmdLine[0] == '"') {
            start = 1;
            end = cmdLine.find('"', 1);
        } else {
            end = cmdLine.find(' ');
        }

        if (end != std::string::npos) {
            exeName = cmdLine.substr(start, end - start);
        } else {
            exeName = cmdLine.substr(start);
        }
    }

    if (exeName.empty()) {
        return false;
    }

    // Convert to lowercase for comparison
    std::string lowerName = exeName;
    std::transform(lowerName.begin(), lowerName.end(), lowerName.begin(), ::tolower);

    bool isRDR2 = lowerName.find("rdr2.exe") != std::string::npos;
    if (isRDR2) {
        DebugLog(L"IsRDR2ProcessA: Detected RDR2.exe in '%S'", exeName.c_str());
    }
    return isRDR2;
}

// Check if the executable name matches RDR2 (Unicode version)
bool IsRDR2Process(LPCWSTR lpApplicationName, LPWSTR lpCommandLine) {
    std::wstring exeName;

    if (lpApplicationName && wcslen(lpApplicationName) > 0) {
        exeName = lpApplicationName;
    } else if (lpCommandLine && wcslen(lpCommandLine) > 0) {
        std::wstring cmdLine = lpCommandLine;
        size_t start = 0;
        size_t end = 0;

        if (cmdLine[0] == L'"') {
            start = 1;
            end = cmdLine.find(L'"', 1);
        } else {
            end = cmdLine.find(L' ');
        }

        if (end != std::wstring::npos) {
            exeName = cmdLine.substr(start, end - start);
        } else {
            exeName = cmdLine.substr(start);
        }
    }

    if (exeName.empty()) {
        return false;
    }

    // Convert to lowercase for comparison
    std::wstring lowerName = exeName;
    std::transform(lowerName.begin(), lowerName.end(), lowerName.begin(), ::towlower);

    bool isRDR2 = lowerName.find(L"rdr2.exe") != std::wstring::npos;
    if (isRDR2) {
        DebugLog(L"IsRDR2Process: Detected RDR2.exe in '%s'", exeName.c_str());
    }
    return isRDR2;
}

// Hooked CreateProcessW - suspends RDR2 when it starts
BOOL WINAPI CreateProcessW_Hook(
    LPCWSTR lpApplicationName,
    LPWSTR lpCommandLine,
    LPSECURITY_ATTRIBUTES lpProcessAttributes,
    LPSECURITY_ATTRIBUTES lpThreadAttributes,
    BOOL bInheritHandles,
    DWORD dwCreationFlags,
    LPVOID lpEnvironment,
    LPCWSTR lpCurrentDirectory,
    LPSTARTUPINFOW lpStartupInfo,
    LPPROCESS_INFORMATION lpProcessInformation
) {
    bool isRDR2 = IsRDR2Process(lpApplicationName, lpCommandLine);

    // If it's RDR2, add CREATE_SUSPENDED flag so launcher can inject before it runs
    DWORD modifiedFlags = dwCreationFlags;
    if (isRDR2) {
        DebugLog(L"CreateProcessW_Hook: RDR2 detected, adding CREATE_SUSPENDED flag");
        modifiedFlags |= CREATE_SUSPENDED;
    }

    // Call original CreateProcessW
    BOOL result = CreateProcessW_Orig(
        lpApplicationName,
        lpCommandLine,
        lpProcessAttributes,
        lpThreadAttributes,
        bInheritHandles,
        modifiedFlags,
        lpEnvironment,
        lpCurrentDirectory,
        lpStartupInfo,
        lpProcessInformation
    );

    if (result && isRDR2 && lpProcessInformation) {
        DebugLog(L"CreateProcessW_Hook: RDR2 created suspended, PID=%lu, TID=%lu",
            lpProcessInformation->dwProcessId, lpProcessInformation->dwThreadId);
        // Don't resume - the launcher will inject and resume
    }

    return result;
}

// Hooked CreateProcessA - suspends RDR2 when it starts
BOOL WINAPI CreateProcessA_Hook(
    LPCSTR lpApplicationName,
    LPSTR lpCommandLine,
    LPSECURITY_ATTRIBUTES lpProcessAttributes,
    LPSECURITY_ATTRIBUTES lpThreadAttributes,
    BOOL bInheritHandles,
    DWORD dwCreationFlags,
    LPVOID lpEnvironment,
    LPCSTR lpCurrentDirectory,
    LPSTARTUPINFOA lpStartupInfo,
    LPPROCESS_INFORMATION lpProcessInformation
) {
    bool isRDR2 = IsRDR2ProcessA(lpApplicationName, lpCommandLine);

    // If it's RDR2, add CREATE_SUSPENDED flag
    DWORD modifiedFlags = dwCreationFlags;
    if (isRDR2) {
        DebugLog(L"CreateProcessA_Hook: RDR2 detected, adding CREATE_SUSPENDED flag");
        modifiedFlags |= CREATE_SUSPENDED;
    }

    // Call original CreateProcessA
    BOOL result = CreateProcessA_Orig(
        lpApplicationName,
        lpCommandLine,
        lpProcessAttributes,
        lpThreadAttributes,
        bInheritHandles,
        modifiedFlags,
        lpEnvironment,
        lpCurrentDirectory,
        lpStartupInfo,
        lpProcessInformation
    );

    if (result && isRDR2 && lpProcessInformation) {
        DebugLog(L"CreateProcessA_Hook: RDR2 created suspended, PID=%lu, TID=%lu",
            lpProcessInformation->dwProcessId, lpProcessInformation->dwThreadId);
        // Don't resume - the launcher will inject and resume
    }

    return result;
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved) {
    switch (ul_reason_for_call) {
    case DLL_PROCESS_ATTACH:
    {
        g_hModule = hModule;
        DisableThreadLibraryCalls(hModule);

        DebugLog(L"DllMain: DLL_PROCESS_ATTACH - Installing CreateProcess hooks");

        // Initialize MinHook
        if (MH_Initialize() != MH_OK) {
            DebugLog(L"DllMain: MH_Initialize failed");
            return FALSE;
        }

        // Hook CreateProcessW
        if (MH_CreateHook(&CreateProcessW, &CreateProcessW_Hook,
            reinterpret_cast<LPVOID*>(&CreateProcessW_Orig)) != MH_OK) {
            DebugLog(L"DllMain: Failed to hook CreateProcessW");
            MH_Uninitialize();
            return FALSE;
        }

        // Hook CreateProcessA
        if (MH_CreateHook(&CreateProcessA, &CreateProcessA_Hook,
            reinterpret_cast<LPVOID*>(&CreateProcessA_Orig)) != MH_OK) {
            DebugLog(L"DllMain: Failed to hook CreateProcessA");
            MH_Uninitialize();
            return FALSE;
        }

        // Enable hooks
        if (MH_EnableHook(MH_ALL_HOOKS) != MH_OK) {
            DebugLog(L"DllMain: Failed to enable hooks");
            MH_Uninitialize();
            return FALSE;
        }

        DebugLog(L"DllMain: Hooks installed successfully");
        break;
    }
    case DLL_PROCESS_DETACH:
        DebugLog(L"DllMain: DLL_PROCESS_DETACH");
        MH_DisableHook(MH_ALL_HOOKS);
        MH_Uninitialize();
        break;
    }
    return TRUE;
}
