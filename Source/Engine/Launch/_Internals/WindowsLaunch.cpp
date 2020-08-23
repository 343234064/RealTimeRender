#include "Launch/LoopWrapper.h"

#if MAIN_LAUNCH_CPP && PLATFORM_WINDOWS  

#include "HAL/Platform.h"
#include "HAL/Chars.h"
#include "HAL/Crash.h"
#include "Log/LogMacros.h"
#include "Editor/Editor.h"

#if _DEBUG
#include <crtdbg.h>
#endif

//Request for high performance NVIDIA/AMD gpu if there are multiples gpus
//http://developer.download.nvidia.com/devzone/devcenter/gamegraphics/files/OptimusRenderingPolicies.pdf
extern "C" { _declspec(dllexport) uint32 NvOptimusEnablement = 0x00000001; }
extern "C" { _declspec(dllexport) uint32 AmdPowerXpressRequestHighPerformance = 0x00000001; }


//To detect wheather this is the first instance
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
	LOG(Fatal, WindowsMainEntrance, TEXTS("Invalid parameter detected:\n Expression: %s\n Function: %s\n File: %s\n Line: %d"), 
		Expression ? Expression : TEXTS("Unknown"), 
		Function ? Function : TEXTS("Unknown"),
		File ? File : TEXTS("Unknown"),
		Line);
}




//This is to get the correct callstack when running as 64bit exe
//The inner exception handler catches crashes/asserts in native C++ code
int32 GuardedMainEntrance(HINSTANCE hInstance, HINSTANCE hPrevInstance, int nCmdShow)
{
	int32 Error = 0;

	{
		__try
		{
			Error = MAIN_ENTRANCE_CALL(hInstance, hPrevInstance, nCmdShow);
		}
		//Inner exception handler
		__except (PlatformCrash::Report(GetExceptionInformation()), EXCEPTION_CONTINUE_SEARCH)
		{
			;
		}
	}

	return Error;
}


#if SHOW_DEBUG_CONSOLE 
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
	gIsFirstInstance = CreateNemedMutex();

	//Cache the hInstance
	gMainInstanceHandle = hInstance;

	//If report crash even if a debugger is attached
	gAlwaysReportCrash = false;


	int32 Error = 0;
	if (::IsDebuggerPresent() && !gAlwaysReportCrash)
	{
		Error = MAIN_ENTRANCE_CALL(hInstance, hPrevInstance, nCmdShow);
	}
	else
	{
		//To catch any exceptions on threads that are not throw by the application code
		//::SetUnhandledExceptionFilter(UnhandledException);

		//Structured exception handling
		__try
		{
			Error = GuardedMainEntrance(hInstance, hPrevInstance, nCmdShow);
		}
		__except (EXCEPTION_EXECUTE_HANDLER)
		{
			ReleaseNamedMutex();
			Error = 1;
			gIsGetCriticalError = true;
			LOG(Fatal, WindowsMainEntrance, TEXTS("Program shutdown because some exceptions. See Log.log and Fatal.log"));

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
