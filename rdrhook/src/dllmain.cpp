#include "stdafx.h"
#include "CMemory.h"
#include <chrono>
#include <fstream>
#include <thread>
#include <unordered_map>
#include "alt-log.h"
#include "scripting/CScriptManager.h"

using namespace alt;

std::wstring _moduleDir;
static std::wstring GetModulePath(HMODULE module)
{
	DWORD size = MAX_PATH;
	std::vector<wchar_t> buffer(size);

	do
	{
		buffer.resize(size);
		GetModuleFileNameW(module, buffer.data(), size);
		size = (DWORD)(size * 1.5);
	} while (GetLastError() == ERROR_INSUFFICIENT_BUFFER);

	std::wstring modulePath = std::wstring(buffer.begin(), buffer.end());

	size_t slashPos = modulePath.size();
	for (int i = int(modulePath.size() - 1); i >= 0; --i)
	{
		if (modulePath[i] == L'/' || modulePath[i] == L'\\') {
			slashPos = i;
			break;
		}
	}

	std::wstring moduleDir = modulePath.substr(0, slashPos);
	return moduleDir;
}

void Init()
{
	CMemory::RunHooks();
	CScriptManager::Instance().Init();
	Log::Info << "Inited" << Log::Endl;
}

LPSTR (*GetCommandLineA_Orig)() = nullptr;
LPSTR WINAPI GetCommandLineA_Hook()
{
	static bool inited = false;
	if (!inited) {
		inited = true;
		Init();
	}
	return GetCommandLineA_Orig();
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved)
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
	{
		_moduleDir = GetModulePath(hModule);
		CScriptManager::Instance().SetClientPath(_moduleDir);
		CMemory::Base() = (uintptr_t)GetModuleHandle(NULL);

		std::wstring logPath = _moduleDir + L"/log.txt";
		Log::Push(new Log::FileStream(logPath));

		std::wstring crossMapPath = _moduleDir + L"/crossmap.dat";
		std::ifstream crossIn(crossMapPath, std::ifstream::binary);

		uint64_t oldHash;
		uint64_t newHash;
		if (crossIn.good())
		{
			while (!crossIn.eof())
			{
				crossIn.read((char*)& oldHash, sizeof(uint64_t));
				crossIn.read((char*)& newHash, sizeof(uint64_t));
				CScriptManager::Instance().AddCrossMapEntry(oldHash, newHash);
			}
		}

		Log::Info << "RDR2 Scripthook initialized" << Log::Endl;

		MH_Initialize();
		CMemory(GetCommandLineA).Detour(GetCommandLineA_Hook, &GetCommandLineA_Orig);
		break;
	}
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
		break;
    case DLL_PROCESS_DETACH:
		MH_Uninitialize();
		Log::Info << "RDR2 Scripthook deinitialized" << Log::Endl;
        break;
    }
    return TRUE;
}
