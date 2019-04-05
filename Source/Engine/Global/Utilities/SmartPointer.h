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
	virtual void AddRef() const
	{
		++SharedCount;
	}

	FORCE_INLINE
	virtual void Release() const
	{
		--SharedCount;
		if (SharedCount == 0)
		{
			delete this;
		}
	}

	FORCE_INLINE
	virtual uint32 GetCount() const
	{
		return SharedCount;
	}

protected:
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
	virtual void AddRef() const
	{
		Counter.Increment();
	}

	FORCE_INLINE
	virtual void Release() const
	{
		int32 Val = Counter.Decrement();
		if (Val == 0)
		{
			delete this;
		}
	}

	FORCE_INLINE
	virtual uint32 GetCount() const
	{
		return (uint32)Counter.GetCounter();
	}


protected:
	mutable AtomicCounter Counter;

};



/**
  Shared smart pointer type
 */
template<typename PointerType>
class SmartPtr
{
public:
	FORCE_INLINE
	SmartPtr() :Pointer(nullptr) {}

	SmartPtr(PointerType* InPtr)
	{
		Pointer = InPtr;
		if (Pointer)
		{
			Pointer->AddRef();
		}
	}

	SmartPtr(const SmartPtr& Copy)
	{
		Pointer = Copy.Pointer;
		if (Pointer)
		{
			Pointer->AddRef();
		}
	}

	FORCE_INLINE
	SmartPtr(SmartPtr&& Copy)
	{
		Pointer = Copy.Pointer;
		Copy.Pointer = nullptr;
	}

	~SmartPtr()
	{
		if (Pointer)
		{
			Pointer->Release();
		}
	}


	SmartPtr& operator=(PointerType* InPtr)
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
	SmartPtr& operator=(const SmartPtr& Other)
	{
		//just pass the pointer,
		//do not need to check if the come-in object is same as this
		return *this = Other.Pointer;
	}

	SmartPtr& operator=(SmartPtr&& Other)
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



protected:
	PointerType* Pointer;
};



template<typename PointerType>
FORCE_INLINE
bool operator==(const SmartPtr<PointerType>& Left, const SmartPtr<PointerType>& Right)
{
	return Left.Get() == Right.Get();
}


template<typename PointerType>
FORCE_INLINE
bool operator==(const SmartPtr<PointerType>& Left, PointerType* RightPtr)
{
	return Left.Get() == RightPtr;
}


template<typename PointerType>
FORCE_INLINE
bool operator==(PointerType* LeftPtr, const SmartPtr<PointerType>& RightPtr)
{
	return LeftPtr == RightPtr.Get();
}

