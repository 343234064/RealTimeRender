#include "Log/LogDevice.h"
#include "Global/Utilities/Misc.h"
#include "HAL/Memory.h"



void LogDevice::Logf(const TChar* Format, ...)
{
	SRINTF_VARARGS(Serialize(BufferPtr));
}


