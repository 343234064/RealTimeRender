#include "Launch/CoreEngine.h"
#include "Global/EngineVariables.h"
#include "Editor/Editor.h"
#include "HAL/Memory/Memory.h"
#include "HAL/Thread/Thread.h"
#include "HAL/Time.h"
#include "Log/LoggerFile.h"
#include "Log/LogMacros.h"


int32 CoreEngine::PreInit()
{
	//Init the memory
	PlatformMemory::Init();


	gMainThreadId = Platform::GetCurrentThreadId();
	gIsMainThreadIdCached = true;
	Platform::SetCurrentThreadAffinityMask(PlatformAffinity::GetMainThreadMask());

	//Init log
	gLogFile = new LoggerFile(TEXTS("Log.txt"), true);
	CHECK(gLogFile != nullptr);

	
	//Set random seed

	//Cache executable's dir
	//Check project path

	//load core module
	


	//load ini file
	//Init file manager

	//create thread pool

	//profiler init

	//init RHI
	//load and compile shader
	//start render thread
	
	LOG(LogVerbosity::ToFile, TEXTS("%s"), TEXTS("回答说科技的户口卡山东航空打算"));
	LOG(LogVerbosity::ToFile, TEXTS("%s"), TEXTS("说山东航空打算"));
	LOG(LogVerbosity::ToFile, TEXTS("%s"), TEXTS("空打算"));
	return 0;
}


int32 CoreEngine::Init()
{
	InitTime();

	//load module
	//e.g phys..

	//Creat Editor
	gEditor = Editor::CreateEditor();
	
	if (!gEditor->Init())
	{
		//log
		return 1;
	}
	gEditor->Show();
	

	//broadcast OnInitComplete
	return 0;
}


void CoreEngine::Tick()
{
	//Render thread tick

	//begin frame render thread

	//tick performance and statistics monitoing 

	//tick(world, game objects, etc.)

	//tick RHI
	gEditor->Tick();

	gFrameCounter++;
}


void CoreEngine::Exit()
{
	//Flush all logs and Loading things
	if (gLogFile)
	{
		delete gLogFile;
		gLogFile = nullptr;
	}


   //save engine config and ini file
   //broadcast OnExit()

   if (gEditor)
   {
	  gEditor->Exit();
	  delete gEditor;
	  gEditor = nullptr;
   }

   //destroy any pool
   //destroy any global
   //terminate phys
   //stop render thread
   //RHI exit

   PlatformMemory::UnInit();

}



void CoreEngine::InitTime()
{
	CurrentTimeSeconds = PlatformTime::Time_Seconds();
	LastFrameCycle = PlatformTime::Time_Cycles();

	//get config
	FPS = 30;//default
	DeltaTime = 1 / (double)FPS;


}