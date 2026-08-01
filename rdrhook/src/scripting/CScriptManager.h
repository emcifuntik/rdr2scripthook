#pragma once

#include <cstdint>
#include <deque>
#include <mutex>
#include <string>
#include <unordered_map>

#include "CSingleton.h"

class CScriptManager: public CSingleton<CScriptManager>
{
private:
	WNDPROC pWndProc = nullptr;
	std::unordered_map<uint64_t, uint64_t> crossMap;
	bool* isInSession = nullptr;

	std::deque<std::pair<uint32_t, bool>> keyEvents;
	std::mutex keyQueue;
	bool needReceiveEvents = false;
	std::wstring wClientPath;
	void*** globalsPtr = nullptr;
	bool wasmModsInitialized = false;
	bool wasmModsLoaded = false;
public:
	bool scriptCanBeStarted = false;

	void SetClientPath(std::wstring path) { wClientPath = path; }

	void Init();

	void AddCrossMapEntry(uint64_t oldHash, uint64_t newHash);
	uintptr_t GetNativeAddress(uint64_t hash);

	void HookWinApi();
	bool UpdateSingleScripts(void* collection);
	void LoadWasmMods();
	void UpdateWasmMods();
	void OnWasmKeyDown(uint32_t key);
	void OnWasmKeyUp(uint32_t key);
	void ShutdownWasmMods();
	LRESULT APIENTRY WndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

	void PushKeyEvent(uint32_t key, bool down);
	bool PopKeyEvent(uint32_t& key, bool& down);

	void* GetGlobalPointer(uint32_t globalId);
};
