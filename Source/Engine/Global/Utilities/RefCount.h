#pragma once


#include "Global/GlobalType.h"
#include "Global/Utilities/AtomicCounter.h"




/**
  Reference counting object
 */
class RefCountObject
{
public:
	RefCountObject(): Counter(int32(0)) {}
	virtual ~RefCountObject() {}

	RefCountObject(const RefCountObject& other) = delete;
	RefCountObject& operator=(const RefCountObject& other) = delete;

	inline int32 AddRef() const
	{
		return Counter.Increment();
	}

	inline int32 Release() const
	{
		int32 CurrentRef = Counter.Decrement();
		if (CurrentRef <= 0)
		{
			delete this;
		}

		return CurrentRef;
	}

	inline int32 GetCount() const
	{
		return Counter.GetCounter();
	}


private:
	mutable AtomicCounter<int32> Counter;

};



/**
  Pointer type for reference counting object 

  Used for Com object liked structure
 */
template<typename RefObjectType>
class RefCountObjectPtr
{
public:
	FORCE_INLINE
	RefCountObjectPtr() :Pointer(nullptr) {}

	RefCountObjectPtr(RefObjectType* InPtr)
	{
		Pointer = InPtr;
		if (Pointer)
		{
			Pointer->AddRef();
		}
	}

	RefCountObjectPtr(const RefCountObjectPtr& Copy)
	{
		Pointer = Copy.Pointer;
		if (Pointer)
		{
			Pointer->AddRef();
		}
	}

	FORCE_INLINE
	RefCountObjectPtr(RefCountObjectPtr&& Copy)
	{
		Pointer = Copy.Pointer;
		Copy.Pointer = nullptr;
	}

	~RefCountObjectPtr(()
	{
		if (Pointer)
		{
			Pointer->Release();
		}
	}


	RefCountObjectPtr(& operator=(RefObjectType* InPtr)
	{
		RefObjectType* OldPtr = Pointer;
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
	RefCountObjectPtr& operator=(const RefCountObjectPtr& Other)
	{
		//just pass the pointer,
		//do not need to check if the come-in object is same as this
		return *this = Other.Pointer;
	}

	RefCountObjectPtr& operator=(RefCountObjectPtr&& Other)
	{
		//see if the come-in referenced object is same as this
		if (this != &Other)
		{
			RefObjectType* OldPtr = Pointer;
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
	RefObjectType* operator->() const { return Pointer; }

	FORCE_INLINE
	operator RefObjectType() const { return Pointer; }

	FORCE_INLINE
	RefObjectType* Get() const { return Pointer; }

	FORCE_INLINE
	uint32 GetCount()
	{
		uint32 Count = 0;
		if (Pointer)
		{
			Count = Pointer->GetCount();
		}

		return Count;
	}

	FORCE_INLINE
	bool Valid() const
	{
		return Pointer != nullptr;
	}

	FORCE_INLINE
	explicit operator bool() const
	{
		return Pointer != nullptr;
	}

	FORCE_INLINE 
	void SafeRelease()
	{
		*this = nullptr;
	}

private:
	RefObjectType* Pointer;
};



template<typename RefObjectType>
FORCE_INLINE
bool operator==(const RefCountObjectPtr<RefObjectType>& Left, const RefCountObjectPtr<RefObjectType>& Right)
{
	return Left.Get() == Right.Get();
}


template<typename RefObjectType>
FORCE_INLINE
bool operator==(const RefCountObjectPtr<RefObjectType>& Left, RefObjectType* RightPtr)
{
	return Left.Get() == RightPtr;
}


template<typename RefObjectType>
FORCE_INLINE
bool operator==(RefObjectType* LeftPtr, const RefCountObjectPtr<RefObjectType>& RightPtr)
{
	return LeftPtr == RightPtr.Get();
}

