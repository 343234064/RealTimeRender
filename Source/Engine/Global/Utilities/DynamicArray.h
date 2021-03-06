#pragma once

#include <utility>
#include <algorithm>
#include <functional>
#include <initializer_list>

#include "Global/GlobalConfigs.h"
#include "Global/GlobalType.h"
#include "Global/Utilities/ContainerAllocator.h"
#include "Global/Utilities/Assertion.h"
#include "HAL/Memory.h"




template< typename ContainerType, typename ElementType, typename SizeType>
class IndexedContainerIterator
{
public:
	IndexedContainerIterator(ContainerType& InContainer, SizeType StartIndex = 0)
		: Container(InContainer)
		, Index(StartIndex)
	{
	}

	/** Advances iterator to the next element in the container. */
	IndexedContainerIterator& operator++()
	{
		++Index;
		return *this;
	}
	IndexedContainerIterator operator++(int)
	{
		IndexedContainerIterator Tmp(*this);
		++Index;
		return Tmp;
	}

	/** Moves iterator to the previous element in the container. */
	IndexedContainerIterator& operator--()
	{
		--Index;
		return *this;
	}
	IndexedContainerIterator operator--(int)
	{
		IndexedContainerIterator Tmp(*this);
		--Index;
		return Tmp;
	}

	/** iterator arithmetic support */
	IndexedContainerIterator& operator+=(SizeType Offset)
	{
		Index += Offset;
		return *this;
	}

	IndexedContainerIterator operator+(SizeType Offset) const
	{
		IndexedContainerIterator Tmp(*this);
		return Tmp += Offset;
	}

	IndexedContainerIterator& operator-=(SizeType Offset)
	{
		return *this += -Offset;
	}

	IndexedContainerIterator operator-(SizeType Offset) const
	{
		IndexedContainerIterator Tmp(*this);
		return Tmp -= Offset;
	}

	ElementType& operator* () const
	{
		return Container[Index];
	}

	ElementType* operator->() const
	{
		return &Container[Index];
	}

	/** conversion to "bool" returning true if the iterator has not reached the last element. */
	FORCE_INLINE explicit operator bool() const
	{
		return Container.IsValidIndex(Index);
	}

	/** Returns an index to the current element. */
	SizeType GetIndex() const
	{
		return Index;
	}

	/** Resets the iterator to the first element. */
	void Reset()
	{
		Index = 0;
	}

	/** Sets iterator to the last element. */
	void SetToEnd()
	{
		Index = Container.CurrentNum();
	}

	/** Removes current element in array. This invalidates the current iterator value and it must be incremented */
	void RemoveCurrent()
	{
		Container.RemoveAt(Index);
		Index--;
	}

	FORCEINLINE friend bool operator==(const IndexedContainerIterator& Lhs, const IndexedContainerIterator& Rhs) { return &Lhs.Container == &Rhs.Container && Lhs.Index == Rhs.Index; }
	FORCEINLINE friend bool operator!=(const IndexedContainerIterator& Lhs, const IndexedContainerIterator& Rhs) { return &Lhs.Container != &Rhs.Container || Lhs.Index != Rhs.Index; }

private:

	ContainerType& Container;
	SizeType      Index;
};


/** operator + */
template <typename ContainerType, typename ElementType, typename SizeType>
FORCE_INLINE IndexedContainerIterator<ContainerType, ElementType, SizeType> operator+(SizeType Offset, IndexedContainerIterator<ContainerType, ElementType, SizeType> RHS)
{
	return RHS + Offset;
}


template <typename ElementType, typename SizeType>
struct CheckedPointerIterator
{
	explicit CheckedPointerIterator(const SizeType& InNum, ElementType* InPtr) :
		Ptr(InPtr),
		CurrentNum(InNum),
		InitialNum(InNum)
	{

	}

	FORCE_INLINE ElementType& operator*() const
	{
		return *Ptr;
	}

	FORCE_INLINE CheckedPointerIterator& operator++()
	{
		++Ptr;
		return *this;
	}

	FORCE_INLINE CheckedPointerIterator& operator--()
	{
		--Ptr;
		return *this;
	}

private:
	ElementType* Ptr;
	const SizeType& CurrentNum;
	SizeType        InitialNum;

	FORCE_INLINE friend bool operator!=(const CheckedPointerIterator& Lhs, const CheckedPointerIterator& Rhs)
	{
		// We only need to do the check in this operator, because no other operator will be
		// called until after this one returns.
		//
		// Also, we should only need to check one side of this comparison - if the other iterator isn't
		// even from the same array then the compiler has generated bad code.
		CHECKF(Lhs.CurrentNum == Lhs.InitialNum, "Array has changed during ranged-for iteration!");
		return Lhs.Ptr != Rhs.Ptr;
	}
};





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
FORCE_INLINE 
int32 GrowPolicy(int32 CurrentElementNum, int32 AllocatedMaxElementNum)
{
	CHECK(CurrentElementNum >= 0);

	Size_T Grow = 4;
	if (AllocatedMaxElementNum || (Size_T(CurrentElementNum) > Grow))
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
FORCE_INLINE
int32 ShrinkPolicy(int32 ElementSlackNum, int32 MaxElementNum, int32 BytesPerElement)
{
	CHECK(ElementSlackNum >= 0);

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
		CHECK(InRawArrayPtr != nullptr || InElementNum > 0);
		InitDataCopyFrom(InRawArrayPtr, InElementNum);
	}

	//Initializer List 
	Array(std::initializer_list<ElementType> List)
	{
		InitDataCopyFrom(List.begin(), (int32)List.size());
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
			InitDataCopyFrom(Other.Begin(), Other.CurrentNum());
		}
		return *this;
	}

	//Move constructor and assignment
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
		CHECKF((Index >= 0 && Index < CurrentElementNum), "Array index out of bounds, current: %i, index: %i", CurrentElementNum, Index);
		return Begin()[Index];
	}


	FORCE_INLINE
	const ElementType& operator[](int32 Index) const
	{
		CHECKF((Index >= 0 && Index < CurrentElementNum), "Array index out of bounds, current: %i, index: %i", CurrentElementNum, Index);
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
		CHECK(CurrentElementNum > 0 && MaxElementNum >= CurrentElementNum);
		ElementType Element = std::move(Begin()[CurrentElementNum - 1]);
		RemoveAt(CurrentElementNum - 1, 1, AllowShrinking);
		return Element;
	}

	FORCE_INLINE
	ElementType& Top()
	{
		CHECK(CurrentElementNum > 0 && MaxElementNum >= CurrentElementNum);
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




	FORCE_INLINE
	int32 AddUnique(const ElementType& Item)
	{
		return AddUniqueInternal(Item);
	}

	FORCE_INLINE
	int32 AddUnique(ElementType&& Item)
	{
		return AddUniqueInternal(std::move(Item));
	}


	/*
	 Insert element, with its init arguments
	 */
	template <typename... ArgsType>
	FORCE_INLINE
	void InsertWithInitArgs(const int32 InsertIndex, ArgsType&& ... Args)
	{
		CHECKF((InsertIndex >= 0 && InsertIndex < CurrentElementNum), "Array index out of bounds, current: %i, index: %i", CurrentElementNum, InsertIndex);
		InsertUninitialize(InsertIndex);
		new(Begin() + InsertIndex) ElementType(std::forward<ArgsType>(Args)...);
	}
	
	/*
	Insert element
	*/
	FORCE_INLINE
	void Insert(const int32 InsertIndex, const ElementType& InsertElement)
	{
		InsertWithInitArgs(InsertIndex, InsertElement);
	}

	/*
       Insert element
       Rvalue version
	*/
	FORCE_INLINE
	void Insert(const int32 InsertIndex, ElementType&& InsertElement)
	{
		InsertWithInitArgs(InsertIndex, std::move(InsertElement));
	}

	/*
	   Insert element using std::initializer_list
	*/
	void Insert(const int32 InsertIndex, std::initializer_list<ElementType> InsertList)
	{
		CHECKF((InsertIndex >= 0 && InsertIndex < CurrentElementNum), "Array index out of bounds, current: %i, index: %i", CurrentElementNum, InsertIndex);
		int32 InsertNum = InsertList.size();
		InsertUninitialize(InsertIndex, InsertNum);
		Memory::ConstructItem(Begin() + InsertIndex, InsertList.begin(), InsertNum);

	}

	/*
       Insert element using a raw array pointer
	 */
	void Insert(const int32 InsertIndex, const int32 InsertNum, const ElementType* InsertElementsPtr)
	{
		CHECKF((InsertIndex >= 0 && InsertIndex < CurrentElementNum), "Array index out of bounds, current: %i, index: %i", CurrentElementNum, InsertIndex);
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
	int32 FindBySelector(const std::function<bool(const ElementType&)>& Selector, int32 StartIndex = 0)  const
	{
		CHECK(StartIndex >= 0 && StartIndex < CurrentElementNum);
		const ElementType* RESTRICT First = Begin() + StartIndex;
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
   Return the index of element
   Return -1 when find nothing
   Selector function type: bool (const ElementType&)
	*/
	int32 FindBySelectorFromEnd(const std::function<bool(const ElementType&)>& Selector, int32 StartIndex)  const
	{
		CHECK(StartIndex >= 0 && StartIndex < CurrentElementNum);
		for (const ElementType* RESTRICT First = Begin(), *RESTRICT Iter = First + StartIndex;  Iter != First; )
		{
			--Iter;
			if (Selector(*Iter))
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
	int32 FindBySelectorFromEnd(const std::function<bool(const ElementType&)>& Selector)  const
	{
		for (const ElementType* RESTRICT First = Begin(), *RESTRICT Iter = First + CurrentElementNum; Iter != First; )
		{
			--Iter;
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


	//Remove the first match item
	void Remove(const ElementType& ToRemove)
	{
		if (CurrentElementNum)
		{
			ElementType* Data = Begin();
			for (int32 Index = 0; Index < CurrentElementNum; Index++)
			{
				if (Data[Index] == ToRemove)
				{
					RemoveAt(Index, 1, false);
					break;
				}
			}
			
		}
	}

	//Remove the all match item
	void RemoveAllMatch(const ElementType& ToRemove, bool AllowShrinking = true)
	{
		if (!CurrentElementNum) return;

		int32 MoveIndex = 0;
		int32 MoveNum = 0;
		int32 HitIndex = 0;
		int32 HitNum = 0;
		int32 EndIndex = CurrentElementNum;
		bool FirstHit = false;
		bool ShouldMove = false;
		ElementType* Data = Begin();

		for (int32 Index = 0; Index <= EndIndex; Index++)
		{
			if (Index == EndIndex || Data[Index] == ToRemove)
			{
				if (ShouldMove)
				{
					HitIndex = Index - MoveNum - HitNum;
					Memory::DestructItem(Data + HitIndex, HitNum);

					Memory::Move((uint8*)Data + MoveIndex * sizeof(ElementType),
						(uint8*)Data + (HitIndex + HitNum) * sizeof(ElementType),
						MoveNum * sizeof(ElementType));

					CurrentElementNum -= HitNum;

					MoveIndex = MoveIndex + MoveNum;
					HitNum = 0;
					MoveNum = 0;
					ShouldMove = false;
					Index--;
				}
				else
				{
					if (!FirstHit)
					{
						MoveIndex = Index;
						FirstHit = true;
					}
					HitNum++;

					//this hitted one is the last one 
					if(Index == EndIndex - 1) ShouldMove = true;
				}
			}
			else if (HitNum)
			{
				ShouldMove = true;
				MoveNum++;
			}
		}

		if (AllowShrinking)
		{
			Shrink();
		}
	}


	void RemoveAt(const int32 StartIndex, const int32 RemoveNum = 1, bool AllowShrinking = true)
	{
		if (RemoveNum >= 0 && StartIndex >= 0)
		{
			CHECKF((RemoveNum <= CurrentElementNum - StartIndex), "Array index out of bounds, current: %i, index: %i, remove: %i", CurrentElementNum, StartIndex, RemoveNum);

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
		CHECK(CurrentElementNum >= 0);
		Resize(CurrentElementNum);
	}

	/*
	  If the current element slack satisfies the conditions in ShrinkPolicy(),
	  then resize the array to the best size 
        */
	void Shrink()
	{
		CHECK(MaxElementNum >= CurrentElementNum);
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
		ClearElements();
		Resize(0);
	}


	/**
	 * Resizes array to given number of elements.
	 *
	 */
	void SetNum(int32 NewNum, bool bAllowShrinking = true)
	{
		if (NewNum > CurrentElementNum)
		{
			const int32 Diff = NewNum - CurrentElementNum;
			const int32 Index = AddUninitialize(Diff);
			Memory::DefaultConstructItem((uint8*)Allocator.Address() + Index * sizeof(ElementType), Diff);
		}
		else if (NewNum < CurrentElementNum)
		{
			RemoveAt(NewNum, CurrentElementNum - NewNum, bAllowShrinking);
		}
	}


	/*
	Add Uninitialize element to the last
	Return the element num that calling AddUninitialize() before
	*/
	int32 AddUninitialize(int32 Num = 1)
	{
		CHECK(Num >= 0);

		int32 OldElementNum = CurrentElementNum;
		if ((CurrentElementNum += Num) > MaxElementNum)
		{
			int32 NewMaxElementNum = GrowPolicy(CurrentElementNum, MaxElementNum);
			Allocator.Resize(MaxElementNum, NewMaxElementNum);
			MaxElementNum = NewMaxElementNum;
		}
		return OldElementNum;
	}

	int32 AddZeroed(int32 Num = 1)
	{
		const int32 Index = AddUninitialize(Num);
		Memory::Zero((uint8*)Allocator.Address() + Index * sizeof(ElementType), Num * sizeof(ElementType));
		return Index;
	}

	/*
	  Insert Uninitialize element to the array
	*/
	void InsertUninitialize(int32 Index, int32 Num = 1)
	{
		CHECKF((Num >=0 && Index >= 0 && Index < CurrentElementNum), "Array index out of bounds, current: %i, index: %i", CurrentElementNum, Index);

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

	FORCE_INLINE bool IsValidIndex(int32 Index) const
	{
		return Index >= 0 && Index < CurrentElementNum;
	}



public:
	/* Iterators */
	typedef IndexedContainerIterator<Array, ElementType, int32> Iterator;
	typedef IndexedContainerIterator<const Array, const ElementType, int32> ConstIterator;

	/**
	 * Creates an iterator for the contents of this array
	 *
	 * @returns The iterator.
	 */
	Iterator CreateIterator()
	{
		return Iterator(*this);
	}

	/**
	 * Creates a const iterator for the contents of this array
	 *
	 * @returns The const iterator.
	 */
	ConstIterator CreateConstIterator() const
	{
		return ConstIterator(*this);
	}

	/**
	* Do not use directly
	* STL-like iterators to enable range-based for loop support.
	*/
	typedef CheckedPointerIterator<      ElementType, int32> RangedForIteratorType;
	typedef CheckedPointerIterator<const ElementType, int32> RangedForConstIteratorType;

	FORCEINLINE RangedForIteratorType      begin() { return RangedForIteratorType(CurrentElementNum, Begin()); }
	FORCEINLINE RangedForConstIteratorType begin() const { return RangedForConstIteratorType(CurrentElementNum, Begin()); }
	FORCEINLINE RangedForIteratorType      end() { return RangedForIteratorType(CurrentElementNum, Begin() + CurrentNum()); }
	FORCEINLINE RangedForConstIteratorType end() const { return RangedForConstIteratorType(CurrentElementNum, Begin() + CurrentNum()); }



private:
	/*
       Resize the allocation to NewMaxElementNum
	   Be careful if NewMaxElementNum < CurrentElementNum
    */
	FORCE_INLINE
	void Resize(int32 NewMaxElementNum)
	{
		CHECK(NewMaxElementNum >= 0);

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

	
	template<typename ArgsType>
	int32 AddUniqueInternal(ArgsType&& Args)
	{
		int32 Index = Find(Args);
		if (Index != -1)
			return Index;
		else
			return Add(std::forward<ArgsType>(Args));
	}





private:
	typedef AllocatorType AllocatorInstance;

	AllocatorInstance Allocator;
	int32 CurrentElementNum;
	int32 MaxElementNum;

};



//To add a new element to ArrayInstance -> "new(ArrayInstance) ElementType(Args)"
template <typename ElementType, typename Allocator> 
void* operator new(Size_T Size, Array<ElementType, Allocator>& Array)
{
	CHECK(Size == sizeof(ElementType));
	const int32 Index = Array.AddUninitialize(1);
	return &Array[Index];
}


template <typename ElementType, typename Allocator>
void* operator new(Size_T Size, Array<ElementType, Allocator>& Array, int32 Index)
{
	CHECK(Size == sizeof(ElementType));
	Array.InsertUninitialized(Index, 1);
	return &Array[Index];
}
