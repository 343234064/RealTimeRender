
#include "Global/Utilities/Assertion.h"
#include "HAL/Platforms/Windows/WindowsMemory.h"

#include <Psapi.h>

#if PLATFORM_ALLOC_SUPPORT_JEM
#include "HAL/AllocJemalloc.h"
#endif

#if PLATFORM_ALLOC_SUPPORT_TBB
#include "HAL/AllocIntelTBB.h"
#endif

#include "HAL/AllocDefault.h"





void WindowsMemory::Init()
{
	BackupOOMMemorySize = WINDOWS_BACKUP_OOM_MEMORY_SIZE;
	PlatformMemoryInterface::Init();

	WindowsMemory::GetMemoryDesc();
}


void WindowsMemory::UnInit()
{
	PlatformMemoryInterface::UnInit();
	//log
}


MallocInterface* WindowsMemory::Allocator()
{
	AllocatorTypes ToUse;

	if (PLATFORM_ALLOC_USE_TBB)
	{
		ToUse = AllocatorTypes::TBB;
	}
	else if (PLATFORM_ALLOC_USE_JEM)
	{
		ToUse = AllocatorTypes::Jemalloc;
	}
	else
	{
		ToUse = AllocatorTypes::Default;
	}

	switch (ToUse)
	{
#if PLATFORM_ALLOC_SUPPORT_TBB
	case AllocatorTypes::TBB:
		return new AllocTBB();//Use system new operator
#endif
#if PLATFORM_ALLOC_SUPPORT_JEM
	case AllocatorTypes::Jemalloc:
		return new AllocJemalloc();//Use system new operator
#endif
	default:
		return new AllocDefault();//Use system new operator
	}

}

void* WindowsMemory::AllocPageFromOS(Size_T Size)
{
	//track
	return VirtualAlloc(NULL, Size, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
}

bool WindowsMemory::FreePageFromOS(void* Dest, Size_T Size)
{
	//track
	bool Result = VirtualFree(Dest, 0, MEM_RELEASE);
	CHECK(Result);
	return Result;
}


PlatformMemoryStates WindowsMemory::GetMemoryState()
{
	MEMORYSTATUSEX MSE;
	WindowsMemory::Memzero(&MSE, sizeof(MEMORYSTATUSEX));
	MSE.dwLength = sizeof(MEMORYSTATUSEX);
	GlobalMemoryStatusEx(&MSE);

	PROCESS_MEMORY_COUNTERS PMC;
	WindowsMemory::Memzero(&PMC, sizeof(PROCESS_MEMORY_COUNTERS));
	GetProcessMemoryInfo(GetCurrentProcess(), &PMC, sizeof(PROCESS_MEMORY_COUNTERS));

	PlatformMemoryStates State;

	State.PhysicalMemoryAvailable = MSE.ullAvailPhys;
	State.VirtualMemoryAvailable = MSE.ullAvailVirtual;

	State.PhysicalMemoryUsed = PMC.WorkingSetSize;
	State.VirtualMemoryUsed = PMC.PagefileUsage;

	State.PeakPhysicalMemoryUsed = PMC.PeakWorkingSetSize;
	State.PeakVirtualMemoryUsed = PMC.PeakPagefileUsage;

	return State;
}


void WindowsMemory::GetMemoryDesc()
{
	if (MemoryDesc.TotalPhysicalMemory == 0)
	{
		MEMORYSTATUSEX MSE;
		WindowsMemory::Memzero(&MSE, sizeof(MEMORYSTATUSEX));
		MSE.dwLength = sizeof(MEMORYSTATUSEX);
		GlobalMemoryStatusEx(&MSE);
		
		SYSTEM_INFO STI;
		WindowsMemory::Memzero(&STI, sizeof(SYSTEM_INFO));
		GetSystemInfo(&STI);

		MemoryDesc.TotalPhysicalMemory = MSE.ullTotalPhys;
		MemoryDesc.TotalVirtualMemory = MSE.ullTotalVirtual;
		MemoryDesc.PageSize = STI.dwPageSize;
	}

}