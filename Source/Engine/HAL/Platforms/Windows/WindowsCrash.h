#pragma once

#include "Global/Utilities/AtomicCounter.h"
#include "Global/EngineVariables.h"
#include "HAL/Platforms/Windows/WindowsPlatform.h"
#include "HAL/Platforms/GenericPlatform.h"
#include "Log/LogManager.h"


class CrashReportThread;

const uint32 AssertExceptionCode = 0x4000;
const uint32 GPUCrashExceptionCode = 0x8000;


struct CrashInfo
{
	const TChar* Message;

	CrashInfo(const TChar* InMessage) :
		Message(InMessage)
	{
	}
};


String GetExceptionInfoString(EXCEPTION_RECORD* ExceptionRecord)
{
	String Message = TEXTS("Unhandled Exception:");

	switch (ExceptionRecord->ExceptionCode)
	{
	case EXCEPTION_ACCESS_VIOLATION:
		Message += TEXTS("EXCEPTION_ACCESS_VIOLATION \n");
		if (ExceptionRecord->ExceptionInformation[0] == 0)
		{
			Message += TEXTS("Reading address: ");
		}
		else if (ExceptionRecord->ExceptionInformation[0] == 1)
		{
			Message += TEXTS("Writing address: ");
		}
		Message += String::Sprintf(TEXTS("0x%08x"), (uint32)ExceptionRecord->ExceptionInformation[1]);
		break;
	case EXCEPTION_ARRAY_BOUNDS_EXCEEDED:
		Message += TEXTS("EXCEPTION_ARRAY_BOUNDS_EXCEEDED");
		break;
	case EXCEPTION_DATATYPE_MISALIGNMENT:
		Message += TEXTS("EXCEPTION_DATATYPE_MISALIGNMENT");
		break;
	case EXCEPTION_FLT_DENORMAL_OPERAND:
		Message += TEXTS("EXCEPTION_FLT_DENORMAL_OPERAND");
		break;
	case EXCEPTION_FLT_DIVIDE_BY_ZERO:
		Message += TEXTS("EXCEPTION_FLT_DIVIDE_BY_ZERO");
		break;
	case EXCEPTION_FLT_INVALID_OPERATION:
		Message += TEXTS("EXCEPTION_FLT_INVALID_OPERATION");
		break;
	case EXCEPTION_ILLEGAL_INSTRUCTION:
		Message += TEXTS("EXCEPTION_ILLEGAL_INSTRUCTION");
		break;
	case EXCEPTION_INT_DIVIDE_BY_ZERO:
		Message += TEXTS("EXCEPTION_INT_DIVIDE_BY_ZERO");
		break;
	case EXCEPTION_PRIV_INSTRUCTION:
		Message += TEXTS("EXCEPTION_PRIV_INSTRUCTION");
		break;
	case EXCEPTION_STACK_OVERFLOW:
		Message += TEXTS("EXCEPTION_STACK_OVERFLOW");
		break;
	default:
		Message += String::Sprintf(TEXTS("0x%08x"), (uint32)ExceptionRecord->ExceptionCode);
	}

	return Message;
}


LONG WINAPI UnhandledStaticInitializationException(LPEXCEPTION_POINTERS ExceptionInfo)
{
	// Report fatal exceptions
	if ((ExceptionInfo->ExceptionRecord->ExceptionCode & 0x80000000L) != 0)
	{
		// Using another thread solely for the purpose of handling crashes
		// https://peteronprogramming.wordpress.com/2016/08/10/crashes-you-cant-handle-easily-2-stack-overflows-on-windows/
		if (gCrashReportThread.)
		{
			return gCrashReportThread->OnCrashDuringStaticInitialization(ExceptionInfo);
		}
	}

	return EXCEPTION_CONTINUE_SEARCH;
}


/************************************
Seperate thread for crash reporting 

a thread is necessary in case that the main thread is corrupted
*************************************/
class CrashReportThread
{
public:
	CrashReportThread():
		ThreadID(0),
		ThreadHandle(nullptr),
		CrashedThreadID(0),
		CrashedThreadHandle(nullptr),
		VectoredExceptionHandle(nullptr),
		StopCounter(0),
		CachedExceptionInfo(nullptr)
	{
		CrashEvent = CreateEvent(nullptr, true, false, nullptr);
		CrashHandledEvent = CreateEvent(nullptr, true, false, nullptr);

		// Add an exception handler to catch issues during static initialization. This
		// is removed once the reporter thread is started.
		if (!Platform::IsDebuggerPresent())
		{
			VectoredExceptionHandle = AddVectoredExceptionHandler(1, UnhandledStaticInitializationException);
		}

		ThreadHandle = CreateThread(NULL, 0, CrashReportingThreadProc, this, 0, &ThreadID);
		if (ThreadHandle)
		{
			SetThreadPriority(ThreadHandle, THREAD_PRIORITY_BELOW_NORMAL);
		}

	}

	~CrashReportThread()
	{
		if (ThreadHandle)
		{
			Stop();

			if (WaitForSingleObject(ThreadHandle, 1000) == WAIT_OBJECT_0)
			{
				CloseHandle(ThreadHandle);
			}
			ThreadHandle = nullptr;
		}

		CloseHandle(CrashEvent);
		CrashEvent = nullptr;

		CloseHandle(CrashHandledEvent);
		CrashHandledEvent = nullptr;
	}


private:
	static DWORD CrashReportingThreadProc(LPVOID pThis)
	{
		CrashReportThread* This = (CrashReportThread*)pThis;
		return This->Run();
	}

	FORCE_NO_INLINE PlatformTypes::int32 Run()
	{
		// Removed vectored exception handler, we are now guaranteed to
		// be able to catch unhandled exception in the main try/catch block.
		if (!Platform::IsDebuggerPresent())
		{
			RemoveVectoredExceptionHandler(VectoredExceptionHandle);
		}

		while (StopCounter.GetCounter() == 0)
		{
			if (WaitForSingleObject(CrashEvent, 500) == WAIT_OBJECT_0)
			{
				ResetEvent(CrashHandledEvent);
				HandleCrash();
				ResetEvent(CrashEvent);
				SetEvent(CrashHandledEvent);
				break;
			}
		}

		return 0;
	}

	void Stop()
	{
		StopCounter.Increment();
	}

	FORCE_NO_INLINE void HandleCrash()
	{
		gLogger.FlushThreadedLogs();

		CrashType Type = CrashType::Crash;
		String Message = TEXTS("Unhandled Exception");

		if (CachedExceptionInfo->ExceptionRecord->ExceptionCode == AssertExceptionCode && CachedExceptionInfo->ExceptionRecord->NumberParameters == 1)
		{
			Type = CrashType::Assert;
			CrashInfo Info = *(CrashInfo*)CachedExceptionInfo->ExceptionRecord->ExceptionInformation[0];
			Message = Info.Message;
		}
		else if (CachedExceptionInfo->ExceptionRecord->ExceptionCode == GPUCrashExceptionCode && CachedExceptionInfo->ExceptionRecord->NumberParameters == 1)
		{
			Type = CrashType::GPUCrash;
			CrashInfo Info = *(CrashInfo*)CachedExceptionInfo->ExceptionRecord->ExceptionInformation[0];
			Message = Info.Message;
		}
		else if (CachedExceptionInfo->ExceptionRecord->ExceptionCode != 1)
		{
			Message = GetExceptionInfoString();
		}


	}

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




};