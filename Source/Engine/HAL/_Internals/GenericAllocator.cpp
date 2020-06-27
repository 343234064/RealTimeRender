
#include "HAL/Platforms/GenericAllocator.h"
#include "HAL/Memory.h"


AtomicCounter<int32> MallocInterface::TotalMallocCalls(0);
AtomicCounter<int32> MallocInterface::TotalReallocCalls(0);
AtomicCounter<int32> MallocInterface::TotalFreeCalls(0);



void* MallocInterface::operator new(size_t Size)
{

	return Memory::SystemMalloc(Size);
}

void MallocInterface::operator delete(void* Dest)
{

	Memory::SystemFree(Dest);
}

void* MallocInterface::operator new[](size_t Size)
{

	return Memory::SystemMalloc(Size);
}

void MallocInterface::operator delete[](void* Dest)
{

	Memory::SystemFree(Dest);
}


void MallocInterface::OutOfMemory(Size_T Size, uint32 Align)
{
	PlatformMemory::OutOfMemoryHandling(Size, Align);
	OutputAllocStatistics();
}

void MallocInterface::UpdateAllocStatistics()
{
	//temp
}

void MallocInterface::OutputAllocStatistics()
{
	//output message
}


