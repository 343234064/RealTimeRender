#include "Log/LogMacros.h"
#include "Log/LogManager.h"
#include "Log/LogDeviceFile.h"
#include "Log/LogDeviceFatal.h"
#include "Global/Utilities/Misc.h"
#include "HAL/Dialog/Dialog.h"
#include "HAL/Time.h"

//For fatal messages
static TChar StaticFatalMsgBuffer[2048]; //2kb
static PlatformCriticalSection FatalMsgBufferCriticalSection;

LogTime MessageLog::TimeType = LogTime::Local;


void MessageLog::Output(LogType Type, const TChar* ClassName, int32 Line, double Time, const TChar* Format, ...)
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

	SRINTF_VARARGS(gLogger.Redirector(Type, ClassName, TimeString, Line, Time, BufferPtr));

}

void MessageLog::Shutdown()
{
	gLogger.Shutdown();
}




void FatalLog::Output(const TChar* ClassName, int32 Line, const TChar* Format, ...)
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
	String Description = String::Sprintf(TEXTS("[Fatal Error: %s, %d]\n"), ClassName, Line);
	PlatformDialog::Open(DialogType::Ok, *Description, StaticFatalMsgBuffer);

	//Output to local
	Description += StaticFatalMsgBuffer;
	Platform::LocalPrintW(*Description);
	gFatalLogger.Log(*Description);

}

void FatalLog::Shutdown()
{
	gFatalLogger.Shutdown();
}