/************************************
Universal Variables 


*************************************/
#pragma once


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
extern bool gIsThrowError;