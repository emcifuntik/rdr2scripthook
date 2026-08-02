#pragma once

#include "structTools.h"

class CGameScriptHandler;

namespace rage
{
using scrProgramId = uint32_t;

enum eThreadState
{
    ThreadStateIdle,
    ThreadStateRunning,
    ThreadStateKilled,
    ThreadStateUnk3,
    ThreadStateUnk4,
};

class scrThreadContext
{
public:
    uint32_t threadId;
    uint32_t scriptHash;
    eThreadState scriptState;
    uint32_t unused1;
    uint32_t unused2;
    uint32_t unused3;
    uint32_t timerA;
    uint32_t timerB;
    uint32_t timerC;
    uint32_t unk1;
    uint32_t unk2;

private:
    char padding002C[0x34];

public:
    uint32_t unk3;

private:
    char padding0064[0x64C];
};
VALIDATE_SIZE(scrThreadContext, 0x6B0);

class scrThread
{
public:
    scrThreadContext context;
    uintptr_t stack;

private:
    char padding00B8[0x10];

public:
    char* exitMessage;
    uint32_t scriptHash;
    CGameScriptHandler* scriptHandler;

private:
    char padding06E8[0xA0];

public:
    virtual ~scrThread() = default;
    virtual eThreadState Reset(scrProgramId programId, const void* args,
                               int argumentCount) = 0;
    virtual eThreadState Run(int operationCount) = 0;
    virtual eThreadState Update(int operationCount) = 0;
    virtual void Kill() = 0;

    scrThreadContext* GetContext() { return &context; }
    uint32_t GetId() const { return context.threadId; }
};
VALIDATE_SIZE(scrThread, 0x788);
}
