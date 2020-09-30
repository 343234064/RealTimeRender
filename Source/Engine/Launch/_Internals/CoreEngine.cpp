#include "Launch/CoreEngine.h"
#include "Global/EngineVariables.h"
#include "Editor/Editor.h"
#include "HAL/Memory.h"
#include "HAL/Thread.h"
#include "HAL/Time.h"
#include "Log/LogMacros.h"
#include "Log/LogManager.h"
#include "Log/LogDeviceFatal.h"


int32 CoreEngine::PreInit()
{
	//Init the memory statistics
	PlatformMemory::OutputMemoryStatistics();

	gMainThreadId = Platform::GetCurrentThreadId();
	gIsMainThreadIdCached = true;
	Platform::SetCurrentThreadAffinityMask(PlatformAffinity::GetMainThreadMask());



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
	

	return 0;
}
#include <assert.h>

int32 CoreEngine::Init()
{
	InitTime();

	//load module
	//e.g phys, animation, python...

	//Creat Editor
	gEditor = Editor::CreateEditor();
	
	if (!gEditor->Init())
	{
		//log
		return 1;
	}
	gEditor->Show();
	

	//broadcast OnInitComplete
	//CHECK(false);
	LOG(Info, CoreEngine, TEXTS("This is a test:%s"), TEXTS("Info，这是一个测试"));
	LOG(Warn, CoreEngine, TEXTS("This is a test:%s"), TEXTS("Warn，这是一个测试"));
	LOG(Fatal, CoreEngine, TEXTS("This is a test:%s"), TEXTS("Fatal2，调偶家将军澳2"));
	DEBUG(CoreEngine, TEXTS("This is a test:%s"), TEXTS("Debug，这是一个测试"));
	LOG(Error, CoreEngine, TEXTS("ZZZZZZZZZZZZ:%s %d"), TEXTS("ZXXXXXXX大时代"), 678);

	LOG(Fatal, CoreEngine, TEXTS("This is a test:%s"), TEXTS("Fatal，这是一个测试"));
	LOG(Fatal, CoreEngine, TEXTS("This is a test:%s"), TEXTS("Fatal2，大寺基调偶家将军澳2"));
	



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
   

   LOG_SHUTDOWN;
   FATALLOG_SHUTDOWN;

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