#include "Global/EngineVariables.h"
#include "Global/Utilities/AtomicCounter.h"
#include "Global/Utilities/CharConversion.h"
#include "HAL/Platforms/GenericPlatform.h"
#include "HAL/Platforms/Windows/WindowsCrash.h"
#include "Log/LogManager.h"

#include <TlHelp32.h>

#define CRASH_CLIENT_MAX_PATH_LEN 265
#define CRASH_CLIENT_MAX_ARGS_LEN 256


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


int32 ReportCrashToReporterClient(
	LPEXCEPTION_POINTERS ExceptionInfo,
	CrashType Type,
	const TChar* ErrorMessage,
	HANDLE CrashingThreadHandle,
	DWORD CrashingThreadID,
	PlatformProcessHandle& CrashReporterHandle,
	void* WritePipe,
	SharedCrashContext* SharedContext
)
{
	SharedContext->CrashType = Type;
	SharedContext->CrashingThreadId = CrashingThreadID;
	PlatformChars::Strcpy(SharedContext->ErrorMessage, MAX_ERROR_MESSAGE_CHARS - 1, ErrorMessage);

	DWORD CurrentProcessId = ::GetCurrentProcessId();
	DWORD CurrentThreadId = ::GetCurrentThreadId();
	HANDLE ThreadSnapshot = ::CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0);
	Array<HANDLE> ThreadHandles;
	if (ThreadSnapshot != INVALID_HANDLE_VALUE)
	{
		::THREADENTRY32 ThreadEntry;
		ThreadEntry.dwSize = sizeof(THREADENTRY32);
		if (::Thread32First(ThreadSnapshot, &ThreadEntry))
		{
			do
			{
				if (ThreadEntry.th32OwnerProcessID == CurrentProcessId)
				{
					// Stop the thread (except current!). We will resume once the crash reporter is done.
					if (ThreadEntry.th32ThreadID != CurrentThreadId)
					{
						HANDLE ThreadHandle = ::OpenThread(THREAD_SUSPEND_RESUME, FALSE, ThreadEntry.th32ThreadID);
						::SuspendThread(ThreadHandle);
						ThreadHandles.Push(ThreadHandle);
					}

					SharedContext->ThreadIds[ThreadIdx] = ThreadEntry.th32ThreadID;
					const TCHAR* ThreadName = nullptr;
					if (ThreadEntry.th32ThreadID == gMainThreadId)
					{
						ThreadName = TEXT("MainThread");
					}
					else if (bThreadManagerAvailable)
					{
						const String& TmThreadName = FThreadManager::Get().GetThreadName(ThreadEntry.th32ThreadID);
						ThreadName = TmThreadName.Len() ? TEXT("Unknown") : *TmThreadName;
					}
					else
					{
						ThreadName = TEXT("Unavailable");
					}
					PlatformChars::Strcpy(
						&SharedContext->ThreadNames[ThreadIdx * MAX_THREAD_NAME_CHARS],
						MAX_THREAD_NAME_CHARS - 1,
						ThreadName
					);
					ThreadIdx++;
				}
			} while (::Thread32Next(ThreadSnapshot, &ThreadEntry) && (ThreadIdx < MAX_THREADS));
		}
	}

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



PlatformProcessHandle LaunchCrashReportClient(void** OutReadPipe, void** OutWritePipe)
{
	TChar ClientPath[CRASH_CLIENT_MAX_PATH_LEN] = { 0 };
	TChar ClientArgs[CRASH_CLIENT_MAX_ARGS_LEN] = { 0 };

	void* PipeInRead = nullptr;
	void* PipeInWrite = nullptr;
	void* PipeOutRead = nullptr;
	void* PipeOutWrite = nullptr;

	if (OutReadPipe)
	{
		if (!PlatformProcess::CreatePipe(PipeOutRead, PipeOutWrite))
		{
			return PlatformProcessHandle();
		}
		else
		{
			*OutReadPipe = PipeOutRead;
		}
	}

	if (OutWritePipe)
	{
		if (!PlatformProcess::CreatePipe(PipeInRead, PipeInWrite))
		{
			return PlatformProcessHandle();
		}
		else
		{
			*OutWritePipe = PipeInWrite;
		}
	}

	String::Sprintf(ClientArgs, TEXTS(" -READ=%0u -WRITE=%0u -MAINPROCID=%u"), PipeInRead, PipeOutWrite, PlatformProcess::GetCurrentProcessId());
	String CrashReporterPath = TEXTS("CrashReporter_Win64.exe");
	
	return PlatformProcess::CreateProc(
		*CrashReporterPath,
		ClientArgs,
		true, false, false,
		nullptr,
		0,
		nullptr,
		nullptr,
		nullptr
	);
}



CrashReportThread::CrashReportThread() :
	ThreadID(0),
	ThreadHandle(nullptr),
	CrashedThreadID(0),
	CrashedThreadHandle(nullptr),
	VectoredExceptionHandle(nullptr),
	StopCounter(0),
	CachedExceptionInfo(nullptr),
	CrashClientWritePipe(nullptr)
{
	CrashEvent = CreateEvent(nullptr, true, false, nullptr);
	CrashHandledEvent = CreateEvent(nullptr, true, false, nullptr);

	// Add an exception handler to catch issues during static initialization. This
	// is removed once the reporter thread is started.
	if (!Platform::IsDebuggerPresent())
	{
		VectoredExceptionHandle = AddVectoredExceptionHandler(1, UnhandledStaticInitializationException);
	}

	CrashClientHandle = LaunchCrashReportClient(nullptr, &CrashClientWritePipe);

	ThreadHandle = CreateThread(NULL, 0, CrashReportingThreadProc, this, 0, &ThreadID);
	if (ThreadHandle)
	{
		SetThreadPriority(ThreadHandle, THREAD_PRIORITY_BELOW_NORMAL);
	}
	
	Memory::Zero(&SharedContext, sizeof(SharedCrashContext));
}


CrashReportThread::~CrashReportThread()
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



PlatformTypes::int32 CrashReportThread::Run()
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


void CrashReportThread::HandleCrash()
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
		Message = GetExceptionInfoString(CachedExceptionInfo->ExceptionRecord);
	}

	// Generate crash callstack
	{
		const Size_T StackTraceSize = 65535;
		ANSICHAR* StackTrace = (ANSICHAR*)gMallocator->Malloc(StackTraceSize);
		StackTrace[0] = 0;

		PlatformChars::Strncpy(gErrorHist, *Message, ARRAY_SIZE(gErrorHist));
		PlatformChars::Strncat(gErrorHist, TEXTS("\r\n\r\n"), ARRAY_SIZE(gErrorHist));

		//Stack Walk and dump

		Array<TChar> ConvertedChars;
		UTF8ToTChar::Convert(ConvertedChars, StackTrace);
		PlatformChars::Strncat(gErrorHist, ConvertedChars.Begin(), ARRAY_SIZE(gErrorHist));

		gMallocator->Free(StackTrace);
	}

	// Run the crash reporter
	if (CrashClientHandle.IsValid() && PlatformProcess::IsProcRunning(CrashClientHandle))
	{
		ReportCrashToReporterClient();
	}




}