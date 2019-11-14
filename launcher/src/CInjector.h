#pragma once
#include <Windows.h>
#include <string>

HMODULE WINAPI GetRemoteModuleHandleW(HANDLE hProcess, LPCWSTR lpModuleName);
FARPROC WINAPI GetRemoteProcAddressW(HANDLE hProcess, HMODULE hModule, LPCWSTR lpProcName, UINT Ordinal = 0, BOOL UseOrdinal = FALSE);

class CInjector
{
	HANDLE process;

public:
	CInjector(HANDLE _process) :
		process(_process)
	{

	}

	bool IsDLLInjected(const std::wstring& moduleName)
	{
		HMODULE module = GetRemoteModuleHandleW(process, moduleName.c_str());

		if (!module)
			return false;
		return true;
	}

	FARPROC GetRemoteProcAddress(const std::wstring& moduleName, const std::wstring& funcName)
	{
		HMODULE module = GetRemoteModuleHandleW(process, moduleName.c_str());

		if (!module)
			return NULL;

		return GetRemoteProcAddressW(process, module, funcName.c_str());
	}

	bool InjectLibrary(const std::wstring& path)
	{
		LPVOID LoadLibraryW_ = (LPVOID)GetProcAddress(GetModuleHandleW(L"kernel32.dll"), "LoadLibraryW");

		LPVOID lpPath = AllocRemoteString(path);
		CallRemoteProc(LoadLibraryW_, lpPath);
		FreeRemoteMem(lpPath);

		return true;
	}

	bool SetDllPath(const std::wstring& path)
	{
		LPVOID SetDllDirectoryW_ = (LPVOID)GetProcAddress(GetModuleHandleW(L"kernel32.dll"), "SetDllDirectoryW");

		LPVOID lpPath = AllocRemoteString(path);
		CallRemoteProc(SetDllDirectoryW_, lpPath);
		FreeRemoteMem(lpPath);

		return true;
	}

	LPVOID AllocRemoteString(const std::wstring& str)
	{
		std::size_t len = str.length();

		LPVOID lpStr = VirtualAllocEx(process, NULL, (len + 1) * sizeof(WCHAR), MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
		WriteProcessMemory(process, lpStr, str.c_str(), (len + 1) * sizeof(WCHAR), NULL);

		return lpStr;
	}

	template<class T>
	LPVOID AllocRemoteStruct(const T& val)
	{
		std::size_t size = sizeof(T);

		LPVOID lpVal = VirtualAllocEx(process, NULL, size, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
		WriteProcessMemory(process, lpVal, &val, size, NULL);

		return lpVal;
	}

	void FreeRemoteMem(LPVOID mem)
	{
		VirtualFreeEx(process, mem, 0, MEM_RELEASE);
	}

	bool CallRemoteProc(LPVOID proc, LPVOID parameter)
	{
		HANDLE remoteThread_ = CreateRemoteThread(process, NULL, NULL, (LPTHREAD_START_ROUTINE)proc, parameter, 0, NULL);
		WaitForSingleObject(remoteThread_, INFINITE);
		CloseHandle(remoteThread_);

		return true;
	}
};
