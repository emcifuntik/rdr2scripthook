#include "stdafx.h"

#include "GtaThread.h"
#include "CScriptManager.h"

static void(*ScriptThreadInit)(GtaThread*);
static rage::eThreadState(*ScriptThreadTick)(GtaThread*, uint32_t);

static CMemory::Hook scrHook([] {
	constexpr CMemory::Pattern initScrThreadPat("48 89 5C 24 ? 57 48 83 EC ? 83 89 ? ? ? ? ? 33 FF 83 A1");
	constexpr CMemory::Pattern gtaThreadTickPat("48 89 5C 24 ? 57 48 83 EC ? 80 B9 ? ? ? ? ? 8B FA 48 8B D9 74 ? 8B 41");

	ScriptThreadInit = initScrThreadPat.Search().Get<void(*)(GtaThread*)>();
	ScriptThreadTick = gtaThreadTickPat.Search().Get<rage::eThreadState(*)(GtaThread*, uint32_t)>();
});

rage::eThreadState GtaThread::Update(int opsToExecute)
{
	return ScriptThreadTick(this, opsToExecute);
}

void GtaThread::Kill()
{
	constexpr CMemory::Pattern gtaThreadKillPat("48 89 5C 24 ? 48 89 74 24 ? 57 48 83 EC ? 48 8B F9 8B 49");
	static auto gtaThreadKill = gtaThreadKillPat.Search().Get<void(*)(GtaThread*)>();
	return gtaThreadKill(this);
}

rage::eThreadState GtaThread::Run(int opsToExecute)
{
	auto state = Push();

	if (context.scriptState != rage::ThreadStateKilled)
		Execute();

	return context.scriptState;
}

rage::eThreadState GtaThread::Reset(rage::scrProgramId hash, void const* pArgs, int argCount)
{
	memset(&context, 0, sizeof(context));

	context.scriptState = rage::eThreadState::ThreadStateIdle;
	context.scriptHash = hash;
	context.unk1 = -1;
	context.unk2 = -1;
	context.unk3 = 1;

	*(uint64_t*)(this + 0x720) = 0i64;
	*(uint32_t*)(this + 0x728) = 0;
	*(uint64_t*)(this + 0x730) = 0i64;
	*(uint32_t*)(this + 0x738) = 0;
	*(uint64_t*)(this + 0x740) = 0i64;
	*(uint32_t*)(this + 0x748) = 0;
	*(uint64_t*)(this + 0x750) = 0i64;
	*(uint32_t*)(this + 0x758) = 0;
	*(uint64_t*)(this + 0x760) = 0i64;
	*(uint32_t*)(this + 0x768) = 0;
	*(uint8_t*)(this + 0x770) = 0;
	*(uint64_t*)(this + 0x6E0) = 0i64;
	*(bool*)(this + 0x71A) = false;

	ScriptThreadInit(this);

	exitMessage = (char*)"Not aborted yet?";
	
	context.threadId = hash;

	CScriptManager::Instance().GetScriptHandleMgr()->RegisterScript(*this);
	return context.scriptState;
}

GtaThread::PushState::PushState(rage::scrThread* thread)
{
	prevThread = CScriptManager::Instance().GetActiveThread();
	CScriptManager::Instance().SetActiveThread(thread);
}

GtaThread::PushState::~PushState()
{
	CScriptManager::Instance().SetActiveThread(prevThread);
}
