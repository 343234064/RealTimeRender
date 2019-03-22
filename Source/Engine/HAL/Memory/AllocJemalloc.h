/****************************************
Jemalloc memory allocator
http://jemalloc.net/

*****************************************/
#pragma once

#include "Core/GlobalDefinations.h"
#include "HAL/Memory/AllocatorInterface.h"


class AllocJemalloc :public MallocInterface
{
public:
	virtual void* Malloc(Size_T Size, uint32 Align = DEFAULT_ALIGNMENT_ARG) override;
	virtual void* Realloc(void* Dest, Size_T Size, uint32 Align = DEFAULT_ALIGNMENT_ARG) override;
	virtual void  Free(void* Dest) override;

	FORCE_INLINE
	virtual bool IsThreadSafe() override { return true; }


protected:

};