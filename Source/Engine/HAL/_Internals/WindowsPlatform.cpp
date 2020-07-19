#include "HAL/Platforms/Windows/WindowsPlatform.h"
#include "Global/EngineVariables.h"
#include "Global/GlobalType.h"
#include "HAL/Platforms/Windows/WindowsChars.h"

extern "C"
{
	HINSTANCE gMainInstanceHandle = NULL;
}

void Platform::RequestExit(bool ForceExit)
{
	//log
	gIsAppRequestExit = true;
	//broadcast

	if (ForceExit)
	{
		//log flush
		TerminateProcess(GetCurrentProcess(), gIsGetCriticalError ? 1 : 0);
	}
	else
	{
		PostQuitMessage(0);
	}
}

int32 Platform::ReportCrash(LPEXCEPTION_POINTERS ExceptionInfo)
{
	//handle crash

	return EXCEPTION_EXECUTE_HANDLER;
}

const PlatformTypes::WIDECHAR* Platform::GetSystemErrorMessage(PlatformTypes::WIDECHAR* OutBuffer, PlatformTypes::int32 BufferCount, PlatformTypes::int32 Error)
{
	if (OutBuffer == nullptr || BufferCount == 0)
	{
		return OutBuffer;
	}

	*OutBuffer = TEXTS('\0');
	if (Error == 0)
	{
		Error = ::GetLastError();
	}
	::FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM, NULL, Error, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), OutBuffer, BufferCount, NULL);

	return OutBuffer;
}