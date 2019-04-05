/**********************
  Atomic Counter

***********************/
#pragma once

#include "Global/GlobalType.h"
#include "HAL/Platform.h"


/*
  Atomic Counter
  Int32
*/
class AtomicCounter
{
public:
	AtomicCounter(int32 InitValue = 0): Counter(InitValue) {}
	AtomicCounter(const AtomicCounter& other)
	{
		Counter = other.GetCounter();
	}

	~AtomicCounter() {}


	int32 GetCounter() const
	{
		return PlatformAtomics::InterlockedRead(&const_cast<AtomicCounter*>(this)->Counter);
	}


	void SetCounter(int32 Val)
	{
		PlatformAtomics::InterlockedStore(&Counter, Val);
	}


	void ResetCounter()
	{
		PlatformAtomics::InterlockedStore(&Counter, 0);
	}


	int32 Increment()
	{
		return PlatformAtomics::InterlockedIncrement(&Counter);
	}


	int32 Add(int32 AddValue)
	{
		return PlatformAtomics::InterlockedAdd(&Counter, AddValue);
	}


	int32 Decrement()
	{
		return PlatformAtomics::InterlockedDecrement(&Counter);
	}


	int32 Sub(int32 SubValue)
	{
		return PlatformAtomics::InterlockedAdd(&Counter, -SubValue);
	}

	AtomicCounter& operator=(const AtomicCounter& Other) = delete;


protected:
	volatile int32 Counter;
};



