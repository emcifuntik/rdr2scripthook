// rdr2d.cpp - RDR2 Script Hook Launcher
// 1. Injects launcher-hook.dll into Rockstar Games Launcher (hooks CreateProcess to suspend RDR2)
// 2. Waits for suspended RDR2.exe to appear
// 3. Injects rdrhook.dll into RDR2
// 4. Resumes RDR2

#include <Windows.h>
#include <TlHelp32.h>
#include <iostream>
#include <string>
#include <set>
#include <vector>
#include "Process.h"
#include "CInjector.h"

// Get all processes with a given name, returning their PIDs and handles
std::vector<std::pair<DWORD, HANDLE>> GetAllProcessesByName(LPCWSTR name)
{
    std::vector<std::pair<DWORD, HANDLE>> result;

    HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    PROCESSENTRY32 process;
    ZeroMemory(&process, sizeof(process));
    process.dwSize = sizeof(process);

    if (Process32First(snapshot, &process))
    {
        do
        {
            if (!wcscmp(process.szExeFile, name))
            {
                HANDLE handle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, process.th32ProcessID);
                if (handle)
                {
                    result.push_back({ process.th32ProcessID, handle });
                }
            }
        } while (Process32Next(snapshot, &process));
    }

    CloseHandle(snapshot);
    return result;
}

// Find Rockstar Games Launcher installation path from registry
std::wstring FindRGLInstall()
{
    TCHAR path[MAX_PATH] = { 0 };
    DWORD size = MAX_PATH * sizeof(TCHAR);
    HKEY key;
    LRESULT res;

    res = RegOpenKeyEx(HKEY_LOCAL_MACHINE, L"SOFTWARE\\WOW6432Node\\Rockstar Games\\Launcher", 0, KEY_READ, &key);
    if (res != ERROR_SUCCESS)
    {
        RegCloseKey(key);
        SetLastError(res);
        return L"";
    }

    res = RegQueryValueExW(key, L"InstallFolder", NULL, NULL, (LPBYTE)path, &size);
    if (res != ERROR_SUCCESS)
    {
        RegCloseKey(key);
        return L"";
    }

    RegCloseKey(key);
    return path;
}

// Inject a DLL into a process
bool InjectIntoProcess(HANDLE process, const std::wstring& dllName)
{
    if (!process) return false;

    CInjector inj(process);

    wchar_t thisPath[MAX_PATH] = { 0 };
    GetCurrentDirectory(MAX_PATH, thisPath);

    // Set DLL directory so dependencies can be found
    LPVOID SetDllDirectoryW_ = (LPVOID)GetProcAddress(GetModuleHandle(L"kernel32.dll"), "SetDllDirectoryW");
    LPVOID dllPath = inj.AllocRemoteString(thisPath);
    inj.CallRemoteProc(SetDllDirectoryW_, dllPath);
    inj.FreeRemoteMem(dllPath);

    return inj.InjectLibrary(dllName.c_str());
}

// Get main thread ID for a process
DWORD GetMainThreadId(DWORD processId)
{
    HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0);
    if (snapshot == INVALID_HANDLE_VALUE)
        return 0;

    THREADENTRY32 te;
    te.dwSize = sizeof(te);

    DWORD mainThreadId = 0;
    ULONGLONG earliestTime = MAXULONGLONG;

    if (Thread32First(snapshot, &te))
    {
        do
        {
            if (te.th32OwnerProcessID == processId)
            {
                HANDLE hThread = OpenThread(THREAD_QUERY_INFORMATION, FALSE, te.th32ThreadID);
                if (hThread)
                {
                    FILETIME createTime, exitTime, kernelTime, userTime;
                    if (GetThreadTimes(hThread, &createTime, &exitTime, &kernelTime, &userTime))
                    {
                        ULONGLONG time = ((ULONGLONG)createTime.dwHighDateTime << 32) | createTime.dwLowDateTime;
                        if (time < earliestTime)
                        {
                            earliestTime = time;
                            mainThreadId = te.th32ThreadID;
                        }
                    }
                    CloseHandle(hThread);
                }
            }
        } while (Thread32Next(snapshot, &te));
    }

    CloseHandle(snapshot);
    return mainThreadId;
}

// Resume all threads in a process
void ResumeProcess(DWORD processId)
{
    HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0);
    if (snapshot == INVALID_HANDLE_VALUE)
        return;

    THREADENTRY32 te;
    te.dwSize = sizeof(te);

    if (Thread32First(snapshot, &te))
    {
        do
        {
            if (te.th32OwnerProcessID == processId)
            {
                HANDLE hThread = OpenThread(THREAD_SUSPEND_RESUME, FALSE, te.th32ThreadID);
                if (hThread)
                {
                    ResumeThread(hThread);
                    CloseHandle(hThread);
                }
            }
        } while (Thread32Next(snapshot, &te));
    }

    CloseHandle(snapshot);
}

int main(int argc, char* argv[])
{
    bool noLaunch = false;

    // Parse command line arguments
    for (int i = 1; i < argc; ++i)
    {
        if (!strcmp(argv[i], "nolaunch"))
            noLaunch = true;
    }

    std::cout << "RDR2 Script Hook Launcher" << std::endl;
    std::cout << "=========================" << std::endl;
    std::cout << std::endl;

    // Check if RDR2 is already running
    HANDLE rdr2Process = Process::GetProcessByName(L"RDR2.exe");
    if (rdr2Process)
    {
        std::cout << "RDR2 is already running, attempting injection..." << std::endl;

        if (InjectIntoProcess(rdr2Process, L"rdrhook.dll"))
        {
            std::cout << "Successfully injected rdrhook.dll!" << std::endl;
            CloseHandle(rdr2Process);
            return 0;
        }
        else
        {
            std::cout << "Error: Failed to inject rdrhook.dll" << std::endl;
            CloseHandle(rdr2Process);
            std::cout << "Press Enter to exit..." << std::endl;
            getchar();
            return 1;
        }
    }

    std::wstring rglPath = FindRGLInstall();

    if (rglPath.empty())
    {
        std::cout << "Error: Rockstar Games Launcher not found in registry" << std::endl;
        std::cout << "Press Enter to exit..." << std::endl;
        getchar();
        return 1;
    }

    // Track which Launcher PIDs we've injected into
    std::set<DWORD> injectedPIDs;

    // Inject launcher-hook into any existing Launcher.exe processes first
    auto existingLaunchers = GetAllProcessesByName(L"Launcher.exe");
    if (!existingLaunchers.empty())
    {
        std::cout << "Found existing Rockstar Launcher, injecting hook..." << std::endl;
        for (auto& [pid, handle] : existingLaunchers)
        {
            if (InjectIntoProcess(handle, L"launcher-hook.dll"))
            {
                injectedPIDs.insert(pid);
                std::cout << "Injected launcher-hook.dll into existing Launcher.exe (PID=" << pid << ")" << std::endl;
            }
            CloseHandle(handle);
        }
    }

    // Start the launcher with game launch parameters (this triggers game launch even if launcher is running)
    if (!noLaunch)
    {
        std::cout << "Starting Rockstar Games Launcher with game launch params..." << std::endl;

        STARTUPINFO si = { sizeof(si) };
        PROCESS_INFORMATION pi = { 0 };

        std::wstring launcherExe = rglPath + L"\\Launcher.exe";
        std::wstring launcherArgs = L"-skipPatcherCheck -minmodeApp=rdr2";

        if (!CreateProcessW(
            launcherExe.c_str(),
            (LPWSTR)launcherArgs.c_str(),
            NULL, NULL, FALSE, 0, NULL,
            rglPath.c_str(), &si, &pi))
        {
            std::cout << "Error: Failed to start Launcher.exe (error=" << GetLastError() << ")" << std::endl;
            std::cout << "Press Enter to exit..." << std::endl;
            getchar();
            return 1;
        }

        CloseHandle(pi.hProcess);
        CloseHandle(pi.hThread);
    }

    std::cout << "Waiting for RDR2.exe";

    // Main loop - inject launcher-hook into any new Launcher.exe processes
    // and wait for RDR2.exe to appear (suspended)
    int elapsed = 0;
    int dotCount = 0;
    const int timeoutMs = 300000; // 5 minutes

    while (true)
    {
        // Check for RDR2.exe
        rdr2Process = Process::GetProcessByName(L"RDR2.exe");
        if (rdr2Process)
        {
            std::cout << std::endl;
            std::cout << "RDR2.exe detected!" << std::endl;
            break;
        }

        // Inject launcher-hook into any new Launcher.exe processes
        auto launchers = GetAllProcessesByName(L"Launcher.exe");
        for (auto& [pid, handle] : launchers)
        {
            if (injectedPIDs.find(pid) == injectedPIDs.end())
            {
                if (InjectIntoProcess(handle, L"launcher-hook.dll"))
                {
                    injectedPIDs.insert(pid);
                }
            }
            CloseHandle(handle);
        }

        // Timeout check
        if (elapsed >= timeoutMs)
        {
            std::cout << std::endl;
            std::cout << "Error: Timed out waiting for RDR2.exe" << std::endl;
            std::cout << "Press Enter to exit..." << std::endl;
            getchar();
            return 1;
        }

        // Print progress dots
        if (elapsed % 1000 == 0)
        {
            std::cout << "." << std::flush;
            dotCount++;
            if (dotCount >= 60)
            {
                std::cout << std::endl;
                dotCount = 0;
            }
        }

        Sleep(100);
        elapsed += 100;
    }

    // Get process ID for resuming later
    DWORD rdr2Pid = GetProcessId(rdr2Process);

    // Small delay to ensure process is ready for injection
    Sleep(100);

    std::cout << "Injecting rdrhook.dll..." << std::endl;

    if (!InjectIntoProcess(rdr2Process, L"rdrhook.dll"))
    {
        std::cout << "Error: Failed to inject rdrhook.dll" << std::endl;
        // Resume anyway so the game doesn't hang
        ResumeProcess(rdr2Pid);
        CloseHandle(rdr2Process);
        std::cout << "Press Enter to exit..." << std::endl;
        getchar();
        return 1;
    }

    std::cout << "Successfully injected rdrhook.dll!" << std::endl;

    // Resume the suspended RDR2 process
    std::cout << "Resuming RDR2..." << std::endl;
    ResumeProcess(rdr2Pid);

    std::cout << std::endl;
    std::cout << "Done! You can close this window now." << std::endl;

    CloseHandle(rdr2Process);
    return 0;
}
