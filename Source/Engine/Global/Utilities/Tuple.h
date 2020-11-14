/****************************************
Tuple


*****************************************/
#pragma once

#include "Global/GlobalType.h"


template <typename... Types>
struct Tuple;

template <typename T, typename... Types>
struct DecayedFrontOfParameterPackIsSameType
{
	enum { Value = IsTypeEqual<T, typename Decayed<typename GetNthTypeFromParameter<0, Types...>::Type>::Type>::Value };
};


template <typename T, uint32 Index>
struct TupleElement
{
	template <
		typename... ArgTypes
	>
		explicit TupleElement(ArgTypes&&... Args)
		: Value(Forward<ArgTypes>(Args)...)
	{
	}

	TupleElement()
		: Value()
	{
	}

	TupleElement(TupleElement&&) = default;
	TupleElement(const TupleElement&) = default;
	TupleElement& operator=(TupleElement&&) = default;
	TupleElement& operator=(const TupleElement&) = default;

	T Value;
};



template <uint32 IterIndex, uint32 Index, typename... Types>
struct TupleElementHelperImpl;

template <uint32 IterIndex, uint32 Index, typename ElementType, typename... Types>
struct TupleElementHelperImpl<IterIndex, Index, ElementType, Types...> : TupleElementHelperImpl<IterIndex + 1, Index, Types...>
{
};

template <uint32 Index, typename ElementType, typename... Types>
struct TupleElementHelperImpl<Index, Index, ElementType, Types...>
{
	typedef ElementType Type;

	template <typename TupleType>
	static FORCE_INLINE ElementType& Get(TupleType& Tuple)
	{
		return static_cast<TupleElement<ElementType, Index>&>(Tuple).Value;
	}

	template <typename TupleType>
	static FORCE_INLINE const ElementType& Get(const TupleType& Tuple)
	{
		return Get((TupleType&)Tuple);
	}
};

template <uint32 WantedIndex, typename... Types>
struct TupleElementHelper : TupleElementHelperImpl<0, WantedIndex, Types...>
{
};


template <uint32 ArgCount, uint32 ArgToCompare>
struct EqualityHelper
{
	template <typename TupleType>
	FORCE_INLINE static bool Compare(const TupleType& Lhs, const TupleType& Rhs)
	{
		return Lhs.template Get<ArgToCompare>() == Rhs.template Get<ArgToCompare>() && EqualityHelper<ArgCount, ArgToCompare + 1>::Compare(Lhs, Rhs);
	}
};

template <uint32 ArgCount>
struct EqualityHelper<ArgCount, ArgCount>
{
	template <typename TupleType>
	FORCE_INLINE static bool Compare(const TupleType& Lhs, const TupleType& Rhs)
	{
		return true;
	}
};


template <uint32 NumArgs, uint32 ArgToCompare = 0, bool Last = ArgToCompare + 1 == NumArgs>
struct LessThanHelper
{
	template <typename TupleType>
	FORCE_INLINE static bool Do(const TupleType& Lhs, const TupleType& Rhs)
	{
		return Lhs.template Get<ArgToCompare>() < Rhs.template Get<ArgToCompare>() || (!(Rhs.template Get<ArgToCompare>() < Lhs.template Get<ArgToCompare>()) && LessThanHelper<NumArgs, ArgToCompare + 1>::Do(Lhs, Rhs));
	}
};

template <uint32 NumArgs, uint32 ArgToCompare>
struct LessThanHelper<NumArgs, ArgToCompare, true>
{
	template <typename TupleType>
	FORCE_INLINE static bool Do(const TupleType& Lhs, const TupleType& Rhs)
	{
		return Lhs.template Get<ArgToCompare>() < Rhs.template Get<ArgToCompare>();
	}
};

template <uint32 NumArgs>
struct LessThanHelper<NumArgs, NumArgs, false>
{
	template <typename TupleType>
	FORCE_INLINE static bool Do(const TupleType& Lhs, const TupleType& Rhs)
	{
		return false;
	}
};



template <typename Indices, typename... Types>
struct TupleStorage;

template <uint32... Indices, typename... Types>
struct TupleStorage<IntegerSequence<uint32, Indices...>, Types...> : TupleElement<Types, Indices>...
{
	template <
		typename... ArgTypes
	>
		explicit TupleStorage(ArgTypes&&... Args)
		: TupleElement<Types, Indices>(Forward<ArgTypes>(Args))...
	{
	}

	TupleStorage() = default;
	TupleStorage(TupleStorage&&) = default;
	TupleStorage(const TupleStorage&) = default;
	TupleStorage& operator=(TupleStorage&&) = default;
	TupleStorage& operator=(const TupleStorage&) = default;

	template <uint32 Index> FORCE_INLINE const typename TupleElementHelper<Index, Types...>::Type& Get() const { return TupleElementHelper<Index, Types...>::Get(*this); }
	template <uint32 Index> FORCE_INLINE  typename TupleElementHelper<Index, Types...>::Type& Get() { return TupleElementHelper<Index, Types...>::Get(*this); }
};


template <typename InKeyType, typename InValueType>
struct TupleStorage<IntegerSequence<uint32, 0, 1>, InKeyType, InValueType>
{
private:
	template <uint32 Index, typename Dummy> // Dummy needed for partial template specialization workaround
	struct GetHelper;

	template <typename Dummy>
	struct GetHelper<0, Dummy>
	{
		typedef InKeyType ResultType;

		static const InKeyType& Get(const TupleStorage& Tuple) { return Tuple.Key; }
		static       InKeyType& Get(TupleStorage& Tuple) { return Tuple.Key; }
	};

	template <typename Dummy>
	struct GetHelper<1, Dummy>
	{
		typedef InValueType ResultType;

		static const InValueType& Get(const TupleStorage& Tuple) { return Tuple.Value; }
		static       InValueType& Get(TupleStorage& Tuple) { return Tuple.Value; }
	};

public:
	typedef InKeyType   KeyType;
	typedef InValueType ValueType;

	template <typename KeyInitType, typename ValueInitType>
	explicit TupleStorage(KeyInitType&& KeyInit, ValueInitType&& ValueInit)
		: Key(Forward<KeyInitType  >(KeyInit))
		, Value(Forward<ValueInitType>(ValueInit))
	{
	}

	TupleStorage()
		: Key()
		, Value()
	{
	}

	TupleStorage(TupleStorage&&) = default;
	TupleStorage(const TupleStorage&) = default;
	TupleStorage& operator=(TupleStorage&&) = default;
	TupleStorage& operator=(const TupleStorage&) = default;

	template <uint32 Index> FORCE_INLINE const typename GetHelper<Index, void>::ResultType& Get() const { return GetHelper<Index, void>::Get(*this); }
	template <uint32 Index> FORCE_INLINE       typename GetHelper<Index, void>::ResultType& Get() { return GetHelper<Index, void>::Get(*this); }

	InKeyType   Key;
	InValueType Value;
};


template <typename Indices, typename... Types>
struct TupleImpl;

template <uint32... Indices, typename... Types>
struct TupleImpl<IntegerSequence<uint32, Indices...>, Types...> : TupleStorage<IntegerSequence<uint32, Indices...>, Types...>
{
private:
	typedef TupleStorage<IntegerSequence<uint32, Indices...>, Types...> Super;

public:
	using Super::Get;

	template <
		typename... ArgTypes
	>
		explicit TupleImpl(ArgTypes&&... Args)
		: Super(Forward<ArgTypes>(Args)...)
	{
	}

	TupleImpl() = default;
	TupleImpl(TupleImpl&& Other) = default;
	TupleImpl(const TupleImpl& Other) = default;
	TupleImpl& operator=(TupleImpl&& Other) = default;
	TupleImpl& operator=(const TupleImpl& Other) = default;

	template <typename FuncType, typename... ArgTypes>
	decltype(auto) ApplyAfter(FuncType&& Func, ArgTypes&&... Args) const
	{
		return Func(Forward<ArgTypes>(Args)..., this->template Get<Indices>()...);
	}

	template <typename FuncType, typename... ArgTypes>
	decltype(auto) ApplyBefore(FuncType&& Func, ArgTypes&&... Args) const
	{
		return Func(this->template Get<Indices>()..., Forward<ArgTypes>(Args)...);
	}


	FORCE_INLINE friend bool operator==(const TupleImpl& Lhs, const TupleImpl& Rhs)
	{
		// This could be implemented with a fold expression when our compilers support it
		return EqualityHelper<sizeof...(Types), 0>::Compare(Lhs, Rhs);
	}

	FORCE_INLINE friend bool operator!=(const TupleImpl& Lhs, const TupleImpl& Rhs)
	{
		return !(Lhs == Rhs);
	}

	FORCE_INLINE friend bool operator<(const TupleImpl& Lhs, const TupleImpl& Rhs)
	{
		return LessThanHelper<sizeof...(Types)>::Do(Lhs, Rhs);
	}

	FORCE_INLINE friend bool operator<=(const TupleImpl& Lhs, const TupleImpl& Rhs)
	{
		return !(Rhs < Lhs);
	}

	FORCE_INLINE friend bool operator>(const TupleImpl& Lhs, const TupleImpl& Rhs)
	{
		return Rhs < Lhs;
	}

	FORCE_INLINE friend bool operator>=(const TupleImpl& Lhs, const TupleImpl& Rhs)
	{
		return !(Lhs < Rhs);
	}
};


template <typename... Types>
FORCE_INLINE Tuple<typename Decayed<Types>::Type...> MakeTupleImpl(Types&&... Args)
{
	return Tuple<typename Decayed<Types>::Type...>(Forward<Types>(Args)...);
}


template <typename TIntegerSequence>
struct TransformTuple_Impl;

template <uint32... Indices>
struct TransformTuple_Impl<IntegerSequence<uint32, Indices...>>
{
	template <typename TupleType, typename FuncType>
	static decltype(auto) Do(TupleType&& Tuple, FuncType Func)
	{
		return MakeTupleImpl(Func(Forward<TupleType>(Tuple).template Get<Indices>())...);
	}
};



template <typename... Types>
struct Tuple : TupleImpl<MakeIntegerSequence<uint32, sizeof...(Types)>, Types...>
{
private:
	typedef TupleImpl<MakeIntegerSequence<uint32, sizeof...(Types)>, Types...> Super;

public:
	template <
		typename... ArgTypes
	>
		explicit Tuple(ArgTypes&&... Args)
		: Super(Forward<ArgTypes>(Args)...)
	{
		// This constructor is disabled for TTuple and zero parameters because VC is incorrectly instantiating it as a move/copy/default constructor.
	}

	Tuple() = default;
	Tuple(Tuple&&) = default;
	Tuple(const Tuple&) = default;
	Tuple& operator=(Tuple&&) = default;
	Tuple& operator=(const Tuple&) = default;
};



/**
 * Example:
 *
 * void Func(const int32 A, FString&& B)
 * {
 *     // Equivalent to:
 *     // Tuple<int32, const TCHAR*, FString> MyTuple(A, TEXT("Hello"), MoveTemp(B));
 *     auto MyTuple = MakeTuple(A, TEXT("Hello"), MoveTemp(B));
 * }
 */
template <typename... Types>
FORCE_INLINE Tuple<typename Decayed<Types>::Type...> MakeTuple(Types&&... Args)
{
	return MakeTupleImpl(Forward<Types>(Args)...);
}



/**
 *
 * Example:
 *
 * float        Overloaded(int32 Arg);
 * char         Overloaded(const TCHAR* Arg);
 * const TCHAR* Overloaded(const FString& Arg);
 *
 * void Func(const Tuple<int32, const TCHAR*, FString>& MyTuple)
 * {
 *     // Equivalent to:
 *     // TTuple<float, char, const TCHAR*> TransformedTuple(Overloaded(MyTuple.Get<0>()), Overloaded(MyTuple.Get<1>()), Overloaded(MyTuple.Get<2>())));
 *     auto TransformedTuple = TransformTuple(MyTuple, [](const auto& Arg) { return Overloaded(Arg); });
 * }
 */
template <typename FuncType, typename... Types>
FORCE_INLINE decltype(auto) TransformTuple(Tuple<Types...>&& Tuple, FuncType Func)
{
	return TransformTuple_Impl<MakeIntegerSequence<uint32, sizeof...(Types)>>::Do(MoveTemp(Tuple), MoveTemp(Func));
}

template <typename FuncType, typename... Types>
FORCE_INLINE decltype(auto) TransformTuple(const Tuple<Types...>& Tuple, FuncType Func)
{
	return TransformTuple_Impl<MakeIntegerSequence<uint32, sizeof...(Types)>>::Do(Tuple, MoveTemp(Func));
}



