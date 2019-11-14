#pragma once
#include "scrThread.h"

class GtaThread : public rage::scrThread
{
	class PushState
	{
	public:
		PushState(rage::scrThread* thread);
		~PushState();

	private:
		rage::scrThread* prevThread;
	};

public:
	virtual ~GtaThread() = default;

	PushState Push() { return PushState{ this }; };

	rage::eThreadState Reset(rage::scrProgramId scriptHash, void const* pArgs, int argCount) override;
	rage::eThreadState Run(int opsToExecute) override;
	rage::eThreadState Update(int opsToExecute) override;
	void Kill() override;

	virtual void Execute() = 0;

	inline void * GetScriptHandler() { return scriptHandler; }
};
VALIDATE_SIZE(GtaThread, 0x788);
