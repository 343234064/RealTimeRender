/************************************
Universal Variables 


*************************************/
#pragma once

#include "Global/GlobalType.h"
#include "HAL/Chars.h"

//==============================
//Version
//==============================
const TChar ApplicationIdentity[] =  TEXTS("_RTR_002");
const TChar ApplicationName[] =  TEXTS("RT-Renderer");
const TChar ApplicationVersionName[] = TEXTS("-ver-0.02");

#define APPLICATION_IDENTITY ApplicationIdentity
#define APPLICATION_NAME ApplicationName

#define APPLICATION_VERSION_NAME ApplicationVersionName
#define APPLICATION_VERSION_NUM 002




/************************************
Globals
************************************/
/*Is the application requests exit*/
extern bool gIsAppRequestExit;

/*Is the application the first instance*/
extern bool gIsFirstInstance;

/*If we get critical error*/
extern bool gIsGetCriticalError;

/*If we init the gMainThreadId*/
extern bool gIsMainThreadIdCached;

/*if messages are being pumped outside of main loop*/
extern bool gIsPumpingMessagesOutOfMainLoop;

/*If always report crash using platform crash handler even if the debugger is attached*/
extern bool gAlwaysReportCrash;


/*Main thread id*/
extern uint32 gMainThreadId;

/*The time process started*/
extern double gStartTime;

/*The Total frame count*/
//uint64 is big enough for counting(e.g. 1000fps will has a maximum to almost 6k years)
extern uint64 gFrameCounter; 

/** For building call stack text dump in guard/unguard mechanism. */
extern TChar gErrorHist[16384];




