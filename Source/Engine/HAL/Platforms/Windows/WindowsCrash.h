#pragma once

#include "HAL/Platforms/GenericCrash.h"
#include "HAL/Platforms/Windows/WindowsPlatform.h"
#include "HAL/Platforms/Windows/WindowsProcess.h"







/************************************
Seperate thread for crash reporting 

a thread is necessary in case that the main thread is corrupted
*************************************/
class CrashReportThread
{
public:
	CrashReportThread();
	~CrashReportThread();


	int32 OnEnsure(LPEXCEPTION_POINTERS ExceptionInfo, const TChar* ErrorMessage);

	void OnCrashed(LPEXCEPTION_POINTERS ExceptionInfo)
	{
		CachedExceptionInfo = ExceptionInfo;
		CrashedThreadHandle = ::GetCurrentThread();
		CrashedThreadID = ::GetCurrentThreadId();
		::SetEvent(CrashEvent);
	}

	int32 OnCrashDuringStaticInit(LPEXCEPTION_POINTERS ExceptionInfo);

	bool WaitUntilCrashIsHandled()
	{
		// Wait 60s
		return ::WaitForSingleObject(CrashHandledEvent, 60000) == WAIT_OBJECT_0;
	}

	bool IsValid() const { return Valid; }

	CrashReportThread(CrashReportThread&&) = delete;
	CrashReportThread& operator=(const CrashReportThread& Other) = delete;
	CrashReportThread& operator=(CrashReportThread&&) = delete;

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
	void* CrashClientReadPipe;

	SharedCrashContext SharedContext;

	bool Valid;
};



struct WindowsCrash
{
	static int32 Report(EXCEPTION_POINTERS* ExceptionInfo);
	static int32 ReportEnsureUsingCrashReportClient(EXCEPTION_POINTERS* ExceptionInfo, const TChar* ErrorMessage);
	static void ReportEnsure(const TChar* ErrorMessage);
	static void ReportAssert(const TChar* ErrorMessage);
	static void ReportGPUCrash(const TChar* ErrorMessage);
	//static void ReportHang(const TChar* ErrorMessage, uint32 HungThreadId);

};

typedef WindowsCrash PlatformCrash;