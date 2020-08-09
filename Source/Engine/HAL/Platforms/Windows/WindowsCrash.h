#pragma once

#include "HAL/Platforms/GenericCrash.h"
#include "HAL/Platforms/Windows/WindowsPlatform.h"
#include "HAL/Platforms/Windows/WindowsProcess.h"


const uint32 AssertExceptionCode = 0x4000;
const uint32 GPUCrashExceptionCode = 0x8000;






/************************************
Seperate thread for crash reporting 

a thread is necessary in case that the main thread is corrupted
*************************************/
class CrashReportThread
{
public:
	CrashReportThread();
	~CrashReportThread();

private:
	static DWORD CrashReportingThreadProc(LPVOID pThis)
	{
		CrashReportThread* This = (CrashReportThread*)pThis;
		return This->Run();
	}

	PlatformTypes::int32 Run();

	void Stop()
	{
		StopCounter.Increment();
	}

	void HandleCrash();

private:
	DWORD ThreadID;
	HANDLE ThreadHandle;

	DWORD CrashedThreadID;
	HANDLE CrashedThreadHandle;

	/* Signal that the application has crashed */
	HANDLE CrashEvent;
	/* Event that signals the reporting thread has finished processing the crash */
	HANDLE CrashHandledEvent;

	HANDLE VectoredExceptionHandle;

	AtomicCounter<PlatformTypes::int32> StopCounter;

	/** Exception information */
	LPEXCEPTION_POINTERS CachedExceptionInfo;

	/* Process handle to crash reporter client */
	PlatformProcessHandle CrashClientHandle;
	void* CrashClientWritePipe;

	SharedCrashContext SharedContext;
};



struct PlatformCrash
{

};