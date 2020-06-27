
#include "HAL/Memory.h"
#include "HAL/AllocJemalloc.h"


//Third party lib
#define JEMALLOC_NO_DEMANGLE
#include <jemalloc/jemalloc.h>

#ifndef JEMALLOC_VERSION
#error JEMALLOC_VERSION not defined
#endif
/////////////////



void* AllocJemalloc::Malloc(Size_T Size, uint32 Align)
{
	TotalMallocCount();

	void* NewMemory = nullptr;

#if PLATFORM_MAC
#error No platform implement code here
#elif PLATFORM_WINDOWS || PLATFORM_LINUX
	if (Align == ALIGNMENT_DEFAULT)
	{
		NewMemory = je_malloc(Size);
	}
	else if (Align == ALIGNMENT_MIXED)
	{
		NewMemory = je_aligned_alloc((Size >= 16 ? (uint32)16 : (uint32)8), Size);
	}
	else
	{
		NewMemory = je_aligned_alloc(Align, Size);
	}
#endif

	if (!NewMemory && Size)
	{
		OutOfMemory(Size, Align);
	}

	return NewMemory;
}


void* AllocJemalloc::Realloc(void* Dest, Size_T Size, uint32 Align)
{
	TotalReallocCount();

	void* NewMemory = nullptr;

#if PLATFORM_MAC
#error No platform implement code here
#elif PLATFORM_WINDOWS || PLATFORM_LINUX
	if (Align == ALIGNMENT_DEFAULT)
	{
		NewMemory = je_realloc(Dest, Size);
	}
	else
	{
		Size_T OldSize = 0;
		NewMemory = Malloc(Size, Align);
		
		if (Dest)
		{
			OldSize = je_malloc_usable_size(Dest);
			Memory::Copy(NewMemory, Dest, OldSize);
			Free(Dest);
		}
	}
#endif

	if (!NewMemory && Size)
	{
		OutOfMemory(Size, Align);
	}

	return NewMemory;
}


void AllocJemalloc::Free(void* Dest)
{
	if (Dest == nullptr) return;

	TotalFreeCount();
	je_free(Dest);
}
