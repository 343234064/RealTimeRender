
#include "HAL/AllocDefault.h"

#if PLATFORM_WINDOWS
#include<heapapi.h>
#endif

AllocDefault::AllocDefault()
{
//Enable Low-fragmentation Heap
#if PLATFORM_WINDOWS
	HANDLE HeapHandle = GetProcessHeap();
	ULONG HEAP_LFH = 2;
	HeapSetInformation(HeapHandle, HeapCompatibilityInformation, &HEAP_LFH, sizeof(HEAP_LFH));
#endif
}

void* AllocDefault::Malloc(Size_T Size, uint32 Align)
{
	TotalMallocCount();

	void* NewMemory = nullptr;

#if PLATFORM_WINDOWS 
	if (Align == ALIGNMENT_DEFAULT)
	{
		NewMemory = _aligned_malloc(Size, 8);
	}
	else if (Align == ALIGNMENT_MIXED)
	{
		NewMemory = _aligned_malloc(Size, (Size >= 16 ? (uint32)16 : (uint32)8));
	}
	else
	{
		NewMemory = _aligned_malloc(Size, Align);
	}
#elif PLATFORM_MAC
	NewMemory = _aligned_malloc(Size, 16);
#else PLATFORM_LINUX
    #error No platform implement code here
#endif

	if (!NewMemory && Size)
	{
		OutOfMemory(Size, Align);
	}

	return NewMemory;
}


void* AllocDefault::Realloc(void* Dest, Size_T Size, uint32 Align)
{
	TotalReallocCount();

	void* NewMemory = nullptr;

#if PLATFORM_WINDOWS 
	if (Align == ALIGNMENT_DEFAULT)
	{
		NewMemory = _aligned_realloc(Dest, Size, 8);
	}
	else if (Align == ALIGNMENT_MIXED)
	{
		NewMemory = _aligned_realloc(Dest, Size, (Size >= 16 ? (uint32)16 : (uint32)8));
	}
	else
	{
		NewMemory = _aligned_realloc(Dest, Size, Align);
	}
#elif PLATFORM_MAC
		NewMemory = _aligned_realloc(Dest, Size, 16);
#else PLATFORM_LINUX
        #error No platform implement code here
#endif
	

	if (!NewMemory && Size)
	{
		OutOfMemory(Size, Align);
	}

	return NewMemory;
}


void AllocDefault::Free(void* Dest)
{
	if (Dest == nullptr) return;

	TotalFreeCount();

#if PLATFORM_WINDOWS || PLATFORM_MAC
	_aligned_free(Dest);
#else PLATFORM_LINUX
    #error No platform implement code here
#endif
}


bool AllocDefault::IsThreadSafe()
{
#if PLATFORM_WINDOWS 
	return false;
#elif PLATFORM_MAC
	return true;
#elif PLATFORM_LINUX
	return true;
#endif
}