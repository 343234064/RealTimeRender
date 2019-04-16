#include "Launch/LoopWrapper.h"

#if MAIN_LAUNCH_CPP && PLATFORM_WINDOWS  

#include "HAL/Platform.h"
#include "HAL/Chars.h"

#include "Editor/Editor.h"

#include <crtdbg.h>


//Globals
HANDLE gNamedMutex = NULL;


void ReleaseNamedMutex()
{
	if (gNamedMutex)
	{
		ReleaseMutex(gNamedMutex);
		gNamedMutex = NULL;
	}
}


bool CreateNemedMutex()
{
	bool IsFirstInstance = false;
	TCHAR MutexName[14] = TEXT("RT-NamedMutex");

	gNamedMutex = CreateMutex(NULL, true, MutexName);

	if (gNamedMutex && GetLastError() != ERROR_ALREADY_EXISTS)
	{
		IsFirstInstance = true;
	}
	else
	{
		ReleaseNamedMutex();
		IsFirstInstance = false;
	}

	return IsFirstInstance;
}




void InvalidParameterHandler(const wchar_t* Expression, const wchar_t* Function, const wchar_t* File, uint32 Line, uintptr_t Reserved)
{
	//log
}


LONG WINAPI UnhandledException(EXCEPTION_POINTERS* ExceptionInfo)
{
	Platform::ReportCrash(ExceptionInfo);
	return EXCEPTION_CONTINUE_SEARCH;
}


//This is to get the correct callstack when running as 64bit exe
//The inner exception handler catches crashes/asserts in native C++ code
int32 GuardedMainEntrance(HINSTANCE hInstance, HINSTANCE hPrevInstance, int nCmdShow)
{
	int32 Error = 0;
	if (gEnableInnerException)
	{
		__try
		{
			Error = MAIN_ENTRANCE_CALL(hInstance, hPrevInstance, nCmdShow);
		}
		//Inner exception handler
		__except (Platform::ReportCrash(GetExceptionInformation()), EXCEPTION_CONTINUE_SEARCH)
		{
			//This block do not run
			;
		}
	}
	else
	{
		Error = MAIN_ENTRANCE_CALL(hInstance, hPrevInstance, nCmdShow);
	}

	return Error;
}

#if SHOW_DEBUG_CONSOLE //Show the console window
int32 main()
#else
int32 WINAPI WinMain( _In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nCmdShow)
#endif
{
#if SHOW_DEBUG_CONSOLE
	HINSTANCE hInstance = GetModuleHandle(nullptr);
	HINSTANCE hPrevInstance = NULL;
	int32 nCmdShow = 0;
#endif

	//Setup invalid parameter handler and CRT report mode
	_set_invalid_parameter_handler(InvalidParameterHandler);
#if _DEBUG
	_CrtSetReportMode(_CRT_ASSERT, _CRTDBG_MODE_DEBUG);
	//Disable filling buffer
	_CrtSetDebugFillThreshold(0);
#endif

	//Setup globals....
	gIsAppRequestExit = false;
	gReportCrashEvenDebugger = false;
	gEnableInnerException = true;
	gIsFirstInstance = CreateNemedMutex();


	//Cache the hInstance
	gMainInstanceHandle = hInstance;


	int32 Error = 0;
	if (::IsDebuggerPresent() && !gReportCrashEvenDebugger)
	{
		Error = MAIN_ENTRANCE_CALL(hInstance, hPrevInstance, nCmdShow);
	}
	else
	{
		//To catch any exceptions on threads that are not throw by the application code
		SetUnhandledExceptionFilter(UnhandledException);

		//Structured exception handling
		__try
		{
			Error = GuardedMainEntrance(hInstance, hPrevInstance, nCmdShow);
		}
		__except (gEnableInnerException ? EXCEPTION_EXECUTE_HANDLER : Platform::ReportCrash(GetExceptionInformation()))
		{
			ReleaseNamedMutex();
			Error = 1;
			gIsGetError = true;
			//log dump error
			//flush log

			//Kill process directly
			//Note that some destructor may not called
			Platform::RequestExit(true);
		}
	}

	ReleaseNamedMutex();

	return Error;
}







#endif
