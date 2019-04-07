#include "Global/EngineVariables.h"
#include "HAL/Memory/AllocatorInterface.h"
#include "HAL/Time.h"
#include "Launch/CoreEngine.h"



/************************************
Globals
************************************/
/*Is the application requests exit*/
bool gIsAppRequestExit = false;

/*Is the application the first instance*/
bool gIsFirstInstance = false;

/*If the application should generate crash report even having a debugger attached*/
bool gReportCrashEvenDebugger = false;

/*If enable the exception handler within native C++*/
bool gEnableInnerException = false;

/*If we get error*/
bool gIsGetError = false;

/*If we init the gMainThreadId*/
bool gIsMainThreadIdCached = false;

/*Main thread id*/
uint32 gMainThreadId = 0;

/*The time started*/
double gStartTime = PlatformTime::InitTime();


/*Global engine instance*/
CoreEngine gCoreEngine;

/*Global allocator*/
MallocInterface* gMallocator = nullptr;