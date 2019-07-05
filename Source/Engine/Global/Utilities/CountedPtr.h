#pragma once


#include "Global/GlobalType.h"
#include "Global/Utilities/AtomicCounter.h"

/**
  Shared object used by shared smart pointer
 */
class SharedObject
{
public:
	SharedObject() :SharedCount(0) {}

	virtual ~SharedObject() {
		//check
	}

	FORCE_INLINE
	void AddRef() const
	{
		++SharedCount;
	}

	FORCE_INLINE
	void Release() const
	{
		--SharedCount;
		if (SharedCount == 0)
		{
			delete this;
		}
	}

	FORCE_INLINE
	uint32 GetCount() const
	{
		return SharedCount;
	}

private:
	mutable uint32 SharedCount;

};


/**
  Shared object for mutithread
 */
class ThreadSharedObject
{
public:
	ThreadSharedObject() {}

	virtual ~ThreadSharedObject()
	{
		//check
	}

	FORCE_INLINE
	void AddRef() const
	{
		Counter.Increment();
	}

	FORCE_INLINE
	void Release() const
	{
		int32 Val = Counter.Decrement();
		if (Val == 0)
		{
			delete this;
		}
	}

	FORCE_INLINE
	uint32 GetCount() const
	{
		return (uint32)Counter.GetCounter();
	}


private:
	mutable AtomicCounter Counter;

};



/**
  Shared Count pointer type

  Used for Com object liked structure
 */
template<typename PointerType>
class CountedPtr
{
public:
	FORCE_INLINE
	CountedPtr() :Pointer(nullptr) {}

	CountedPtr(PointerType* InPtr)
	{
		Pointer = InPtr;
		if (Pointer)
		{
			Pointer->AddRef();
		}
	}

	CountedPtr(const CountedPtr& Copy)
	{
		Pointer = Copy.Pointer;
		if (Pointer)
		{
			Pointer->AddRef();
		}
	}

	FORCE_INLINE
	CountedPtr(CountedPtr&& Copy)
	{
		Pointer = Copy.Pointer;
		Copy.Pointer = nullptr;
	}

	~CountedPtr()
	{
		if (Pointer)
		{
			Pointer->Release();
		}
	}


	CountedPtr& operator=(PointerType* InPtr)
	{
		PointerType* OldPtr = Pointer;
		Pointer = InPtr;
		if (Pointer)
		{
			Pointer->AddRef();
		}

		if (OldPtr)
		{
			OldPtr->Release();
		}

		return *this;
	}

	FORCE_INLINE
	CountedPtr& operator=(const CountedPtr& Other)
	{
		//just pass the pointer,
		//do not need to check if the come-in object is same as this
		return *this = Other.Pointer;
	}

	CountedPtr& operator=(CountedPtr&& Other)
	{
		//see if the come-in referenced object is same as this
		if (this != &Other)
		{
			PointerType* OldPtr = Pointer;
			Pointer = Other.Pointer;

			//the come-in reference is no more needed
			Other.Pointer = nullptr;

			if (OldPtr)
			{
				OldPtr->Release();
			}
		}

		return *this;
	}

	FORCE_INLINE
	PointerType* operator->() const { return Pointer; }

	FORCE_INLINE
	operator PointerType*() const { return Pointer; }

	FORCE_INLINE
	PointerType* Get() const { return Pointer; }

	FORCE_INLINE
	uint32 GetCount()
	{
		uint32 Count = 0;
		if (Pointer)
		{
			Count = Pointer->GetCount();
			//check
		}

		return Count;
	}



private:
	PointerType* Pointer;
};



template<typename PointerType>
FORCE_INLINE
bool operator==(const CountedPtr<PointerType>& Left, const CountedPtr<PointerType>& Right)
{
	return Left.Get() == Right.Get();
}


template<typename PointerType>
FORCE_INLINE
bool operator==(const CountedPtr<PointerType>& Left, PointerType* RightPtr)
{
	return Left.Get() == RightPtr;
}


template<typename PointerType>
FORCE_INLINE
bool operator==(PointerType* LeftPtr, const CountedPtr<PointerType>& RightPtr)
{
	return LeftPtr == RightPtr.Get();
}

