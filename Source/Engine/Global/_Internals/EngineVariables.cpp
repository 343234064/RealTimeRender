#include "Global/EngineVariables.h"


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
bool gIsThrowError = false;