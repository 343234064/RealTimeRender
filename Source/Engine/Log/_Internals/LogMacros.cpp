#include "Log/LogMacros.h"
#include "Log/LoggerFile.h"
#include "Global/Utilities/Misc.h"


void LogInternal(LogVerbosity Verbosity, const TChar* Format, ...)
{
	
	switch (Verbosity)
	{
	case LogVerbosity::Error:
	case LogVerbosity::Warning:
	case LogVerbosity::ToAll:
	{
		
		if (gLogFile)
		{
			CALL_FUNC_VARARGS(gLogFile->Logf(Verbosity, Format, Args), Format);
		}

		/*
		if (gLogScreen)
		{

		}*/

		break;
	}
	case LogVerbosity::ToFile:
	{
		
		if (gLogFile)
		{
			CALL_FUNC_VARARGS(gLogFile->Logf(Verbosity, Format, Args), Format);
		}
		break;
	}
	case LogVerbosity::ToScreen:
	{
		/*
		if (gLogScreen)
		{

		}*/
		break;
	}
	default:
		break;
	}


}