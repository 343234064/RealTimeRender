#include "Log/LogDeviceFatal.h"
#include "Global/GlobalType.h"
#include "Global/EngineVariables.h"
#include "Global/Utilities/CharConversion.h"
#include "Global/Utilities/DynamicArray.h"


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
		PlatformChars::Strncpy(FatalHist, Data, ARRAY_SIZE(FatalHist));
		PlatformChars::Strcat(FatalHist, ARRAY_SIZE(FatalHist), TEXTS("\r\n"));

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
	if (!Writer.is_open() && !InitFailed)
	{
		Writer.open("Fatal.log", std::ios_base::in);

		if (!Writer.is_open())
		{
			InitFailed = true;
		}
	}

	if (Writer.is_open())
	{
		//Convert to utf8, otherwise it will output garbled character while using unicode
		const int32 DataLength = (int32)PlatformChars::Strlen(FatalHist);
		const int32 ConvertedLength = TCharToUTF8::Length(FatalHist, DataLength);
		Array<ANSICHAR> ConvertedUTF8Text;

		ConvertedUTF8Text.AddUninitialize(ConvertedLength);
		TCharToUTF8::Convert(ConvertedUTF8Text.Begin(), ConvertedLength, FatalHist, DataLength);

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
	if (Writer.is_open())
	{
		Writer.flush();
		Writer.close();
	}

	InitFailed = false;
}