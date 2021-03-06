/****************************************
Thread safe decorator

*****************************************/
#pragma once

#include "Global/Utilities/Misc.h"
#include "Global/GlobalType.h"
#include "HAL/Platforms/GenericAllocator.h"


class AllocThreadSafeDecorator :public MallocInterface
{
public:
	AllocThreadSafeDecorator(MallocInterface* Malloc):
		InternalMalloc(Malloc)
	{}
	virtual ~AllocThreadSafeDecorator() {}

	virtual void* Malloc(Size_T Size, uint32 Align = DEFAULT_ALIGNMENT_ARG) override
	{
		LockGuard<PlatformCriticalSection> Lock(CriticalSection);
		return InternalMalloc->Malloc(Size, Align);
	}


	virtual void* Realloc(void* Dest, Size_T Size, uint32 Align = DEFAULT_ALIGNMENT_ARG) override
	{
		LockGuard<PlatformCriticalSection> Lock(CriticalSection);
		return InternalMalloc->Realloc(Dest, Size, Align);
	}


	virtual void  Free(void* Dest) override
	{
		LockGuard<PlatformCriticalSection> Lock(CriticalSection);
		InternalMalloc->Free(Dest);
	}


	virtual void OutputAllocStatistics() override
	{
		LockGuard<PlatformCriticalSection> Lock(CriticalSection);
		InternalMalloc->OutputAllocStatistics();
	}


	virtual bool IsThreadSafe() override { return true; }


protected:
	MallocInterface* InternalMalloc;
	PlatformCriticalSection CriticalSection;
};