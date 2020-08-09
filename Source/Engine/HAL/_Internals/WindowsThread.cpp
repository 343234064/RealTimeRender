#include "Global/EngineVariables.h"
#include "Global/Utilities/CharConversion.h"
#include "HAL/Platforms/Windows/WindowsThread.h"


bool WindowsThread::PlatformInit(Runnable* ObjectToRun,
	                             const TChar* InitThreadName,
	                             uint32 InitStackSize,
	                             ThreadPriority InitPriority,
	                             uint64 AffinityMask)
{
	static bool SetMainThreadPri = false;
	if (!SetMainThreadPri)
	{
		SetMainThreadPri = true;
		::SetThreadPriority(::GetCurrentThread(), THREAD_PRIORITY_ABOVE_NORMAL);
	}

	CHECK(ObjectToRun != nullptr);
    
	RunObject = ObjectToRun;
	ThreadAffinityMask = AffinityMask;

	ThreadName = InitThreadName ? InitThreadName : TEXTS("RTR Unnamed Thread");

	//Create auto reset sync event
	SyncEvent = ::CreateEvent(NULL, false, 0, nullptr);
	CHECK(SyncEvent != NULL);

	ThreadHandle = ::CreateThread(NULL, InitStackSize, ThreadEntrance, this, STACK_SIZE_PARAM_IS_A_RESERVATION | CREATE_SUSPENDED, (DWORD*)&ThreadID);

	if (ThreadHandle == NULL)
	{
		RunObject = nullptr;
	}
	else
	{
		::SetThreadDescription(ThreadHandle, *ThreadName);

		::ResumeThread(ThreadHandle);

		//Here will wait for Runnable's Init() finish
		::WaitForSingleObject(SyncEvent, INFINITE);


		SetThreadName(ThreadID, *ThreadName);
		SetThreadPriority(InitPriority);
	}

	return ThreadHandle != NULL;
}


/*******************
//Set Thread Name
//http://msdn.microsoft.com/en-us/library/xcb2z8hs.aspx
*******************/
void WindowsThread::SetThreadName(uint32 ThreadId, TChar* Name)
{

	const DWORD MS_VC_EXCEPTION = 0x406D1388;

	typedef struct tagTHREADNAME_INFO
	{
		DWORD dwType; // Must be 0x1000.  
		LPCSTR szName; // Pointer to name (in user addr space).  
		DWORD dwThreadID; // Thread ID (-1=caller thread).  
		DWORD dwFlags; // Reserved for future use, must be zero.  
	} THREADNAME_INFO;


	Array<ANSICHAR> ConvertedChars;
	TCharToUTF8::Convert(ConvertedChars, Name);

	THREADNAME_INFO info;
	info.dwType = 0x1000;
	info.szName = ConvertedChars.Begin();
	info.dwThreadID = ThreadId;
	info.dwFlags = 0;

	__try {

		::RaiseException(MS_VC_EXCEPTION, 0, sizeof(info) / sizeof(ULONG_PTR), (ULONG_PTR*)&info);
	}
	__except (EXCEPTION_EXECUTE_HANDLER) {
	}

}


bool WindowsThread::Kill(bool WaitUntilExit)
{
	CHECK(ThreadHandle != NULL);

	if (RunObject)
	{
		RunObject->Stop();
	}

	if (WaitUntilExit == true)
	{
		WaitForSingleObject(ThreadHandle, INFINITE);
	}

	if (SyncEvent != NULL)
	{
		CloseHandle(SyncEvent);
		SyncEvent = NULL;
	}

	CloseHandle(ThreadHandle);
	ThreadHandle = NULL;



	return true;
}




uint32 WindowsThread::RunWrapper()
{
	uint32 Result = 0;

	Platform::SetCurrentThreadAffinityMask(ThreadAffinityMask);

	if (::IsDebuggerPresent())
	{
		Result = Run();
	}
	else
	{

		//Structured exception handling
		__try
		{
			Result = Run();
		}
		__except (Platform::ReportCrash(GetExceptionInformation()))
		{
			Result = 1;
			LOG(Error, WindowsThread, TEXTS("Runnable thread %s crashed."), *ThreadName);

			//log dump error
			//flush log

			PlatformChars::Strncpy(gErrorHist, TEXTS("\nCrash in runnable thread"), ARRAY_SIZE(gErrorHist));
			PlatformChars::Strncat(gErrorHist, TEXTS("\r\n\r\n"), ARRAY_SIZE(gErrorHist));
			
			//gFatal->HandleError()
			Platform::RequestExit(true);
		}
	}

	return Result;
}



uint32 WindowsThread::Run()
{
	CHECK(RunObject != nullptr);
	CHECK(SyncEvent != NULL);
	uint32 Result = 1;

	if (RunObject->Init())
	{
		//Set waiting point here
		::SetEvent(SyncEvent);

		Result = RunObject->Run();
		
		RunObject->Exit();
	}
	else
	{
		//Set waiting point here
		::SetEvent(SyncEvent);
	}
	
	return Result;
}