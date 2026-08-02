#pragma once

#include "scrThread.h"

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
    virtual void GetScriptId(scrThread& thread) = 0;
    virtual void CreateScriptHandler(scrThread& thread) = 0;
    virtual void GetScriptHandler(const scriptIdBase& scriptId) = 0;
    virtual void RegisterScript(scrThread& thread) = 0;
    virtual void UnregisterScript(scrThread& thread) = 0;
    virtual void PlayerHasJoined(const netPlayer& player) = 0;
    virtual void PlayerHasLeft(const netPlayer& player) = 0;
    virtual void GetNumRequiredEntities() = 0;
};
}
