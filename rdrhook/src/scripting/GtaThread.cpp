#include "stdafx.h"

#include "GtaThread.h"

#include "CScriptManager.h"

namespace
{
void (*InitializeScriptThread)(GtaThread*) = nullptr;
rage::eThreadState (*TickScriptThread)(GtaThread*, uint32_t) = nullptr;

CMemory::Hook ScriptThreadHooks([] {
    constexpr CMemory::Pattern initializePattern(
        "48 89 5C 24 ? 57 48 83 EC ? 83 89 ? ? ? ? ? 33 FF 83 A1");
    constexpr CMemory::Pattern tickPattern(
        "48 89 5C 24 ? 57 48 83 EC ? 80 B9 ? ? ? ? ? 8B FA 48 8B D9 74 ? 8B 41");

    InitializeScriptThread =
        initializePattern.Search().Get<decltype(InitializeScriptThread)>();
    TickScriptThread = tickPattern.Search().Get<decltype(TickScriptThread)>();
});
}

rage::eThreadState GtaThread::Update(int operationCount)
{
    return TickScriptThread(this, operationCount);
}

void GtaThread::Kill()
{
    constexpr CMemory::Pattern killPattern(
        "48 89 5C 24 ? 48 89 74 24 ? 57 48 83 EC ? 48 8B F9 8B 49");
    static auto killScriptThread =
        killPattern.Search().Get<void (*)(GtaThread*)>();
    killScriptThread(this);
}

rage::eThreadState GtaThread::Run(int)
{
    auto activeThread = Activate();
    if (context.scriptState != rage::ThreadStateKilled)
        Execute();
    return context.scriptState;
}

rage::eThreadState GtaThread::Reset(rage::scrProgramId hash, const void*, int)
{
    std::memset(&context, 0, sizeof(context));
    context.scriptState = rage::ThreadStateIdle;
    context.scriptHash = hash;
    context.unk1 = -1;
    context.unk2 = -1;
    context.unk3 = 1;

    *reinterpret_cast<uint64_t*>(reinterpret_cast<uintptr_t>(this) + 0x720) = 0;
    *reinterpret_cast<uint32_t*>(reinterpret_cast<uintptr_t>(this) + 0x728) = 0;
    *reinterpret_cast<uint64_t*>(reinterpret_cast<uintptr_t>(this) + 0x730) = 0;
    *reinterpret_cast<uint32_t*>(reinterpret_cast<uintptr_t>(this) + 0x738) = 0;
    *reinterpret_cast<uint64_t*>(reinterpret_cast<uintptr_t>(this) + 0x740) = 0;
    *reinterpret_cast<uint32_t*>(reinterpret_cast<uintptr_t>(this) + 0x748) = 0;
    *reinterpret_cast<uint64_t*>(reinterpret_cast<uintptr_t>(this) + 0x750) = 0;
    *reinterpret_cast<uint32_t*>(reinterpret_cast<uintptr_t>(this) + 0x758) = 0;
    *reinterpret_cast<uint64_t*>(reinterpret_cast<uintptr_t>(this) + 0x760) = 0;
    *reinterpret_cast<uint32_t*>(reinterpret_cast<uintptr_t>(this) + 0x768) = 0;
    *reinterpret_cast<uint8_t*>(reinterpret_cast<uintptr_t>(this) + 0x770) = 0;
    *reinterpret_cast<uint64_t*>(reinterpret_cast<uintptr_t>(this) + 0x6E0) = 0;
    *reinterpret_cast<bool*>(reinterpret_cast<uintptr_t>(this) + 0x71A) = false;

    InitializeScriptThread(this);
    exitMessage = const_cast<char*>("Not aborted yet?");
    context.threadId = hash;

    CScriptManager::Instance().GetScriptHandlerManager()->RegisterScript(*this);
    return context.scriptState;
}

GtaThread::ScopedActiveThread::ScopedActiveThread(rage::scrThread* thread)
    : previousThread(CScriptManager::Instance().GetActiveThread())
{
    CScriptManager::Instance().SetActiveThread(thread);
}

GtaThread::ScopedActiveThread::~ScopedActiveThread()
{
    CScriptManager::Instance().SetActiveThread(previousThread);
}
