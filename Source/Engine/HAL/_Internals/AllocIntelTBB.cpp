
#include "HAL/Memory/AllocIntelTBB.h"

//Third party lib
#include <tbb/scalable_allocator.h>
/////////////////



void* AllocTBB::Malloc(Size_T Size, uint32 Align)
{
	TotalMallocCount();

	void* NewMemory = nullptr;

#if PLATFORM_MAC
	NewMemory = scalable_aligned_malloc(Size, 16);
#elif PLATFORM_WINDOWS || PLATFORM_LINUX
	if (Align == ALIGNMENT_DEFAULT)
	{
		NewMemory = scalable_malloc(Size);
	}
	else if (Align == ALIGNMENT_MIXED)
	{
		NewMemory = scalable_aligned_malloc(Size, (Size >= 16 ? (uint32)16 : (uint32)8));
	}
	else
	{
		NewMemory = scalable_aligned_malloc(Size, Align);
	}
#endif
	if (!NewMemory && Size)
	{
		OutOfMemory(Size, Align);
	}

	return NewMemory;
}


void* AllocTBB::Realloc(void* Dest, Size_T Size, uint32 Align)
{
	TotalReallocCount();

	void* NewMemory = nullptr;

#if PLATFORM_MAC
	NewMemory = scalable_aligned_realloc(Dest, Size, 16);
#elif PLATFORM_WINDOWS || PLATFORM_LINUX
	if (Align == ALIGNMENT_DEFAULT)
	{
		NewMemory = scalable_realloc(Dest, Size);
	}
	else if (Align == ALIGNMENT_MIXED)
	{
		NewMemory = scalable_aligned_realloc(Dest, Size, (Size >= 16 ? (uint32)16 : (uint32)8));
	}
	else
	{
		NewMemory = scalable_aligned_realloc(Dest, Size, Align);
	}
#endif

	if (!NewMemory && Size)
	{
		OutOfMemory(Size, Align);
	}

	return NewMemory;
}


void AllocTBB::Free(void* Dest)
{
	if (Dest == nullptr) return;

	TotalFreeCount();
	scalable_free(Dest);
}
