#include "Global/Utilities/Assertion.h"
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

		fprintf(stderr, FinalText);
		Platform::LocalPrintA(FinalText);

		PlatformDialog::Open(DialogType::Ok, "Assertion Failed", FinalText);
	}
}