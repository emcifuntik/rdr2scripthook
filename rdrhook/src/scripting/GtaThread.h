#pragma once

#include "scrThread.h"

class GtaThread : public rage::scrThread
{
public:
    class ScopedActiveThread
    {
    public:
        explicit ScopedActiveThread(rage::scrThread* thread);
        ~ScopedActiveThread();

        ScopedActiveThread(const ScopedActiveThread&) = delete;
        ScopedActiveThread& operator=(const ScopedActiveThread&) = delete;

    private:
        rage::scrThread* previousThread;
    };

    virtual ~GtaThread() = default;

    ScopedActiveThread Activate() { return ScopedActiveThread(this); }

    rage::eThreadState Reset(rage::scrProgramId scriptHash, const void* arguments,
                             int argumentCount) override;
    rage::eThreadState Run(int operationCount) override;
    rage::eThreadState Update(int operationCount) override;
    void Kill() override;

    virtual void Execute() = 0;
};
VALIDATE_SIZE(GtaThread, 0x788);
