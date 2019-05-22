#include "Log/LogMacros.h"
#include "Log/LoggerFile.h"
#include "Global/Utilities/Misc.h"





void LogMisc::LogInternal(LogVerbosity Verbosity, const TChar* Format, ...)
{
	
	switch (Verbosity)
	{
	case LogVerbosity::Error:
	case LogVerbosity::Warning:
	case LogVerbosity::ToAll:
	{
		
		if (gLogFile)
		{
			//Do not use gLogFile->Logf here, otherwise it will output garbled character
			SRINTF_VARARGS(gLogFile->Log(Verbosity, BufferPtr));
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
			SRINTF_VARARGS(gLogFile->Log(Verbosity, BufferPtr));
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