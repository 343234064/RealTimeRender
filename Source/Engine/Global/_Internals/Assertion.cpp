#include "Global/Utilities/Assertion.h"
#include "Global/Utilities/String.h"
#include "Global/Utilities/CharConversion.h"
#include "HAL/Dialog.h"
#include <stdio.h>


PlatformCriticalSection Assertion::DebugCriticalSection;


void Assertion::OuputAssertionFailed(const ANSICHAR* Expression, const ANSICHAR* File, int32 Line, const ANSICHAR* Format, ...)
{
	if (!gIsGetCriticalError)
	{
		ANSICHAR DescriptionText[DESCRIPTION_LENGTH];
		ANSICHAR PrefixText[PREFIX_LENGTH];
		ANSICHAR FinalText[PREFIX_LENGTH + DESCRIPTION_LENGTH + 10];//Leave a little space

		int32 Result = 0;
		GET_FORMAT_VARARGS(DescriptionText, sizeof(ANSICHAR) * DESCRIPTION_LENGTH, sizeof(ANSICHAR) * (DESCRIPTION_LENGTH - 1), Format, Format, Result);
		PlatformChars::Sprintf(PrefixText, PREFIX_LENGTH, "Assertion Failed: %s \n", Expression);

		PlatformChars::Sprintf(FinalText, PREFIX_LENGTH + DESCRIPTION_LENGTH + 10, "%s [File:%s][Line:%i]\n%s\n", PrefixText, File, Line, DescriptionText);

		LockGuard<PlatformCriticalSection> DebugLock(DebugCriticalSection);

		Platform::LocalPrintA(FinalText);

		Array<TChar> ConvertedChars;
		UTF8ToTChar::Convert(ConvertedChars, FinalText);
		PlatformChars::Strncpy(gErrorHist, ConvertedChars.Begin(), ARRAY_SIZE(gErrorHist));
		PlatformChars::Strncat(gErrorHist, TEXTS("\r\n\r\n"), ARRAY_SIZE(gErrorHist));

		PlatformDialog::Open(DialogType::Ok, "Assertion Failed", FinalText);

		//dump log
	}
}