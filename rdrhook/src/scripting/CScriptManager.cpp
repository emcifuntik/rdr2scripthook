#include "stdafx.h"
#include "CScriptManager.h"

bool(*UpdateSingleScripts_orig)(void*) = nullptr;
bool UpdateSingleScripts_Hook(void* collection)
{
	bool origResult = UpdateSingleScripts_orig(collection);
	bool ourResult = CScriptManager::Instance().UpdateSingleScripts(collection);
	return origResult || ourResult;
}

LRESULT APIENTRY _WndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	return CScriptManager::Instance().WndProc(hwnd, uMsg, wParam, lParam);
}

uintptr_t(*GetNativeAddress_orig)(uint64_t hash) = nullptr;

void(*ShutdownLoadingScreen_orig)(void) = nullptr;
void ShutdownLoadingScreen()
{
	if (!CScriptManager::Instance().scriptCanBeStarted)
		CScriptManager::Instance().scriptCanBeStarted = true;
	ShutdownLoadingScreen_orig();
}

void CScriptManager::HookWinApi()
{
	HWND hWnd = FindWindowA("sgaWindow", "Red Dead Redemption 2");
	pWndProc = (WNDPROC)SetWindowLongPtr(hWnd, GWLP_WNDPROC, (LONG_PTR)_WndProc);
}

rage::scrThread * CScriptManager::GetActiveThread()
{
	return *currentScriptThread;
}

void CScriptManager::SetActiveThread(rage::scrThread * thread)
{
	*currentScriptThread = thread;
}

void CScriptManager::Init()
{
	constexpr CMemory::Pattern scriptHandlerMgrPat("48 8D 0D ? ? ? ? E8 ? ? ? ? 84 C0 75 ? 8B 8B ? ? ? ?");
	constexpr CMemory::Pattern currentScriptThreadPat("48 39 1D ? ? ? ? 75 ? 48 8D 05 ? ? ? ?");
	constexpr CMemory::Pattern isInSessionPat("80 3D ? ? ? ? ? 74 ? 48 8B 0D ? ? ? ? E8 ? ? ? ? 0F B6 40 ?");
	constexpr CMemory::Pattern getNativeAddressPat("0F B6 C1 48 8D 15 ? ? ? ? 4C 8B C9");
	constexpr CMemory::Pattern updateSingleScriptsPat("48 89 5C 24 ? 48 89 6C 24 ? 48 89 74 24 ? 57 41 56 41 57 48 83 EC ? 45 33 F6 BD ? ? ? ?");
	constexpr CMemory::Pattern shutdownLoadingScreenPat("8A 05 ? ? ? ? 84 C0 75 ? C6 05 ? ? ? ? ?");
	constexpr CMemory::Pattern globalsPtrPat("4C 8D 05 ? ? ? ? 4D 8B 08 4D 85 C9 74 ? 4D 3B D9");

	g_scriptHandlerMgr = scriptHandlerMgrPat.Search().GetOffset().Get<rage::scriptHandlerMgr*>();
	currentScriptThread = currentScriptThreadPat.Search().GetOffset(3).Get<rage::scrThread * *>();
	isInSession = isInSessionPat.Search().GetOffset(2).Get<bool*>();
	GetNativeAddress_orig = getNativeAddressPat.Search().Get<decltype(GetNativeAddress_orig)>();
	updateSingleScriptsPat.Search().Detour(UpdateSingleScripts_Hook, &UpdateSingleScripts_orig);
	shutdownLoadingScreenPat.Search().Detour(ShutdownLoadingScreen, &ShutdownLoadingScreen_orig);
	globalsPtr = globalsPtrPat.Search().GetOffset().Get<void***>();
}

void CScriptManager::AddCrossMapEntry(uint64_t oldHash, uint64_t newHash)
{
	if (!crossMap.count(oldHash)) {}
		crossMap.insert(std::pair<uint64_t, uint64_t>(oldHash, newHash));
}

uintptr_t CScriptManager::GetNativeAddress(uint64_t hash)
{
	uint64_t _hash = hash;
	uintptr_t address = 0;
	if (crossMap.count(_hash))
		_hash = crossMap[_hash];

	return GetNativeAddress_orig(_hash);
}

bool CScriptManager::UpdateGtaScript(GtaThread* thread, int ticksCount)
{
	bool result = false;
	if (thread->context.threadId)
	{
		if (thread->Update(ticksCount) != rage::eThreadState::ThreadStateKilled)
			result = true;
	}
	return result;
}

bool customScriptsInited = false;

bool CScriptManager::UpdateSingleScripts(void* collection)
{
	if (*CScriptManager::isInSession)
	{
		TerminateProcess(GetCurrentProcess(), 0);
		return false;
	}

	bool result = false;

	std::vector<std::pair<uint32_t, bool>> keys;
	uint32_t key = 0;
	bool down = false;

	while (PopKeyEvent(key, down)) keys.push_back(std::pair(key, down));

	for (size_t i = 0; i < ourThreads.size(); ++i)
	{
		GtaThread* thread = ourThreads[i];
		if (UpdateGtaScript(thread, 13000000))
			result = true;

		TickerScript* script = (TickerScript*)thread;
		auto state = script->Push();
		for (auto ev : keys)
		{
			if (ev.second) script->KeyDown(ev.first);
			else script->KeyUp(ev.first);
		}
	}


	if (!customScriptsInited && scriptCanBeStarted)
	{
		HookWinApi();
		LoadCustomScripts();
		customScriptsInited = true;
	}

	return result;
}

//https://stackoverflow.com/questions/874134/find-out-if-string-ends-with-another-string-in-c
bool hasEnding(std::string const& fullString, std::string const& ending) {
	if (fullString.length() >= ending.length())
		return (0 == fullString.compare(fullString.length() - ending.length(), ending.length(), ending));
	else
		return false;
}

typedef uintptr_t(*GetNativeAddressFunc)(uint64_t hash);
typedef void*(*GetGlobalPointerFunc)(uint32_t globalVarId);
typedef void(*LibInitFunc)(GetNativeAddressFunc, GetGlobalPointerFunc);
typedef void(*LibTickFunc)();
typedef void(*LibKeyDown)(uint32_t);
typedef void(*LibKeyUp)(uint32_t);

uintptr_t _GetNativeAddress(uint64_t hash)
{
	return CScriptManager::Instance().GetNativeAddress(hash);
}

void* _GetGlobalPointer(uint32_t globalVarId)
{
	return CScriptManager::Instance().GetGlobalPointer(globalVarId);
}

void CScriptManager::LoadCustomScripts()
{
	const fs::path pathToShow{ wClientPath + L"/scripts" };

	for (const auto& entry : fs::directory_iterator(pathToShow))
	{
		const auto filenameStr = entry.path().filename().string();
		if (entry.is_regular_file())
		{
			if (hasEnding(filenameStr, ".dll") || hasEnding(filenameStr, ".asi"))
			{
				alt::Log::Debug << "Trying to load \"" << entry.path().generic_u8string() << "\"" << alt::Log::Endl;
				HMODULE scriptLib = LoadLibraryW(entry.path().generic_wstring().c_str());
				if (scriptLib) {
					LibTickFunc libTick = (LibTickFunc)GetProcAddress(scriptLib, "Tick");
					LibInitFunc libInit = (LibInitFunc)GetProcAddress(scriptLib, "Init");
					LibKeyDown libKeyDown = (LibKeyDown)GetProcAddress(scriptLib, "OnKeyDown");
					LibKeyUp libKeyUp = (LibKeyUp)GetProcAddress(scriptLib, "OnKeyUp");

					if (libInit && libTick)
					{
						libInit(_GetNativeAddress, _GetGlobalPointer);
						auto ticker = CScriptManager::Instance().CreateTicker(libTick);

						if (libKeyDown)
						{
							alt::Log::Debug << "OnKeyDown event binded for " << filenameStr << alt::Log::Endl;
							ticker->BindKeyDown(libKeyDown);
						}
						if (libKeyUp)
						{
							ticker->BindKeyUp(libKeyUp);
							alt::Log::Debug << "OnKeyUp event binded for " << filenameStr << alt::Log::Endl;
						}

						alt::Log::Info << filenameStr << " successfully loaded" << alt::Log::Endl;
					}
					else
						alt::Log::Error << filenameStr << " load error. Init() and Tick() functions must be present in script DLL" << alt::Log::Endl;
				}
				else 
					alt::Log::Error << filenameStr << " load error" << alt::Log::Endl;
			}
		}
	}
	needReceiveEvents = true;
}

LRESULT CScriptManager::WndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	bool callOrig = true;

	switch (uMsg)
	{
		case WM_KEYDOWN:
		case WM_KEYUP:
		case WM_CHAR:
		case WM_SYSCHAR:
		case WM_SYSKEYDOWN:
		case WM_SYSKEYUP:
		{
			bool down = false;
			bool wasDown = (lParam & (1 << 30)) > 0;

			if ((uMsg == WM_KEYDOWN || uMsg == WM_SYSKEYDOWN) && !wasDown)
			{
				down = true;
				CScriptManager::PushKeyEvent((uint32_t)wParam, down);
			}
			else if ((uMsg == WM_KEYUP || uMsg == WM_SYSKEYUP) && wasDown)
			{
				down = false;
				CScriptManager::PushKeyEvent((uint32_t)wParam, down);
			}

			break;
		}
	}

	return callOrig
		? CallWindowProc(pWndProc, hwnd, uMsg, wParam, lParam)
		: DefWindowProc(hwnd, uMsg, wParam, lParam);
}

void CScriptManager::PushKeyEvent(uint32_t key, bool down)
{
	if (!needReceiveEvents) return;
	std::unique_lock<std::mutex> lock(keyQueue);
	keyEvents.push_back(std::pair(key, down));
}

bool CScriptManager::PopKeyEvent(uint32_t& key, bool& down)
{
	if (!needReceiveEvents) return false;
	std::unique_lock<std::mutex> lock(keyQueue);
	if (keyEvents.size() > 0) {
		auto ev = keyEvents.front();
		key = ev.first;
		down = ev.second;
		keyEvents.pop_front();
		return true;
	}
	return false;
}

void* CScriptManager::GetGlobalPointer(uint32_t globalId)
{
	int firstArrayId = globalId / 0x3ffff;
	int secondArrayId = globalId & 0x3ffff;
	return (void*)&globalsPtr[firstArrayId][secondArrayId];
}

bool CScriptManager::RegisterThread(GtaThread* thread)
{
	auto context = thread->GetContext();

	context->threadId = GetNextScriptID();
	context->scriptHash = context->threadId;
	thread->scriptHash = context->threadId;

	thread->Reset(context->scriptHash, nullptr, 0);

	alt::Log::Debug("Created Thread with ID:", thread->GetId(), "ptr:", (void*)thread);

	return true;
}

