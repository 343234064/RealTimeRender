/****************************************
Allocator used for containers

*****************************************/
#pragma once
#include "HAL/Memory.h"


/*
   Simple Base Allocator
*/
template <typename ElementType>
class BaseContainerAllocator
{
public:
	BaseContainerAllocator():
		Data(nullptr)
	{}

	virtual ~BaseContainerAllocator()
	{
		Release();
	}

	FORCE_INLINE
	virtual ElementType* Address() const { return Data; }
	
	FORCE_INLINE
	SIZE_T AllocatedSize(int32 MaxNum, SIZE_T SizePerElement) const { return MaxNum * SizePerElement; }
	
	FORCE_INLINE
	virtual void MoveFrom(BaseContainerAllocator* Other)
	{
		if (this != Other)
		{
			Release();
			Data = Other->Data;
			Other->Data = nullptr;
		}
	}

	virtual void Resize(uint32 OldElementNum, uint32 NewElementNum)
	{
		if (Data)
		{
			Data = (ElementType*)Memory::Realloc(Data, NewElementNum * sizeof(ElementType));
		}
		else
		{
			Data = (ElementType*)Memory::Malloc(NewElementNum * sizeof(ElementType));
		}
	}
	

	FORCE_INLINE
	virtual void Release()
	{
		if (Data)
		{
			Memory::Free(Data);
			Data = nullptr;
		}
	}

	BaseContainerAllocator(const BaseContainerAllocator& Other) = delete;
	BaseContainerAllocator& operator=(const BaseContainerAllocator& Other) = delete;
	BaseContainerAllocator(BaseContainerAllocator&& Other) = delete;
	BaseContainerAllocator& operator=(BaseContainerAllocator&& Other) = delete;

protected:
	ElementType* Data;
};