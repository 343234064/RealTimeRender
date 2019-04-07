#include "Launch/CoreEngine.h"
#include "Global/EngineVariables.h"
#include "HAL/Memory.h"
#include "HAL/Thread.h"
#include "HAL/Time.h"

int32 CoreEngine::PreInit()
{
	//Init the memory
	PlatformMemory::Init();


	gMainThreadId = Platform::GetCurrentThreadId();
	gIsMainThreadIdCached = true;
	Platform::SetCurrentThreadAffinityMask(PlatformAffinity::GetMainThreadMask());

	//Set random seed

	//Cache executable's dir
	//Check project path

	//load core module
	
	//Init log
	//Init output device
	//load ini file
	//Init file manager

	//create thread pool

	//profiler init

	//init RHI
	//load and compile shader
	//start render thread
	

	return 0;
}


int32 CoreEngine::Init()
{
	InitTime();

	//load module
	//e.g phys..

	//creat window

	//broadcast OnInitComplete
	return 0;
}


void CoreEngine::Tick()
{

}


void CoreEngine::Exit()
{
   //Flush all Loading things

	//save config and ini file
	//broadcast OnExit()
   //close windows
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