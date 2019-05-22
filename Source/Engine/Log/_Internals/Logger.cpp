#include "Log/Logger.h"
#include "Global/Utilities/Misc.h"
#include "HAL/Memory/Memory.h"



void Logger::Logf(LogVerbosity Verbosity, const TChar* Format, ...)
{
	SRINTF_VARARGS(Serialize(Verbosity, BufferPtr));
}


