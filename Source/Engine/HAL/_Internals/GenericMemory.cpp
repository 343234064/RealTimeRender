
#include "HAL/Platforms/GenericMemory.h"
#include "HAL/AllocThreadSafeDecorator.h"
#include "HAL/Memory.h"


/*
   Init static variables
*/
bool PlatformMemoryInterface::IsOutOfMemory = false;
uint64 PlatformMemoryInterface::CauseOOMAllocationSize = 0;
uint32 PlatformMemoryInterface::CauseOOMAllocationAlign = ALIGNMENT_DEFAULT;
void* PlatformMemoryInterface::BackupOOMMemory = nullptr;
uint32 PlatformMemoryInterface::BackupOOMMemorySize = 0;
PlatformMemoryDesc PlatformMemoryInterface::MemoryDesc = PlatformMemoryDesc();


void PlatformMemoryInterface::Init()
{
	//setup state
	
	
	//create the global memory allocator
	if (nullptr == gMallocator)
	{
		CreateGlobalMalloc();
		//check 
	}

	//setup memory pool
	if (BackupOOMMemorySize > 0)
	{
		//tracker
		BackupOOMMemory = PlatformMemory::AllocPageFromOS(BackupOOMMemorySize);
	}

	
}


void PlatformMemoryInterface::UnInit()
{
	if (BackupOOMMemory)
	{
		PlatformMemory::FreePageFromOS(BackupOOMMemory, BackupOOMMemorySize);
		//log
		//tracker
		BackupOOMMemory = nullptr;
	}

	if (gMallocator)
	{
		delete gMallocator;
		gMallocator = nullptr;
	}
}


void PlatformMemoryInterface::OutOfMemoryHandling(uint64 Size, uint32 Alignment)
{
	CauseOOMAllocationSize = Size;
	CauseOOMAllocationAlign = Alignment;

	if (IsOutOfMemory) return;
	IsOutOfMemory = true;


	if (BackupOOMMemory)
	{
		PlatformMemory::FreePageFromOS(BackupOOMMemory, BackupOOMMemorySize);
		//log
		//tracker
		BackupOOMMemory = nullptr;
	}

	//handling..
	PlatformMemoryStates MemoryState = PlatformMemory::GetMemoryState();
	
	//log 
	OutputMemoryStatistics();

	//Delegate broadcast
	//log
}


void PlatformMemoryInterface::OutputMemoryStatistics()
{
	//output message
}

void PlatformMemoryInterface::UpdateMemoryStatistics()
{
	//temp
}



void CreateGlobalMalloc()
{
	//base allocator
	gMallocator = PlatformMemory::Allocator();

	//decorators....
	/*
	if (MALLOC_USE_PROFILER)
	{
		//decorate class
	}
	*/

	if (!(gMallocator->IsThreadSafe()))
	{
		gMallocator = new AllocThreadSafeDecorator(gMallocator);
	}

	/*
    if (MALLOC_USE_LEAKDETECTION)
    {
	    //decorate class
    }
    */
	
	return;
}