#pragma once
#include "Global/GlobalConfigs.h"
#include "Global/GlobalType.h"
#include "Global/EngineVariables.h"


#if PLATFORM_WINDOWS
#define MAIN_ENTRANCE_DECL int32 Main(HINSTANCE hInInstance, HINSTANCE hPrevInstance, int32 nCmdShow)
#define MAIN_ENTRANCE_CALL(InInstance, PrevInstance, CmdShow) Application::Main(InInstance, PrevInstance, nCmdShow)
#else
#define MAIN_ENTRANCE_DECL int32 Main()  
#define MAIN_ENTRANCE_CALL Application::Main()  
#endif


struct Application 
{
	static int32 PreInit()
	{
		return 0;
	}

	static int32 Init()
	{
		return 0;
	}

	static void  Tick() 
	{

	}

	static void  Exit()
	{

	}

	struct CleanUpGuard
	{
		~CleanUpGuard()
		{
			Application::Exit();
		}
	};

	static MAIN_ENTRANCE_DECL
	{
		CleanUpGuard CallWhenExit;

	    int32 Error = 0;

		Error = Application::PreInit();
		if (Error > 0 || gIsAppRequestExit)
		{
			return Error;
		}


		Error = Application::Init();
		if (Error || gIsAppRequestExit)
		{
			return Error;
		}


		//log

		while (!gIsAppRequestExit)
		{
			Application::Tick();
			break;
		}

		//log

		return Error;
	}

};






























