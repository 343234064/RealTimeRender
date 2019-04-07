#pragma once
#include "Global/GlobalConfigs.h"
#include "Global/GlobalType.h"
#include "Global/EngineVariables.h"

#include "CoreEngine.h"

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
		int32 Error = gCoreEngine.PreInit();
		return Error;
	}

	static int32 Init()
	{
		int32 Error = gCoreEngine.Init();
		return Error;
	}

	static void Tick() 
	{
		gCoreEngine.Tick();
	}

	static void Exit()
	{
		gIsAppRequestExit = true;
		gCoreEngine.Exit();
	}

	static MAIN_ENTRANCE_DECL
	{
	    struct CleanUpGuard
	    {
		    ~CleanUpGuard()
	    	{
		 	   Application::Exit();
		    }
	    };
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






























