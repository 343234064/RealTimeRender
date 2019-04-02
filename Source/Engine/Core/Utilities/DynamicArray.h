#pragma once

#include <utility>
#include <algorithm>
#include <functional>
#include <initializer_list>

#include "Core/GlobalConfigs.h"
#include "Core/GlobalDefinations.h"
#include "Core/Utilities/ContainerAllocator.h"
#include "HAL/Memory.h"


/**
  Test template for Dynamic Array List to test if two
  SrcArray can be moved to DestArray
 */
template <typename SrcArrayType, typename DestArrayType>
struct IsArrayPointerCanBeMoved
{
	typedef typename SrcArrayType::AllocatorType  SrcAllocatorType;
	typedef typename SrcArrayType::ElementType    SrcElementType;
	typedef typename DestArrayType::AllocatorType DestAllocatorType;
	typedef typename DestArrayType::ElementType   DestElementType;


	enum
	{
		Value = (
			IsTypeEqual< SrcAllocatorType, DestAllocatorType >::Value &&
			(
				IsTypeEqual< SrcElementType, DestElementType >::Value ||
				IsBitwiseConstructible< DestElementType, SrcElementType >::Value
				)
			)
	};

};



/**
  Helper functions for Class Array
  Giving the number of current elements then output the size that should be growed to
  Call to calculate the actual size we need that has slack to avoid calling Realloc() or Malloc() too much times
  when calling function like Add()
  The output will be in range[4, MAX_INT32]
 */
FORCEINLINE 
int32 GrowPolicy(int32 CurrentElementNum, int32 MaxElementNum)
{
	//check
	Size_T Grow = 4;
	if (MaxElementNum || (Size_T(CurrentElementNum) > Grow))
	{
		Grow = Size_T(CurrentElementNum) + 3 * Size_T(CurrentElementNum) / 8 + 16;
	}

	if (Grow > MAX_INT32)
		Grow = MAX_INT32;

	return (int32)Grow;
}



/**
  Helper functions for Class Array
  Giving the number of current element slack and max element size then output the size that should be shrinked
  The output MUST be in range[0, ElementSlackNum]
 */
FORCEINLINE
int32 ShrinkPolicy(int32 ElementSlackNum, int32 MaxElementNum, int32 BytesPerElement)
{
	//check

	int32 ShrinkedNum = 0;

	//If ElementSlackNum > 64
	bool TooMuchSlack = ElementSlackNum > 64;
	//If ElementSlackNum / MaxElementNum > 33%
	bool TooMuchSlackPercent = 3 * ElementSlackNum > MaxElementNum;
	//If BytesOfSlack > 16kB
	bool TooMuchSlackBytes = ElementSlackNum * BytesPerElement >= 16384;
	if (TooMuchSlack && (TooMuchSlackPercent || TooMuchSlackBytes))
	{
		ShrinkedNum = ElementSlackNum;
	}

	return ShrinkedNum;
}


/**
  Dynamic Array List
  * Continuous memory array
  * Memory operation seperated
 */
template<typename TElementType, typename TAllocatorType = BaseContainerAllocator<TElementType>>
class Array
{
public:

	typedef TElementType    ElementType;
	typedef TAllocatorType  AllocatorType;

public:
	//Default constructor
	FORCE_INLINE
	Array() :CurrentElementNum(0), MaxElementNum(0) {}

	FORCE_INLINE
	Array(const ElementType* InRawArrayPtr, int32 InElementNum)
	{
		//check
		InitDataCopyFrom(InRawArrayPtr, InElementNum);
	}

	//Initializer List 
	Array(std::initializer_list<ElementType> List)
	{
		InitDataCopyFrom(List.begin(), List.size());
	}


	Array& operator=(std::initializer_list<ElementType> List)
	{
		Memory::DestructItem(Begin(), CurrentElementNum);
		InitDataCopyFrom(List.begin(), (int32)List.size());
		return *this;
	}

	//Copy constructor and assginment
	FORCE_INLINE
	Array(const Array& Other)
	{
		InitDataCopyFrom(Other.Begin(), Other.CurrentNum());
	}


	Array& operator=(const Array& Other)
	{
		if (this != &Other)
		{
			Memory::DestructItem(Begin(), CurrentElementNum);
			InitDataCopyFrom(Other.Begin(), Other.CurrentSize());
		}
		return *this;
	}

	//Move constructor and assignment
	FORCE_INLINE
	Array(Array&& Other)
	{
		Move(*this, Other);
	}


	Array& operator=(Array&& Other)
	{
		if (this != &Other)
		{
			Memory::DestructItem(Begin(), CurrentElementNum);
			Move(*this, Other);
		}
		return *this;
	}

	//Destructor
	~Array()
	{
		ClearElements();
		//Allocator called its destructor to free allocation after
	}


	FORCE_INLINE
	ElementType& operator[](int32 Index)
	{
		//check
		return Begin()[Index];
	}


	FORCE_INLINE
	const ElementType& operator[](int32 Index) const
	{
		//check
		return Begin()[Index];
	}


	FORCE_INLINE
	int32 CurrentNum() const { return CurrentElementNum; }

	FORCE_INLINE
	int32 MaxNum() const { return MaxElementNum; }

	FORCE_INLINE
	uint32 TypeSize() const { return sizeof(ElementType); }

	FORCE_INLINE
	Size_T AllocatedSize() const { return Allocator.AllocatedSize(MaxElementNum, sizeof(ElementType)); }

	FORCE_INLINE
	int32 RemainElementSize() const { return MaxElementNum - CurrentElementNum; }


	/*
	  Append a new array to the last
	*/
	void Append(const Array& Other)
	{
		if (this != &Other && Other.CurrentElementNum)
		{
			int32 AddIndex = AddUninitialize(Other.CurrentElementNum);
			Memory::ConstructItem<ElementType>(Begin() + AddIndex, Other.Begin(), Other.CurrentElementNum);
		}
	}

	/*
      Append a new array to the last
	  Rvalue version
    */
	void Append(Array&& Other)
	{
		if (this != &Other && Other.CurrentElementNum)
		{
			int32 AddIndex = AddUninitialize(Other.CurrentElementNum);
			//Relocate it 
			Memory::RelocateItem<ElementType>(Begin() + AddIndex, Other.Begin(), Other.CurrentElementNum);
			Other.CurrentElementNum = 0;
		}
	}

	/*
      Append a new array to the last
      std::initializer_list version
    */
	void Append(std::initializer_list<ElementType> List)
	{
		int32 Num = (int32)List.size();
		int32 AddIndex = AddUninitialize(Num);
		Memory::ConstructItem<ElementType>(Begin() + AddIndex, List.begin(), Num);
	}

	Array& operator+=(Array& Other)
	{
		Append(Other);
		return *this;
	}

	Array& operator+=(Array&& Other)
	{
		Append(std::move(Other));
		return *this;
	}

	Array& operator+=(std::initializer_list<ElementType> List)
	{
		Append(List);
		return *this;
	}

	bool operator==(const Array& Other) const
	{
		return (
			    (CurrentElementNum == Other.CurrentElementNum) &&
			     Memory::CompareItem<ElementType>(Begin(), Other.Begin(), CurrentElementNum)
			   );
	}

	FORCE_INLINE
	bool operator!=(const Array& Other) const
	{
		return !((*this) == Other);
	}



	FORCE_INLINE
	ElementType Pop(bool AllowShrinking = true)
	{
		//check
		ElementType Element = std::move(Begin()[CurrentElementNum - 1]);
		RemoveAt(CurrentElementNum - 1, 1, AllowShrinking);
		return Element;
	}

	FORCE_INLINE
	ElementType& Top()
	{
		//check
		return Begin()[CurrentElementNum - 1];
	}

	FORCE_INLINE
	void Push(const ElementType& Item)
	{
		Add(Item);
	}

	FORCE_INLINE
	void Push(ElementType&& Item)
	{
		Add(std::move(Item));
	}

	/*
	   Add element to the last, with its init arguments
	   return the index of that element
	*/
	template <typename ... ArgsType>
	FORCE_INLINE
	int32 AddWithInitArgs(ArgsType&& ... Args)
	{
		int32 ElementTail = AddUninitialize();
		new(Begin() + ElementTail) ElementType(std::forward<ArgsType>(Args)...);
		return ElementTail;
	}


	/*
       Add element to the last
       return the index of that element
    */
	FORCE_INLINE
		int32 Add(const ElementType& Item)
	{
		return AddWithInitArgs(Item);
	}


	/*
	   Add element to the last
	   return the index of that element
	   Rvalue version
	*/
	FORCE_INLINE
	int32 Add(ElementType&& Item)
	{
		return AddWithInitArgs(std::move(Item));
	}



	/*
       Insert element, with its init arguments
    */
	template <typename... ArgsType>
	FORCE_INLINE
	void InsertWithInitArgs(const int32 InsertIndex, ArgsType&& ... Args)
	{
		//check address
		InsertUninitialize(InsertIndex);
		new(Begin() + InsertIndex) ElementType(std::forward<ArgsType>(Args)...);
	}
	
	/*
       Insert element
    */
	FORCE_INLINE
	void Insert(const int32 InsertIndex, const ElementType& InsertElement)
	{
		//check address
		InsertWithInitArgs(InsertIndex, InsertElement);
	}

	/*
       Insert element
       Rvalue version
    */
	FORCE_INLINE
	void Insert(const int32 InsertIndex, ElementType&& InsertElement)
	{
		//check address
		InsertWithInitArgs(InsertIndex, std::move(InsertElement));
	}

	/*
	   Insert element using std::initializer_list
	*/
	void Insert(const int32 InsertIndex, std::initializer_list<ElementType> InsertList)
	{
		//check
		
		int32 InsertNum = InsertList.size();
		InsertUninitialize(InsertIndex, InsertNum);
		Memory::ConstructItem(Begin() + InsertIndex, InsertList.begin(), InsertNum);

	}

	/*
       Insert element using a raw array pointer
    */
	void Insert(const int32 InsertIndex, const int32 InsertNum, const ElementType* InsertElementsPtr)
	{
		//check
		InsertUninitialize(InsertIndex, InsertNum);
		Memory::ConstructItem(Begin() + InsertIndex, InsertElementsPtr, InsertNum);
	}


	/*
	  Return the index of element
	  Return -1 when find nothing
	*/
	int32 Find(const ElementType& ToFind)
	{
		const ElementType* RESTRICT First = Begin();
		for (const ElementType* RESTRICT Iter = First, *RESTRICT End = Iter + CurrentElementNum; Iter != End; ++Iter)
		{
			if (*Iter == ToFind)
			{
				return static_cast<int32>(Iter - First);
			}
		}
		return -1;
	}

	/*
	   Return the index of element
	   Return -1 when find nothing
	*/
	int32 FindFromTail(const ElementType& ToFind)
	{
		const ElementType* RESTRICT First = Begin();
		for (const ElementType* RESTRICT Iter = First + CurrentElementNum; Iter != First; )
		{
			--Iter;
			if (*Iter == ToFind)
			{
				return static_cast<int32>(Iter - First);
			}
		}
		return -1;
	}

	/*
	   Return the index of element
	   ElementType must implemented operator== for KeyType
	   Return -1 when find nothing
	*/
	template <typename KeyType>
	int32 FindByKey(const KeyType& Key)
	{
		const ElementType* RESTRICT First = Begin();
		for (const ElementType* RESTRICT Iter = First, *RESTRICT End = Iter + CurrentElementNum; Iter != End; ++Iter)
		{
			if (*Iter == Key)
			{
				return static_cast<int32>(Iter - First);
			}
		}
		return -1;
	}

	/*
       Return the index of element
       Return -1 when find nothing
	   Selector function type: bool (const ElementType&)
    */
	int32 FindBySelector(const std::function<bool(const ElementType&)>& Selector)
	{
		const ElementType* RESTRICT First = Begin();
		for (const ElementType* RESTRICT Iter = First, *RESTRICT End = Iter + CurrentElementNum; Iter != End; ++Iter)
		{
			if (Selector(*Iter))
			{
				return static_cast<int32>(Iter - First);
			}
		}
		return -1;
	}

	/*
	   Return the address of element
	   ElementType must implemented operator== for KeyType
	   Return nullptr when find nothing
	*/
	template <typename KeyType>
	ElementType* FindElementByKey(const KeyType& Key)
	{
		for (const ElementType* RESTRICT Iter = Begin(), *RESTRICT End = Iter + CurrentElementNum; Iter != End; ++Iter)
		{
			if (*Iter == Key)
			{
				return Iter;
			}
		}
		return nullptr;
	}

	/*
	   Filter based on a selector function
	   Selector function type: bool (const ElementType&)
	*/
	Array<ElementType> FilterBySelector(const std::function<bool(const ElementType&)>& Selector)
	{
		Array<ElementType> FilterArray;
		for (const ElementType* RESTRICT Iter = Begin(), *RESTRICT End = Iter + CurrentElementNum; Iter != End; ++Iter)
		{
			if (Selector(*Iter))
			{
				FilterArray.Add(*Iter);
			}
		}
		return FilterArray;
	}

	/*
	   Sort using the ueser defined < operator
	*/
	void Sort()
	{
		ElementType* First = Begin();
		ElementType* End = First + CurrentElementNum;

		std::sort(First, End, [](const ElementType& A, const ElementType& B)->bool { return A < B; });
	}

	/*
	   Sort using the ueser defined function
	*/
	void Sort(const std::function<bool(ElementType&)>& Comparator)
	{
		ElementType* First = Begin();
		ElementType* End = First + CurrentElementNum;

		std::sort(First, End, Comparator);
	}

	/*
	   Stable sort using the ueser defined < operator
	*/
	void StableSort()
	{
		ElementType* First = Begin();
		ElementType* End = First + CurrentElementNum;

		std::stable_sort(First, End, [](const ElementType& A, const ElementType& B)-> bool{ return A < B; });

	}

	/*
	   Stable sort using the ueser defined function
	*/
	void StableSort(const std::function<bool(ElementType&)>& Comparator)
	{
		ElementType* First = Begin();
		ElementType* End = First + CurrentElementNum;

		std::stable_sort(First, End, Comparator);
	}


	void RemoveAt(const int32 StartIndex, const int32 RemoveNum = 1, bool AllowShrinking = true)
	{
		if (RemoveNum)
		{
			//check

			Memory::DestructItem(Begin() + StartIndex, RemoveNum);

			int32 ElementsNeededToMove = CurrentElementNum - StartIndex - RemoveNum;
			if (ElementsNeededToMove)
			{
				              //Cast Begin() to byte*
				Memory::Move((uint8*)Begin() + StartIndex * sizeof(ElementType),
					         (uint8*)Begin() + (StartIndex + RemoveNum) * sizeof(ElementType),
					         ElementsNeededToMove * sizeof(ElementType));
			}

			CurrentElementNum -= RemoveNum;
			
			if (AllowShrinking)
			{
				Shrink();
			}
		}
	}


	/*
      Force the array to fit the elements 
    */
	FORCE_INLINE
	void ForceShrink()
	{
		//check
		Resize(CurrentElementNum);
	}

	/*
	  If the current element slack satisfies the conditions in ShrinkPolicy(),
	  then resize the array to the best size 
    */
	void Shrink()
	{
		//check
		int32 ShrinkNum = ShrinkPolicy(MaxElementNum - CurrentElementNum, MaxElementNum, sizeof(ElementType));
		if (ShrinkNum)
		{
			int32 NewMaxNum = MaxElementNum - ShrinkNum;
			Resize(NewMaxNum);
		}
		
	}

	/*
	   Clear all the elements, but does not change the array memory allocation
	*/
	FORCE_INLINE
	void ClearElements()
	{
		//check

		if (CurrentElementNum)
		{
			Memory::DestructItem(Begin(), CurrentElementNum);
		}

		CurrentElementNum = 0;
	}

	/*
       Clear all the elements and allocation
    */
	FORCE_INLINE
	void Empty()
	{
		//check
		ClearElements();
		Resize(0);
	}


	/*
      Add Uninitialize element to the last
      Return the element num that calling AddUninitialize() before
    */
	int32 AddUninitialize(int32 Num = 1)
	{
		//check

		int32 OldElementNum = CurrentElementNum;
		if ((CurrentElementNum += Num) > MaxElementNum)
		{
			int32 NewMaxElementNum = GrowPolicy(CurrentElementNum, MaxElementNum);
			Allocator.Resize(MaxElementNum, NewMaxElementNum);
			MaxElementNum = NewMaxElementNum;
		}
		return OldElementNum;
	}

	/*
	  Insert Uninitialize element to the array
	*/
	void InsertUninitialize(int32 Index, int32 Num = 1)
	{
		//check
		int32 OldElementNum = CurrentElementNum;
		if ((CurrentElementNum += Num) > MaxElementNum)
		{
			int32 NewMaxElementNum = GrowPolicy(CurrentElementNum, MaxElementNum);
			Allocator.Resize(MaxElementNum, NewMaxElementNum);
			MaxElementNum = NewMaxElementNum;
		}
		ElementType* MoveData = Begin() + Index;
		Memory::RelocateItem(MoveData + Num, MoveData, OldElementNum - Index);

	}


	/*
      Unique memory entrance
    */
	FORCE_INLINE
	ElementType* Begin() { return Allocator.Address(); }

	FORCE_INLINE
	const ElementType* Begin() const { return Allocator.Address(); }

private:
	/*
       Resize the allocation to NewMaxElementNum
	   Be careful if NewMaxElementNum < CurrentElementNum
    */
	FORCE_INLINE
	void Resize(int32 NewMaxElementNum)
	{
		//check 
		if (NewMaxElementNum != MaxElementNum)
		{
			Allocator.Resize(MaxElementNum, NewMaxElementNum);
			MaxElementNum = NewMaxElementNum;
		}

	}


	/*
	   Call to init the array data
       Assuming that the array is empty
    */
	void InitDataCopyFrom(const ElementType* InOtherDataPtr, int32 InElementNum, int32 ExtraSlack = 0)
	{
		//check
		if (InElementNum || ExtraSlack)
		{
			Resize(InElementNum + ExtraSlack);
			Memory::ConstructItem<ElementType>(Begin(), InOtherDataPtr, InElementNum);

			CurrentElementNum = InElementNum;
			MaxElementNum = InElementNum + ExtraSlack;
		}

	}


	template <typename SrcArrayType, typename DestArrayType>
	FORCE_INLINE
	typename FuncTrigger<IsArrayPointerCanBeMoved<SrcArrayType, DestArrayType>::Value>::Type
	Move(DestArrayType& DestArray, SrcArrayType& SrcArray)
	{
		//Array pointer can be moved
		DestArray.Allocator.MoveFrom(&(SrcArray.Allocator));

		DestArray.CurrentElementNum = SrcArray.CurrentElementNum;
		DestArray.MaxElementNum = SrcArray.MaxElementNum;

		SrcArray.CurrentElementNum = 0;
		SrcArray.MaxElementNum = 0;
	}

	template <typename SrcArrayType, typename DestArrayType>
	FORCE_INLINE
	typename FuncTrigger<!(IsArrayPointerCanBeMoved<SrcArrayType, DestArrayType>::Value)>::Type
	Move(DestArrayType& DestArray, SrcArrayType& SrcArray)
	{
		//Array pointer can not be moved, so do copy
		DestArray.InitDataCopyFrom(SrcArray.Begin(), SrcArray.CurrentSize());
	}


private:
	typedef AllocatorType AllocatorInstance;

	AllocatorInstance Allocator;
	int32 CurrentElementNum;
	int32 MaxElementNum;

};
