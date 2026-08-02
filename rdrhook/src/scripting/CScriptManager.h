#pragma once

#include <cstdint>
#include <deque>
#include <mutex>
#include <string>
#include <unordered_map>

#include "CSingleton.h"
#include "GtaThread.h"
#include "rage/CSysAllocator.h"
#include "scriptHandlerMgr.h"

class CScriptManager: public CSingleton<CScriptManager>
{
public:
	class WasmScriptThread final : public GtaThread
	{
	public:
		void Execute() override;

		void* operator new(size_t size)
		{
			return CSysAllocator::Instance().Alloc(size);
		}

		void operator delete(void* memory)
		{
			CSysAllocator::Instance().Dealloc(memory);
		}
	};

private:
	rage::scriptHandlerMgr* scriptHandlerManager = nullptr;
	rage::scrThread** currentScriptThread = nullptr;
	WasmScriptThread* wasmThread = nullptr;
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

	rage::scrThread* GetActiveThread() const;
	void SetActiveThread(rage::scrThread* thread);
	rage::scriptHandlerMgr* GetScriptHandlerManager() const
	{
		return scriptHandlerManager;
	}

	void SetClientPath(std::wstring path) { wClientPath = path; }

	void Init();

	void AddCrossMapEntry(uint64_t oldHash, uint64_t newHash);
	uintptr_t GetNativeAddress(uint64_t hash);

	void HookWinApi();
	bool UpdateGtaScript(GtaThread* thread, int operationCount);
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

private:
	bool StartWasmThread();
	bool RegisterThread(GtaThread* thread);
	uint32_t GetNextScriptId();
};
