#include "Log/LogDeviceFatal.h"
#include "Global/GlobalType.h"
#include "Global/EngineVariables.h"
#include "Global/Utilities/CharConversion.h"
#include "Global/Utilities/DynamicArray.h"
#include "Log/LogMacros.h"

/*Total Fatal messages cache*/
static TChar FatalHist[8192] = TEXTS("");
static PlatformCriticalSection FatalHistCriticalSection;
static bool FatalHistHasFlush = false;


LogDeviceFatal::LogDeviceFatal():
	InitFailed(false)
{}

void LogDeviceFatal::Serialize(const TChar* Data)
{
	static bool ErrorEntry = false;
	if (!gIsGetCriticalError || ErrorEntry)
	{
		LockGuard<PlatformCriticalSection> Lock(FatalHistCriticalSection);

		//Copy to fatal message buffer, to wait for output to file
		PlatformChars::Strncpy(FatalHist + PlatformChars::Strlen(FatalHist), Data, ARRAY_SIZE(FatalHist));
		//PlatformChars::Strcat(FatalHist, ARRAY_SIZE(FatalHist), TEXTS("\r"));

		FatalHistHasFlush = false;
	}
	else
	{
		ErrorEntry = true;
		Serialize(Data);
		ErrorEntry = false;
	}

}


void LogDeviceFatal::WriteFatalHistToFile()
{
#if ENABLE_LOG
	if (!Writer.is_open() && !InitFailed)
	{
		Writer.open("Fatal.log", std::ios_base::app);
		
		if (!Writer.is_open())
		{
			InitFailed = true;
			DEBUG(LogDeviceFatal, TEXTS("Open Fatal.log file failed!"));
		}
	}
#endif

	if (Writer.is_open())
	{
		//Convert to utf8, otherwise it will output garbled character while using unicode
		Array<ANSICHAR> ConvertedUTF8Text;
		TCharToUTF8::Convert(ConvertedUTF8Text, FatalHist);

		Writer.write(ConvertedUTF8Text.Begin(), ConvertedUTF8Text.CurrentNum() * sizeof(ANSICHAR));
	}

	FatalHistHasFlush = true;
}


void LogDeviceFatal::Flush()
{
	LockGuard<PlatformCriticalSection> Lock(FatalHistCriticalSection);

	if (ARRAY_SIZE(FatalHist) > 0 && !FatalHistHasFlush)
	{
		FatalHist[ARRAY_SIZE(FatalHist) - 1] = 0;
		WriteFatalHistToFile();
	}

	if (Writer.is_open())
		Writer.flush();
}



void LogDeviceFatal::Shutdown()
{
	if(!FatalHistHasFlush)
		Flush();

	if (Writer.is_open())
	{
		Writer.close();
	}

	InitFailed = false;
}