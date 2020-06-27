/************************************
Windows thread


*************************************/
#pragma once


#include "HAL/Platforms/GenericThread.h"
#include "HAL/Platforms/Windows/WindowsPlatform.h"


class WindowsThread: public Thread
{
public:
	static Thread* CreateThread()
	{
		return new WindowsThread();
	}

	virtual ~WindowsThread() 
	{
		if (ThreadHandle != NULL)
		{
			Kill(true);
		}
	}


	void SetThreadPriority(ThreadPriority PriorityToSet) override
	{
	    Priority = PriorityToSet;

		int32 PlatformPriority = 0;
		//Here remapped the the normal priority to above normal 
		switch (PriorityToSet)
		{
		case ThreadPriority::AboveNormal: PlatformPriority = THREAD_PRIORITY_HIGHEST;break;
		case ThreadPriority::BelowNormal: PlatformPriority = THREAD_PRIORITY_NORMAL; break;
		case ThreadPriority::Highest:     PlatformPriority = THREAD_PRIORITY_HIGHEST;break;
		case ThreadPriority::Lowest:      PlatformPriority = THREAD_PRIORITY_LOWEST; break;
		default:
			PlatformPriority = THREAD_PRIORITY_ABOVE_NORMAL;  
		}

		::SetThreadPriority(ThreadHandle, PlatformPriority);
	}

	void Pause() override
	{
		CHECK(ThreadHandle != NULL);
		::SuspendThread(ThreadHandle);
	}

	void Resume() override
	{
		CHECK(ThreadHandle != NULL);
		::ResumeThread(ThreadHandle);
	}

	bool Kill(bool WaitUntilExit = true) override;

	void WaitForComplete() override
	{
		::WaitForSingleObject(ThreadHandle, INFINITE);
	}

protected:
	WindowsThread() :
		ThreadHandle(NULL),
		SyncEvent(NULL)
	{}

	uint32 RunWrapper();
	uint32 Run();

	bool PlatformInit(Runnable* ObjectToRun,
		              const ANSICHAR* InitThreadName,
		              uint32 InitStackSize = 0,
		              ThreadPriority InitPriority = ThreadPriority::Normal,
		              uint64 AffinityMask = PlatformAffinity::GetNormalThradMask()) override;

	static DWORD ThreadEntrance(LPVOID Object)
	{
		CHECK(Object != nullptr);
		return ((WindowsThread*)Object)->RunWrapper();
	}

	static void SetThreadName(uint32 ThreadId, LPCSTR Name);


protected:
	HANDLE ThreadHandle;
	HANDLE SyncEvent;
};


typedef WindowsThread PlatformThread;