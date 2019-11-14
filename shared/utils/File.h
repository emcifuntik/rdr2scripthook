#pragma once
#ifdef _WIN32

#pragma comment(lib, "version.lib")
#pragma comment(lib, "shlwapi.lib")

#include <Windows.h>
#include <Shlwapi.h>
#include <psapi.h>
#endif
#include <string>
#include <vector>
#include "Process.h"

namespace File {
	static bool Exists(const std::wstring& path) {
		return PathFileExistsW(path.c_str());
	}

	static bool Exists(const std::string& path) {
		return PathFileExistsA(path.c_str());
	}

	static std::wstring GetCurrentDir()
	{
		DWORD len = GetCurrentDirectoryW(0, NULL);
		std::wstring path(len - 1, L'\0');
		GetCurrentDirectoryW(len, (LPWSTR)path.data());
		return path;
	}

	static std::wstring GetModulePath(HMODULE module)
	{
		DWORD size = 256;
		std::vector<wchar_t> buffer(size);

		do
		{
			buffer.resize(size);
			GetModuleFileNameW(module, buffer.data(), size);
			size *= 1.5;
		} while (GetLastError() == ERROR_INSUFFICIENT_BUFFER);

		return std::wstring(buffer.begin(), buffer.end());
	}

	static std::wstring GetRemoteModulePath(HANDLE process, HMODULE module)
	{
		DWORD size = 256;
		std::vector<wchar_t> buffer(size);

		do
		{
			buffer.resize(size);
			GetModuleFileNameExW(process, module, buffer.data(), size);
			size *= 1.5;
		} while (GetLastError() == ERROR_INSUFFICIENT_BUFFER);

		return std::wstring(buffer.begin(), buffer.end());
	}

	static std::wstring GetModuleDir(HMODULE module)
	{
		std::wstring path = GetModulePath(module);
		return path.substr(0, path.find_last_of(L"\\/"));
	}

	static std::wstring GetModuleName(HMODULE module)
	{
		std::wstring path = GetModulePath(module);
		return path.substr(path.find_last_of(L"\\") + 1);
	}

	static std::wstring GetRemoteModuleName(HANDLE process, HMODULE module)
	{
		std::wstring path = GetRemoteModulePath(process, module);
		return path.substr(path.find_last_of(L"\\") + 1);
	}

	static std::wstring GetModuleNameWithoutExtension(HMODULE module)
	{
		std::wstring name = GetModuleName(module);
		size_t idx = name.find_last_of(L".");
		return (idx == -1)
			? name
			: name.substr(0, idx);
	}
}
