/****************************************
Default Memory Allocator
Basically unused
*****************************************/
#pragma once

#include "Global/GlobalType.h"
#include "HAL/Memory/AllocatorInterface.h"


class AllocDefault :public MallocInterface
{
public:
	AllocDefault();
	virtual void* Malloc(Size_T Size, uint32 Align = DEFAULT_ALIGNMENT_ARG) override;
	virtual void* Realloc(void* Dest, Size_T Size, uint32 Align = DEFAULT_ALIGNMENT_ARG) override;
	virtual void  Free(void* Dest) override;
    virtual bool IsThreadSafe() override;


protected:

};
