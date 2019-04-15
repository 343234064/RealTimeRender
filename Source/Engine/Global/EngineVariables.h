/************************************
Universal Variables 


*************************************/
#pragma once

#include "Global/GlobalType.h"
#include "HAL/Chars.h"



//==============================
//Version
//==============================
const TChar ApplicationIdentity[] =    TEXTS("_RTR_001");
const TChar ApplicationName[] =        TEXTS("RT-Renderer");
const TChar ApplicationVersionName[] = TEXTS("-ver-0.01");

#define APPLICATION_IDENTITY ApplicationIdentity
#define APPLICATION_NAME ApplicationName

#define APPLICATION_VERSION_NAME ApplicationVersionName
#define APPLICATION_VERSION_NUM 001




/************************************
Globals
************************************/
/*Is the application requests exit*/
extern bool gIsAppRequestExit;

/*Is the application the first instance*/
extern bool gIsFirstInstance;

/*If the application should generate crash report even having a debugger attached*/
extern bool gReportCrashEvenDebugger;

/*If enable the inner exception handler within native C++*/
extern bool gEnableInnerException;

/*If we get error*/
extern bool gIsGetError;

/*If we init the gMainThreadId*/
extern bool gIsMainThreadIdCached;

/*if messages are being pumped outside of main loop*/
extern bool gIsPumpingMessagesOutOfMainLoop;


/*Main thread id*/
extern uint32 gMainThreadId;

/*The time process started*/
extern double gStartTime;

