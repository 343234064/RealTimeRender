#include "Log/LogMacros.h"
#include "Log/LogDeviceFile.h"
#include "Global/Utilities/Misc.h"


//For fatal messages
static TChar StaticFatalMsgBuffer[4096]; //8kb
static PlatformCriticalSection FatalMsgBufferCriticalSection;



void FatalLog::OutputToLocal(const TChar* Format, ...)
{
	SRINTF_VARARGS(Platform::LocalPrintW(BufferPtr));
}


void FatalLog::Output(const TChar* ClassName, const TChar* Format, ...)
{

	TChar Description[2048];//4kb
	{
		LockGuard<PlatformCriticalSection> Lock(FatalMsgBufferCriticalSection);
		GET_FORMAT_VARARGS_NORESULT(StaticFatalMsgBuffer, ARRAY_SIZE(StaticFatalMsgBuffer), ARRAY_SIZE(StaticFatalMsgBuffer) - 1, Format, Format);
		PlatformChars::Strncpy(Description, StaticFatalMsgBuffer, ARRAY_SIZE(Description) - 1);
		Description[ARRAY_SIZE(Description) - 1] = '\0';
	}

	OutputToLocal(TEXTS("[Fatal Error][%s]: %s"), ClassName, Description);

	//message box

	//another error output device


}