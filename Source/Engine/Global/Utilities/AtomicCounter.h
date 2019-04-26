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
template <typename Type>
class AtomicCounter
{
	static_assert(IsIntegralType<Type>::Value, "None integral type is not allowed");

public:
	AtomicCounter(Type InitValue = 0): Counter(InitValue) {}
	AtomicCounter(const AtomicCounter& other)
	{
		Counter = other.GetCounter();
	}

	~AtomicCounter() {}

	AtomicCounter(AtomicCounter&&) = delete;
	AtomicCounter& operator=(const AtomicCounter& Other) = delete;
	AtomicCounter& operator=(AtomicCounter&&) = delete;

	Type operator=(Type Val)
	{
		PlatformAtomics::InterlockedExchange(&Counter, Val);
		return GetCounter();
	}

	Type GetCounter() const
	{
		return PlatformAtomics::InterlockedRead(&const_cast<AtomicCounter*>(this)->Counter);
	}


	void SetCounter(Type Val)
	{
		PlatformAtomics::InterlockedStore(&Counter, Val);
	}


	void ResetCounter()
	{
		PlatformAtomics::InterlockedStore(&Counter, 0);
	}


	Type Increment()
	{
		return PlatformAtomics::InterlockedIncrement(&Counter);
	}


	Type Add(Type AddValue)
	{
		return PlatformAtomics::InterlockedAdd(&Counter, AddValue);
	}


	Type Decrement()
	{
		return PlatformAtomics::InterlockedDecrement(&Counter);
	}


	Type Sub(Type SubValue)
	{
		return PlatformAtomics::InterlockedAdd(&Counter, -SubValue);
	}




protected:
	volatile Type Counter;
};



