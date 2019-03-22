/********************************
Platform Memory


Do management of platform memory
*********************************/
#pragma once

#include "Core/GlobalConfigs.h"
#include "HAL/Memory/AllocatorInterface.h"




//Create the global memory allocator
//Thread Unsafe, must be called before the engine build up
void CreateGlobalMalloc();



enum class AllocatorTypes
{
	//Default fallback allocator
	Default,
	//Intel's Threading Building Blocks allocator
	TBB,
	//Jemalloc allocator
	Jemalloc
};


struct PlatformMemoryDesc
{
	PlatformMemoryDesc() :
		TotalPhysicalMemory(0),
		TotalVirtualMemory(0),
		PageSize(0)
	{}

	PlatformMemoryDesc(const PlatformMemoryDesc& Other) :
		TotalPhysicalMemory(Other.TotalPhysicalMemory),
		TotalVirtualMemory(Other.TotalVirtualMemory),
		PageSize(Other.PageSize)
	{}

	//The total size of actual physical memory, in bytes
	uint64 TotalPhysicalMemory;
	//The total size of virtual memory, in bytes
	uint64 TotalVirtualMemory;
	//The size of physical page, in bytes
	Size_T PageSize;

};


struct PlatformMemoryStates
{
	PlatformMemoryStates():
		PhysicalMemoryAvailable(0),
		VirtualMemoryAvailable(0),
		PhysicalMemoryUsed(0),
		VirtualMemoryUsed(0),
		PeakPhysicalMemoryUsed(0),
		PeakVirtualMemoryUsed(0)
	{}

	PlatformMemoryStates(const PlatformMemoryStates& Other) :
		PhysicalMemoryAvailable(Other.PhysicalMemoryAvailable),
		VirtualMemoryAvailable(Other.VirtualMemoryAvailable),
		PhysicalMemoryUsed(Other.PhysicalMemoryUsed),
		VirtualMemoryUsed(Other.VirtualMemoryUsed),
		PeakPhysicalMemoryUsed(Other.PeakPhysicalMemoryUsed),
		PeakVirtualMemoryUsed(Other.PeakVirtualMemoryUsed)
	{}

	//The total size of currently physical memory available, in bytes
	uint64 PhysicalMemoryAvailable;

	//The total size of currently virtual memory available, in bytes
	uint64 VirtualMemoryAvailable;

	//The total size of currently used memory by process, in bytes
	uint64 PhysicalMemoryUsed;
	uint64 VirtualMemoryUsed;

	//The peak size of memory used by process, in bytes
	uint64 PeakPhysicalMemoryUsed;
	uint64 PeakVirtualMemoryUsed;


};






class PlatformMemoryInterface
{
public:
	static void Init();//Should be called in platform memory init interface 
	static void UnInit();//Should be called in platform memory uninit interface
    static void OutOfMemoryHandling(uint64 Size, uint32 Alignment);
	static void OutputMemoryStatistics();
	static void UpdateMemoryStatistics();
	static uint32 GetBackupOOMMemorySize() { return BackupOOMMemorySize; }

public:
	static PlatformMemoryDesc MemoryDesc;

protected:
	static bool IsOutOfMemory;
	
	//The size and alignment of allocation that caused out of memory
	static uint64 CauseOOMAllocationSize;
	static uint32 CauseOOMAllocationAlign;

	//The back up memory used by OOM reporting and handling.
	//Using virtual memory page(Allocated pages from os).
    //Actual physical pages are not allocated unless/until the virtual addresses are actually accessed(when OOM accurred)
	static void* BackupOOMMemory;


/*****************************************
Platform specified functions and data,
need to be implemented by different platforms
*****************************************/
public:
	//Get global allocator, and assign to gMallocator 
	static MallocInterface* Allocator() { /*log*/return nullptr; }
	//Allocates pages from the OS
	static void* AllocPageFromOS(Size_T Size) { /*log*/return nullptr; }
	//Free pages to the OS
	static void  FreePageFromOS(void* Dest, Size_T Size) { /*log*/return; }
	static void GetMemoryState(PlatformMemoryStates& State) {/*log*/return; }

protected:
	static void GetMemoryDesc() {/*log*/return; }

protected:
	static uint32 BackupOOMMemorySize;



/*****************************************
System memory operations
*****************************************/
public:
	FORCE_INLINE
	static void* Memmove(void* Dest, const void* Src, Size_T Size)
	{
		return memmove(Dest, Src, Size);
	}

	FORCE_INLINE
	static int32 Memcmp(const void* Src1, const void* Src2, Size_T Size)
	{
		return memcmp(Src1, Src2, Size);
	}

	FORCE_INLINE
	static void* Memset(void* Dest, uint8 Value, Size_T Size)
	{
		return memset(Dest, Value, Size);
	}

	FORCE_INLINE
	static void* Memzero(void* Dest, Size_T Size)
	{
		return memset(Dest, 0, Size);
	}

	FORCE_INLINE
	static void* Memcopy(void* Dest, const void* Src, Size_T Size)
	{
		return memcpy(Dest, Src, Size);
	}

};




