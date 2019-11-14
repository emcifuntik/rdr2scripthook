#pragma once

#include <functional>
#include <cstdint>
#include <deque>
#include <mutex>
#include <unordered_map>

#include "scriptHandlerMgr.h"
#include "CSingleton.h"
#include "scrThread.h"
#include <alt-log.h>

class CScriptManager: public CSingleton<CScriptManager>
{
public:
	struct TickerScript : GtaThread
	{
		std::function<void()> callback;
		std::function<void(uint32_t)> onKeyDown;
		std::function<void(uint32_t)> onKeyUp;

		TickerScript(std::function<void()>&& _callback) :
			callback(std::move(_callback))
		{

		}

		void BindKeyDown(std::function<void(uint32_t)> cb) { onKeyDown = cb; }
		void BindKeyUp(std::function<void(uint32_t)> cb) { onKeyUp = cb; }

		void KeyDown(uint32_t key)
		{
			if (onKeyDown)
				onKeyDown(key);
		}

		void KeyUp(uint32_t key)
		{
			if (onKeyUp)
				onKeyUp(key);
		}

		void Execute() override { callback(); }

		void* operator new(size_t size)
		{
			return CSysAllocator::Instance().Alloc(size);
		}

		void operator delete(void* p)
		{
			return CSysAllocator::Instance().Dealloc(p);
		}
	};

private:
	rage::scriptHandlerMgr * g_scriptHandlerMgr;
	std::vector<GtaThread*> ourThreads;
	rage::scrThread** currentScriptThread;
	
	WNDPROC pWndProc;
	std::unordered_map<uint64_t, uint64_t> crossMap;
	bool* isInSession = nullptr;
	
	std::deque<std::pair<uint32_t, bool>> keyEvents;
	std::mutex keyQueue;
	bool needReceiveEvents = false;
	std::wstring wClientPath;
public:
	bool scriptCanBeStarted;

	rage::scrThread* GetActiveThread();
	void SetActiveThread(rage::scrThread* thread);

	rage::scriptHandlerMgr* GetScriptHandleMgr() { return g_scriptHandlerMgr; }
	void SetClientPath(std::wstring path) { wClientPath = path; }

	TickerScript* CreateTicker(std::function<void()>&& callback)
	{
		TickerScript* script = new TickerScript{ std::move(callback) };

		if (!RegisterThread(script))
		{
			delete script;
			return nullptr;
		}
		ourThreads.push_back(script);

		return script;
	}

	void DeleteScript(GtaThread* script)
	{
		if (script)
		{
			script->Kill();
			//delete script;
		}
	}

	uint32_t GetNextScriptID()
	{
		static int id = 0xFFFF;
		return ++id;
	}

	void Init();

	void AddCrossMapEntry(uint64_t oldHash, uint64_t newHash);
	uintptr_t GetNativeAddress(uint64_t hash);
	
	void HookWinApi();
	bool UpdateGtaScript(GtaThread* thread, int ticksCount);
	bool UpdateSingleScripts(void* collection);
	void LoadCustomScripts();
	LRESULT APIENTRY WndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

	void PushKeyEvent(uint32_t key, bool down);
	bool PopKeyEvent(uint32_t& key, bool& down);

private:
	bool RegisterThread(GtaThread* thread);
};