#include "Global/EngineVariables.h"
#include "HAL/Memory/AllocatorInterface.h"
#include "HAL/Time.h"
#include "Launch/CoreEngine.h"
#include "Editor/Editor.h"
#include "Log/LogManager.h"
#include "Log/LogDeviceFatal.h"


/************************************
Globals
************************************/
/*Is the application requests exit*/
bool gIsAppRequestExit = false;

/*Is the application the first instance*/
bool gIsFirstInstance = false;

/*If we get critical error*/
bool gIsGetCriticalError = false;

/*If we init the gMainThreadId*/
bool gIsMainThreadIdCached = false;

/*if messages are being pumped outside of main loop*/
bool gIsPumpingMessagesOutOfMainLoop = false;

/*Main thread id*/
uint32 gMainThreadId = 0;

/*The time started*/
double gStartTime = PlatformTime::InitTime();

/*The Total frame count*/
uint64 gFrameCounter = 0;





/********Global instance************/

/*Global engine instance*/
CoreEngine gCoreEngine;

/*Global allocator*/
MallocInterface* gMallocator = nullptr;

/*Global editor*/
Editor* gEditor = nullptr;

/*Global Logger*/
LogManager& gLogger = LogManager::Get();

/*Fatal messages output device*/
LogDeviceFatal gFatalLogger;