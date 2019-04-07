/************************************
Universal Variables 


*************************************/
#pragma once

#include "Global/GlobalType.h"


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


/*Main thread id*/
extern uint32 gMainThreadId;

/*The time process started*/
extern double gStartTime;

