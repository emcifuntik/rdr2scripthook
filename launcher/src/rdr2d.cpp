// rdr2d.cpp : Этот файл содержит функцию "main". Здесь начинается и заканчивается выполнение программы.
//

#include <iostream>
#include "Process.h"
#include "CInjector.h"

HANDLE WaitForGameStart(const wchar_t* process)
{
	HANDLE _process;
	while (true) {
		_process = Process::GetProcessByName(process);
		if (_process)
			return _process;
		Sleep(10);
	}
}

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

int main(int argc, char* argv[])
{
	bool noLaunch = false;
	if (argc > 0)
	{
		for (int i = 0; i < argc; ++i)
		{
			if (!strcmp(argv[i], "nolaunch"))
				noLaunch = true;
		}
	}

	HANDLE process = Process::GetProcessByName(L"RDR2.exe");
	if (process)
	{
		std::cout << "Error: RDR2 Process already started" << std::endl;
		getchar();
		return 1;
	}

	std::wstring newLauncherPath = FindRGLInstall();
	if (newLauncherPath.size() > 0 && !noLaunch)
	{
		std::cout << "Starting RDR2 game with launcher" << std::endl;

		STARTUPINFO siStartupInfo;
		PROCESS_INFORMATION piProcessInfo;

		memset(&siStartupInfo, 0, sizeof(siStartupInfo));
		memset(&piProcessInfo, 0, sizeof(piProcessInfo));
		siStartupInfo.cb = sizeof(siStartupInfo);

		if (!CreateProcessW((newLauncherPath + L"\\Launcher.exe").c_str(), (LPWSTR)L"-skipPatcherCheck -minmodeApp=rdr2", NULL, NULL, true, PROCESS_QUERY_INFORMATION, NULL, newLauncherPath.c_str(), &siStartupInfo, &piProcessInfo))
		{
			std::cout << "Error: Failed to start Launcher.exe" << std::endl;
			return 1;
		}
		WaitForGameStart(L"RDR2.exe");
	}
	else
	{
		if (noLaunch)
			std::cout << "You disabled auto launch" << std::endl;
		std::cout << "Start your RDR2 game with launcher" << std::endl;
		WaitForGameStart(L"RDR2.exe");
	}

	process = Process::GetProcessByName(L"RDR2.exe");
	if (!process) {
		std::cout << "RDR Process not started" << std::endl;
		getchar();
		return 1;
	}

	CInjector inj(process);

	wchar_t thisPath[MAX_PATH] = { 0 };
	GetCurrentDirectory(MAX_PATH, thisPath);

	LPVOID SetDllDirectoryW_ = (LPVOID)GetProcAddress(GetModuleHandle(L"kernel32.dll"), "SetDllDirectoryW");
	LPVOID dllPath = inj.AllocRemoteString(thisPath);
	inj.CallRemoteProc(SetDllDirectoryW_, dllPath);
	inj.FreeRemoteMem(dllPath);

	inj.InjectLibrary(L"rdrhook.dll");

	std::cout << "Injected" << std::endl;
	return 0;
}
