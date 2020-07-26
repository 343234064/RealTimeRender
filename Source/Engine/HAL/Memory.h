/************************************
Memory relatives


*************************************/
#pragma once


#include "HAL/Platform.h"


#if PLATFORM_WINDOWS
#include "HAL/Platforms/Windows/WindowsMemory.h"
#elif PLATFORM_LINUX
#include "HAL/Platforms/Linux/LinuxMemory.h"
#elif PLATFORM_MAC
#include "HAL/Platforms/Mac/MacMemory.h"
#else
#error Unknown platform
#endif

#include "HAL/Platforms/GenericMemory.h"

/*
  Memory Interfaces Entrance
  Wrapper of PlatformMemory
  Must be initialized using PlatformMemory::Init()
  Must be uninitialized using PlatformMemory::UnInit()
  All in Bytes
*/
namespace Memory
{
	FORCE_INLINE
	void* Move(void* Dest, const void* Src, Size_T Size)
	{
		return PlatformMemory::Memmove(Dest, Src, Size);
	}

	FORCE_INLINE
	int32 Compare(const void* Src1, const void* Src2, Size_T Size)
	{
		return PlatformMemory::Memcmp(Src1, Src2, Size);
	}

	FORCE_INLINE
	void* Set(void* Dest, uint8 Value, Size_T Size)
	{
		return PlatformMemory::Memset(Dest, Value, Size);
	}

	FORCE_INLINE
	void* Zero(void* Dest, Size_T Size)
	{
		return PlatformMemory::Memzero(Dest, Size);
	}

	FORCE_INLINE
	void* Copy(void* Dest, const void* Src, Size_T Size)
	{
		return PlatformMemory::Memcopy(Dest, Src, Size);
	}

	FORCE_INLINE
	void* Malloc(Size_T Size, uint32 Align = ALIGNMENT_DEFAULT)
	{
		void* NewMemory = nullptr;
		NewMemory = gMallocator->Malloc(Size, Align);
		//track
		return NewMemory;
	}

	FORCE_INLINE
	void* Realloc(void* Dest, Size_T Size, uint32 Align = ALIGNMENT_DEFAULT)
	{
		void* NewMemory = nullptr;
		NewMemory = gMallocator->Realloc(Dest, Size, Align);
		//track
		return NewMemory;
	}

	FORCE_INLINE
	void Free(void* Dest)
	{
		//track
		gMallocator->Free(Dest);
	}


	FORCE_INLINE
	void* SystemMalloc(Size_T Size)
	{
		return ::malloc(Size);
	}

	FORCE_INLINE
	void SystemFree(void* Dest)
	{
		::free(Dest);
	}




	//////////////////////////////////////////////////////////////////////
    //ConstructItem
    //////////////////////////////////////////////////////////////////////
	template <typename ItemType>
	FORCE_INLINE 
	typename FuncTrigger<!IsZeroConstructType<ItemType>::Value>::Type
	DefaultConstructItem(ItemType* Address, int32 Count)
	{
		ItemType* Element = Address;
		while (Count)
		{
			new (Element) ItemType;
			++Element;
			--Count;
		}
	}


	template <typename ItemType>
	FORCE_INLINE 
	typename FuncTrigger<IsZeroConstructType<ItemType>::Value>::Type
	DefaultConstructItem(ItemType* Elements, int32 Count)
	{
		PlatformMemory::Memset(Elements, 0, sizeof(ItemType) * Count);
	}



	template <typename DestType, typename ItemType>
	FORCE_INLINE
	typename FuncTrigger<!IsBitwiseConstructible<DestType, ItemType>::Value, void>::Type
	ConstructItem(DestType* Dest, const ItemType* SrcItem)
	{
		new (Dest) DestType(*SrcItem);
	}

	template <typename DestType, typename ItemType>
	FORCE_INLINE
	typename FuncTrigger<IsBitwiseConstructible<DestType, ItemType>::Value, void>::Type
	ConstructItem(DestType* Dest, const ItemType* SrcItem)
	{
		PlatformMemory::Memcopy(Dest, SrcItem, sizeof(ItemType));
	}


	template <typename DestType, typename ItemType>
	FORCE_INLINE
	typename FuncTrigger<!IsBitwiseConstructible<DestType, ItemType>::Value, void>::Type
	ConstructItem(DestType* Dest, const ItemType* SrcItems, int32 Num)
	{
		while (Num)
		{
			new (Dest) DestType(*SrcItems);
			++(DestType*&)Dest;//ref of a Rvalue, so can using ++
			++SrcItems;
			--Num;
		}
	}

	template <typename DestType, typename ItemType>
	FORCE_INLINE
	typename FuncTrigger<IsBitwiseConstructible<DestType, ItemType>::Value, void>::Type
	ConstructItem(DestType* Dest, const ItemType* SrcItems, int32 Num)
	{
		PlatformMemory::Memcopy(Dest, SrcItems, sizeof(ItemType) * Num);
	}




	//////////////////////////////////////////////////////////////////////
	//DestructItem
	//////////////////////////////////////////////////////////////////////
	template <typename ItemType>
	FORCE_INLINE
	typename FuncTrigger<!IsTriviallyDestructible<ItemType>::Value, void>::Type
	DestructItem(ItemType* Item)
	{
		//VC do not compile the destructor call if ItemType itself has a member called ItemType
		//typedef to avoid the possible conflict naming problem 
		typedef ItemType DestructItemType;
		Item->DestructItemType::~DestructItemType();
	}

	template <typename ItemType>
	FORCE_INLINE
	typename FuncTrigger<IsTriviallyDestructible<ItemType>::Value, void>::Type
	DestructItem(ItemType* Item) {}


	template <typename ItemType>
	FORCE_INLINE
	typename FuncTrigger<!IsTriviallyDestructible<ItemType>::Value, void>::Type
	DestructItem(ItemType* Items, int32 Num)
	{
		while (Num)
		{
			typedef ItemType DestructItemType;
			Items->DestructItemType::~DestructItemType();
			++Items;
			--Num;
		}
	}

	template <typename ItemType>
	FORCE_INLINE
	typename FuncTrigger<IsTriviallyDestructible<ItemType>::Value, void>::Type
	DestructItem(ItemType* Items, int32 Num) {}




	//////////////////////////////////////////////////////////////////////
    //RelocateItem
	//Move the Item to Dest
    //////////////////////////////////////////////////////////////////////
	template <typename DestType, typename ItemType>
	FORCE_INLINE
	typename FuncTrigger<!IsBitwiseRelocatable<DestType, ItemType>::Value, void>::Type
	RelocateItem(DestType* Dest, const ItemType* SrcItems, int32 Num)
	{
		while (Num)
		{
			new (Dest) DestType(*SrcItems);
			++(DestType*&)Dest;

			typedef ItemType DestructItemType;
			SrcItems->DestructItemType::~DestructItemType();
			++SrcItems;
			--Num;
		}
	}

	template <typename DestType, typename ItemType>
	FORCE_INLINE
	typename FuncTrigger<IsBitwiseRelocatable<DestType, ItemType>::Value, void>::Type 
	RelocateItem(DestType* Dest, const ItemType* SrcItems, int32 Num)
	{
		PlatformMemory::Memmove(Dest, SrcItems, sizeof(ItemType) * Num);
	}



	//////////////////////////////////////////////////////////////////////
    //CompareItem
    //
    //////////////////////////////////////////////////////////////////////
	template <typename ItemType>
	FORCE_INLINE
	typename FuncTrigger<!IsBitwiseComparable<ItemType>::Value, bool>::Type
	CompareItem(const ItemType* ItemA, const ItemType* ItemB, int32 Num)
	{
		while (Num)
		{
			if (*ItemA != *ItemB)
			{
				return false;
			}
			++ItemA;
			++ItemB;
			--Num;
		}

		return true;
	}

	template <typename ItemType>
	FORCE_INLINE
	typename FuncTrigger<IsBitwiseComparable<ItemType>::Value, bool>::Type
	CompareItem(const ItemType* ItemA, const ItemType* ItemB, int32 Num)
	{
		return (PlatformMemory::Memcmp(ItemA, ItemA, sizeof(ItemType) * Num) == 0);
	}



	//////////////////////////////////////////////////////////////////////
    //Copy Assign Item
    //
    //////////////////////////////////////////////////////////////////////
	template <typename ItemType>
	FORCE_INLINE
	typename FuncTrigger<IsTriviallyCopyAssignable<ItemType>::Value>::Type
	CopyAssignItem(ItemType* Dest, const ItemType* Src, int32 Num)
	{
		PlatformMemory::Memcopy(Dest, Src, Num * sizeof(ItemType));
	}


	template <typename ItemType>
	FORCE_INLINE
	typename FuncTrigger<!IsTriviallyCopyAssignable<ItemType>::Value>::Type
	CopyAssignItem(ItemType* Dest, const ItemType* Src, int32 Num)
	{
		while (Num)
		{
			*Dest++ = *Src++;
			--Num;
		}
	}
};
