/********************************
Global Memory Allocator



*********************************/

#pragma once

#include "Global/GlobalType.h"
#include "Global/Utilities/AtomicCounter.h"


/***************************
Global allocator instance
***************************/
extern class MallocInterface* gMallocator;

 enum 
{
	//use allocator default alignment 
	ALIGNMENT_DEFAULT = 0,

	//if alloc size >= 16 bytes, aligned with 16 bytes
	//else aligned with 8 bytes
	ALIGNMENT_MIXED = 1,

	//always 8 bytes
	ALIGNMENT_8BYTE = 8,

	//always 16 bytes
	ALIGNMENT__16BYTE = 16
};


#define DEFAULT_ALIGNMENT_ARG ALIGNMENT_DEFAULT


class MallocInterface
{
public:
	virtual void* Malloc(Size_T Size, uint32 Align = DEFAULT_ALIGNMENT_ARG) = 0;
	virtual void* Realloc(void* Dest, Size_T Size, uint32 Align = DEFAULT_ALIGNMENT_ARG) = 0;
	virtual void  Free(void* Dest) = 0;

	virtual void OutOfMemory(Size_T Size, uint32 Align);

	virtual void UpdateAllocStatistics();
	virtual void OutputAllocStatistics();

	
	void* operator new(size_t Size);
	void  operator delete(void* Dest);
	void* operator new[](size_t Size);
	void  operator delete[](void* Dest);


	FORCE_INLINE
	virtual bool IsThreadSafe() { return false; }

protected:
	FORCE_INLINE
	void TotalMallocCount() { TotalMallocCalls.Increment(); }

	FORCE_INLINE
	void TotalReallocCount() { TotalReallocCalls.Increment(); }

	FORCE_INLINE
	void TotalFreeCount() { TotalFreeCalls.Increment(); }

protected:
	static AtomicCounter<int32> TotalMallocCalls;
	static AtomicCounter<int32> TotalReallocCalls;
	static AtomicCounter<int32> TotalFreeCalls;

};

