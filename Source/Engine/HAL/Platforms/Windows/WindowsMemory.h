/********************************
Windows Memory Interface


*********************************/
#pragma once


#include "HAL/Platforms/GenericMemory.h"



//Back up memory size to handle out of memory
//24 MB
#define WINDOWS_BACKUP_OOM_MEMORY_SIZE 24 * 1024 * 1024


class WindowsMemory :public PlatformMemoryInterface
{
public:
	static void Init();
	static void UnInit();

	static MallocInterface* Allocator();
	//Allocates pages from the OS
	static void* AllocPageFromOS(Size_T Size);
	//Free pages to the OS
	static bool  FreePageFromOS(void* Dest, Size_T Size);

	static PlatformMemoryStates GetMemoryState();

protected:
	static void GetMemoryDesc();


};


typedef WindowsMemory PlatformMemory;