/************************************
Thread relatives


*************************************/
#pragma once

#include "Global/Utilities/String.h"
#include "Global/GlobalType.h"
#include "HAL/Platform.h"


struct PlatformAffinity
{
	static const uint64 GetNormalThradMask()
	{
		return 0xFFFFFFFFFFFFFFFF;
	}

	static const uint64 GetMainThreadMask()
	{
		return 0xFFFFFFFFFFFFFFFF;
	}

	static const uint64 GetRenderThreadMask()
	{
		return 0xFFFFFFFFFFFFFFFF;
	}

};


enum class ThreadPriority
{
	Normal = 0,

	AboveNormal,

	BelowNormal,

	Highest,

	Lowest
};




/************************************
Base runnable interface
*************************************/
class Runnable
{
public:
	Runnable() {}
	virtual ~Runnable() {}

	virtual bool Init() { return true; }

	//Return exit code
	virtual uint32 Run() {}

	virtual void Stop() {}
	virtual void Exit() {}
};




/************************************
Base Thread interface
*************************************/
class Thread
{
public:
	static Thread* Create(Runnable* ObjectToRun,
		                  const ANSICHAR* InitThreadName,
		                  uint32 InitStackSize = 0,
		                  ThreadPriority InitPriority = ThreadPriority::Normal,
		                  uint64 AffinityMask = PlatformAffinity::GetNormalThradMask());

	virtual ~Thread() {}


	virtual void SetThreadPriority(ThreadPriority Priority) = 0;
	virtual void Pause() = 0;
	virtual void Resume() = 0;
	virtual bool Kill(bool WaitUntilExit = true) = 0;
	virtual void WaitForComplete() = 0;

	virtual const uint32 GetThreadID() const
	{
		return ThreadID;
	}


protected:
	Thread():
		RunObject(nullptr),
		ThreadAffinityMask(PlatformAffinity::GetNormalThradMask()),
		Priority(ThreadPriority::Normal),
		ThreadID(0)
	{}


	virtual bool PlatformInit(Runnable* ObjectToRun,
		                      const ANSICHAR* InitThreadName,
		                      uint32 InitStackSize = 0,
		                      ThreadPriority InitPriority = ThreadPriority::Normal,
		                      uint64 AffinityMask = PlatformAffinity::GetNormalThradMask()) = 0;


protected:
	Runnable* RunObject;
	
	uint64 ThreadAffinityMask;
	
	ThreadPriority Priority;
	
	uint32 ThreadID;

};


