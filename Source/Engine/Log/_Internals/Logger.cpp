#include "Log/Logger.h"
#include "Global/Utilities/Misc.h"
#include "HAL/Memory.h"



void Logger::Logf(LogVerbosity Verbosity, const TChar* Format, ...)
{
	const int32 BufferInitSize = 512;
	TChar  PrintBuffer[BufferInitSize];
	TChar* BufferPtr = PrintBuffer;
	int32  BufferSize = BufferInitSize;
	int32  WroteCount = -1;

	GET_FORMAT_VARARGS(BufferPtr, BufferSize, BufferSize - 1, Format, Format, WroteCount);

	if (WroteCount == -1)
	{
		BufferPtr = nullptr;
		while (WroteCount == -1)
		{
			BufferSize *= 2;
			BufferPtr = (TChar*)Memory::Realloc(BufferPtr, BufferSize * sizeof(TChar));
			GET_FORMAT_VARARGS(BufferPtr, BufferSize, BufferSize - 1, Format, Format, WroteCount);
		}
	}

	BufferPtr[WroteCount] = 0;

	Serialize(Verbosity, BufferPtr);

	if (BufferSize != BufferInitSize)
		Memory::Free(BufferPtr);

}


