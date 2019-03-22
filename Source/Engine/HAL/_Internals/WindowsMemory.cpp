
#include "HAL/Platforms/Windows/WindowsMemory.h"

#include <Psapi.h>

#if PLATFORM_ALLOC_SUPPORT_JEM
#include "HAL/Memory/AllocJemalloc.h"
#endif

#if PLATFORM_ALLOC_SUPPORT_TBB
#include "HAL/Memory/AllocIntelTBB.h"
#endif

#include "HAL/Memory/AllocDefault.h"





void WindowsPlatformMemory::Init()
{
	BackupOOMMemorySize = WINDOWS_BACKUP_OOM_MEMORY_SIZE;
	PlatformMemoryInterface::Init();

	WindowsPlatformMemory::GetMemoryDesc();
	//log

	OutputMemoryStatistics();
}


void WindowsPlatformMemory::UnInit()
{
	PlatformMemoryInterface::UnInit();
	//log
}


MallocInterface* WindowsPlatformMemory::Allocator()
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

void* WindowsPlatformMemory::AllocPageFromOS(Size_T Size)
{
	//track
	return VirtualAlloc(NULL, Size, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
}

void WindowsPlatformMemory::FreePageFromOS(void* Dest, Size_T Size)
{
	//track
	//check  VirtualFree(Dest, 0, MEM_RELEASE)!=0
	VirtualFree(Dest, 0, MEM_RELEASE);
}


void WindowsPlatformMemory::GetMemoryState(PlatformMemoryStates& State)
{
	MEMORYSTATUSEX MSE;
	WindowsPlatformMemory::Memzero(&MSE, sizeof(MEMORYSTATUSEX));
	MSE.dwLength = sizeof(MEMORYSTATUSEX);
	GlobalMemoryStatusEx(&MSE);

	PROCESS_MEMORY_COUNTERS PMC;
	WindowsPlatformMemory::Memzero(&PMC, sizeof(PROCESS_MEMORY_COUNTERS));
	GetProcessMemoryInfo(GetCurrentProcess(), &PMC, sizeof(PROCESS_MEMORY_COUNTERS));

	State.PhysicalMemoryAvailable = MSE.ullAvailPhys;
	State.VirtualMemoryAvailable = MSE.ullAvailVirtual;

	State.PhysicalMemoryUsed = PMC.WorkingSetSize;
	State.VirtualMemoryUsed = PMC.PagefileUsage;

	State.PeakPhysicalMemoryUsed = PMC.PeakWorkingSetSize;
	State.PeakVirtualMemoryUsed = PMC.PeakPagefileUsage;
}


void WindowsPlatformMemory::GetMemoryDesc()
{
	if (MemoryDesc.TotalPhysicalMemory == 0)
	{
		MEMORYSTATUSEX MSE;
		WindowsPlatformMemory::Memzero(&MSE, sizeof(MEMORYSTATUSEX));
		MSE.dwLength = sizeof(MEMORYSTATUSEX);
		GlobalMemoryStatusEx(&MSE);
		
		SYSTEM_INFO STI;
		WindowsPlatformMemory::Memzero(&STI, sizeof(SYSTEM_INFO));
		GetSystemInfo(&STI);

		MemoryDesc.TotalPhysicalMemory = MSE.ullTotalPhys;
		MemoryDesc.TotalVirtualMemory = MSE.ullTotalVirtual;
		MemoryDesc.PageSize = STI.dwPageSize;
	}

}