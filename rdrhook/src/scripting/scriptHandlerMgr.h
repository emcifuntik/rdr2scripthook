#pragma once
#include "GtaThread.h"

namespace rage
{
	using netPlayer = void*;
	using scriptIdBase = uint64_t;

	class scriptHandlerMgr 
	{
	public:
		virtual ~scriptHandlerMgr() = default;
		virtual void Init() = 0;
		virtual void Update() = 0;
		virtual void Shutdown() = 0;
		virtual void NetworkInit() = 0;
		virtual void NetworkUpdate() = 0;
		virtual void NetworkShutdown() = 0;
		virtual void GetScriptId(rage::scrThread& thread) = 0;
		virtual void CreateScriptHandler(rage::scrThread& thread) = 0;
		virtual void GetScriptHandler(const rage::scriptIdBase& scriptId) = 0;
		virtual void RegisterScript(rage::scrThread& thread) = 0;
		virtual void UnregisterScript(rage::scrThread& thread) = 0;
		virtual void PlayerHasJoined(const rage::netPlayer& player) = 0;
		virtual void PlayerHasLeft(const rage::netPlayer& player) = 0;
		virtual void GetNumRequiredEntities() = 0;
	};
}
