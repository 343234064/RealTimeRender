#include "Log/LogMacros.h"
#include "Log/LogManager.h"
#include "Log/LogDeviceFile.h"
#include "Log/LogDeviceFatal.h"
#include "Global/Utilities/Misc.h"
#include "HAL/Dialog/Dialog.h"
#include "HAL/Time.h"

//For fatal messages
static TChar StaticFatalMsgBuffer[2048]; //4kb
static PlatformCriticalSection FatalMsgBufferCriticalSection;

LogTime MessageLog::TimeType = LogTime::Local;


void MessageLog::Output(LogType Type, const TChar* ClassName, double Time, const TChar* Format, ...)
{
	//Get the time as early as possible
	Time = (Time < 0.0) ? PlatformTime::Time_Seconds() - gStartTime : Time;

	const int32 TimeLength = 50;
	TChar TimeBuffer[TimeLength];
	TChar* TimeString = nullptr;

	switch (TimeType)
	{
	case LogTime::Local:
	{
		PlatformTime::SystemTimeToStr(TimeBuffer, TimeLength);
		TimeString = TimeBuffer;
		break;
	}
	case LogTime::UTC:
	{
		PlatformTime::UTCTimeToStr(TimeBuffer, TimeLength);
		TimeString = TimeBuffer;
		break;
	}
	default:
		break;
	}

	SRINTF_VARARGS(gLogger.Redirector(Type, ClassName, TimeString, Time, BufferPtr));

}

void MessageLog::Shutdown()
{
	gLogger.Shutdown();
}




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

		int32 WriteCount = 0;
		GET_FORMAT_VARARGS(StaticFatalMsgBuffer, ARRAY_SIZE(StaticFatalMsgBuffer), ARRAY_SIZE(StaticFatalMsgBuffer) - 1, Format, Format, WriteCount);
		if (WriteCount > 0)
		{
			if (WriteCount <= ARRAY_SIZE(StaticFatalMsgBuffer) - 2)
			{
				StaticFatalMsgBuffer[WriteCount] = '\n';
				StaticFatalMsgBuffer[WriteCount + 1] = '\0';
			}
			else
			{
				StaticFatalMsgBuffer[WriteCount - 1] = '\n';
				StaticFatalMsgBuffer[WriteCount] = '\0';
			}
		}
		else 
			StaticFatalMsgBuffer[ARRAY_SIZE(StaticFatalMsgBuffer) - 1] = '\0';
	};

	//Show message box 
	PlatformDialog::Open(DialogType::Ok, TEXTS("Fatal Error!"), StaticFatalMsgBuffer);

	//Output to local
	OutputToLocal(TEXTS("[Fatal Error][%s]: %s"), ClassName, StaticFatalMsgBuffer);

}

void FatalLog::Shutdown()
{
	gFatalLogger.Shutdown();
}