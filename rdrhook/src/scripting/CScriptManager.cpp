#include "stdafx.h"
#include "CScriptManager.h"
#include "wasm/Bindings.h"
#include "wasm/ModLoader.h"
#include "wasm/Runtime.h"
#include "webview/WebViewOverlay.h"
#include "Logger.h"

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

rage::scrThread* CScriptManager::GetActiveThread() const
{
	return currentScriptThread ? *currentScriptThread : nullptr;
}

void CScriptManager::SetActiveThread(rage::scrThread* thread)
{
	if (currentScriptThread)
		*currentScriptThread = thread;
}

void CScriptManager::Init()
{
	constexpr CMemory::Pattern scriptHandlerManagerPat("48 8D 0D ? ? ? ? E8 ? ? ? ? 84 C0 75 ? 8B 8B ? ? ? ?");
	constexpr CMemory::Pattern currentScriptThreadPat("48 39 1D ? ? ? ? 75 ? 48 8D 05 ? ? ? ?");
	constexpr CMemory::Pattern isInSessionPat("80 3D ? ? ? ? ? 74 ? 48 8B 0D ? ? ? ? E8 ? ? ? ? 0F B6 40 ?");
	constexpr CMemory::Pattern getNativeAddressPat("48 8B 15 ? ? ? ? 4C 8B C9 49 F7 D1");
	constexpr CMemory::Pattern updateSingleScriptsPat("48 89 5C 24 ? 48 89 6C 24 ? 48 89 74 24 ? 57 41 56 41 57 48 83 EC ? 45 33 F6 BD ? ? ? ?");
	constexpr CMemory::Pattern shutdownLoadingScreenPat("8A 05 ? ? ? ? 84 C0 75 ? C6 05 ? ? ? ? ?");
	constexpr CMemory::Pattern globalsPtrPat("4C 8D 05 ? ? ? ? 4D 8B 08 4D 85 C9 74 ? 4D 3B D9");

	scriptHandlerManager = scriptHandlerManagerPat.Search().GetOffset().Get<rage::scriptHandlerMgr*>();
	currentScriptThread = currentScriptThreadPat.Search().GetOffset(3).Get<rage::scrThread**>();
	isInSession = isInSessionPat.Search().GetOffset(2).Get<bool*>();
	GetNativeAddress_orig = getNativeAddressPat.Search().Get<decltype(GetNativeAddress_orig)>();
	updateSingleScriptsPat.Search().Detour(UpdateSingleScripts_Hook, &UpdateSingleScripts_orig);
	shutdownLoadingScreenPat.Search().Detour(ShutdownLoadingScreen, &ShutdownLoadingScreen_orig);
	globalsPtr = globalsPtrPat.Search().GetOffset().Get<void***>();
}

void CScriptManager::AddCrossMapEntry(uint64_t oldHash, uint64_t newHash)
{
	crossMap.try_emplace(oldHash, newHash);
}

uintptr_t CScriptManager::GetNativeAddress(uint64_t hash)
{
	const auto mapped = crossMap.find(hash);
	return GetNativeAddress_orig(mapped != crossMap.end() ? mapped->second : hash);
}

void CScriptManager::WasmScriptThread::Execute()
{
	auto& manager = CScriptManager::Instance();
	if (!manager.wasmModsInitialized)
	{
		manager.wasmModsInitialized = true;
		manager.LoadWasmMods();
		return;
	}

	manager.UpdateWasmMods();
	rdr2wasm::bindings::PumpGameUiInput();
}

bool CScriptManager::UpdateGtaScript(GtaThread* thread, int operationCount)
{
	if (!thread || !thread->context.threadId)
		return false;

	return thread->Update(operationCount) != rage::ThreadStateKilled;
}

bool CScriptManager::UpdateSingleScripts(void*)
{
	if (*CScriptManager::isInSession)
	{
		TerminateProcess(GetCurrentProcess(), 0);
		return false;
	}

	std::vector<std::pair<uint32_t, bool>> keys;
	uint32_t key = 0;
	bool down = false;

	while (PopKeyEvent(key, down)) keys.push_back(std::pair(key, down));

	rdr2wasm::bindings::PollKeyboard();
	if (rdr2::webview::WebViewOverlay::Instance().Pump())
		CScriptManager::PushKeyEvent(VK_F8, true);

	if (!wasmThread && scriptCanBeStarted)
	{
		HookWinApi();
		if (StartWasmThread())
			needReceiveEvents = true;
	}

	bool running = UpdateGtaScript(wasmThread, 13000000);
	if (wasmThread)
	{
		auto activeThread = wasmThread->Activate();
		for (const auto& event : keys)
		{
			if (event.second) OnWasmKeyDown(event.first);
			else OnWasmKeyUp(event.first);
		}
	}

	return running;
}

uintptr_t _GetNativeAddress(uint64_t hash)
{
	return CScriptManager::Instance().GetNativeAddress(hash);
}

void* _GetGlobalPointer(uint32_t globalVarId)
{
	return CScriptManager::Instance().GetGlobalPointer(globalVarId);
}

void CScriptManager::LoadWasmMods()
{
	spdlog::info("Initializing Wasmtime runtime...");

	// Wire the game-side resolvers used by the WASM host imports.
	rdr2wasm::InstallGameBridge(_GetNativeAddress, _GetGlobalPointer);

	auto& runtime = rdr2wasm::GetRuntime();
	if (!runtime.IsValid())
	{
		spdlog::error("Failed to initialize Wasmtime");
		return;
	}

	auto& modLoader = rdr2wasm::GetModLoader();
	if (!modLoader.Initialize(wClientPath))
	{
		spdlog::error("Failed to initialize mod loader");
		return;
	}

	int loadedCount = modLoader.LoadAllMods();
	if (loadedCount > 0)
	{
		wasmModsLoaded = true;
	}
}

void CScriptManager::UpdateWasmMods()
{
	if (!wasmModsLoaded) return;
	rdr2wasm::GetModLoader().TickAll();
}

void CScriptManager::OnWasmKeyDown(uint32_t key)
{
	if (!wasmModsLoaded) return;
	rdr2wasm::GetModLoader().OnKeyDownAll(key);
}

void CScriptManager::OnWasmKeyUp(uint32_t key)
{
	if (!wasmModsLoaded) return;
	rdr2wasm::GetModLoader().OnKeyUpAll(key);
}

void CScriptManager::ShutdownWasmMods()
{
	if (!wasmModsLoaded) return;
	rdr2wasm::GetModLoader().UnloadAllMods();
	wasmModsLoaded = false;
}

LRESULT CScriptManager::WndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if (rdr2::webview::WebViewOverlay::Instance().HandleWindowMessage(
			hwnd, uMsg, wParam, lParam))
		return 0;

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

bool CScriptManager::StartWasmThread()
{
	auto* thread = new WasmScriptThread();
	if (!RegisterThread(thread))
	{
		delete thread;
		spdlog::error("Failed to register the Wasmtime game script thread");
		return false;
	}

	wasmThread = thread;
	spdlog::info("Wasmtime game script thread registered with id {}",
	             wasmThread->GetId());
	return true;
}

bool CScriptManager::RegisterThread(GtaThread* thread)
{
	if (!thread || !scriptHandlerManager || !currentScriptThread)
		return false;

	const uint32_t scriptId = GetNextScriptId();
	thread->scriptHash = scriptId;
	thread->Reset(scriptId, nullptr, 0);
	return true;
}

uint32_t CScriptManager::GetNextScriptId()
{
	static uint32_t scriptId = 0xFFFF;
	return ++scriptId;
}
