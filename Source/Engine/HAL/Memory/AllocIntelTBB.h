/****************************************
Intel's Threading Building Blocks
64-bit scalable memory allocator
https://www.threadingbuildingblocks.org/

*****************************************/
#pragma once

#include "Global/GlobalType.h"
#include "HAL/Memory/AllocatorInterface.h"


class AllocTBB :public MallocInterface
{
public:
	virtual void* Malloc(Size_T Size, uint32 Align = DEFAULT_ALIGNMENT_ARG) override;
	virtual void* Realloc(void* Dest, Size_T Size, uint32 Align = DEFAULT_ALIGNMENT_ARG) override;
	virtual void  Free(void* Dest) override;

	FORCE_INLINE
	virtual bool IsThreadSafe() override { return true; }


protected:

};
