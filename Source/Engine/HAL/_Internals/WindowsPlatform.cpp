#include "HAL/Platforms/Windows/WindowsPlatform.h"
#include "Global/EngineVariables.h"
#include "Global/GlobalType.h"


void Platform::RequestExit(bool ForceExit)
{
	//log
	gIsAppRequestExit = true;
	//broadcast

	if (ForceExit)
	{
		//log flush
		TerminateProcess(GetCurrentProcess(), gIsGetError ? 1 : 0);
	}
	else
	{
		PostQuitMessage(0);
	}
}

int32 Platform::ReportCrash(LPEXCEPTION_POINTERS ExceptionInfo)
{
	//handle crash

	return EXCEPTION_EXECUTE_HANDLER;
}
