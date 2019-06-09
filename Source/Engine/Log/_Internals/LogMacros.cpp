#include "Log/LogMacros.h"
#include "Log/LogDeviceFile.h"
#include "Log/LogDeviceFatal.h"
#include "Global/Utilities/Misc.h"
#include "HAL/Dialog/Dialog.h"

//For fatal messages
static TChar StaticFatalMsgBuffer[2048]; //4kb
static PlatformCriticalSection FatalMsgBufferCriticalSection;



void FatalLog::OutputToLocal(const TChar* Format, ...)
{
	int32 WriteCount = 0;
	TChar Description[2048];
	GET_FORMAT_VARARGS(Description, ARRAY_SIZE(Description), ARRAY_SIZE(Description) - 1, Format, Format, WriteCount);
	Description[WriteCount] = '\0';

	Platform::LocalPrintW(Description);

	gFatalLogger.Log(Description);
}


void FatalLog::Output(const TChar* ClassName, const TChar* Format, ...)
{
	{
		LockGuard<PlatformCriticalSection> Lock(FatalMsgBufferCriticalSection);

		GET_FORMAT_VARARGS_NORESULT(StaticFatalMsgBuffer, ARRAY_SIZE(StaticFatalMsgBuffer), ARRAY_SIZE(StaticFatalMsgBuffer) - 1, Format, Format);
		StaticFatalMsgBuffer[ARRAY_SIZE(StaticFatalMsgBuffer) - 1] = '\0';
	};

	//Show message box 
	PlatformDialog::Open(DialogType::Ok, TEXTS("Fatal Error!"), StaticFatalMsgBuffer);

	
	//Output to local
	OutputToLocal(TEXTS("[Fatal Error][%s]: %s"), ClassName, StaticFatalMsgBuffer);

}