/****************************************
Tuple


*****************************************/
#pragma once

#include "Global/GlobalType.h"


template <typename T, typename... Types>
struct DecayedFrontOfParameterPackIsSameType
{
	enum { Value = IsTypeEqual<T, typename DecayedType<typename GetNthTypeFromParameter<0, Types...>::Type>::Type>::Value };
};

template <typename T, uint32 Index>
struct TupleElement
{
	template <
		typename... ArgTypes,
		typename = typename FuncTrigger <
		AndTest<
		sizeof...(ArgTypes) != 0,
		OrTest<
		sizeof...(ArgTypes) != 1,
		NotTest<DecayedFrontOfParameterPackIsSameType<TupleElement, ArgTypes...>>
		>
		>::Value
		>::Type
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



template <typename Indices, typename... Types>
struct TupleStorage;

template <uint32... Indices, typename... Types>
struct TupleStorage<IntegerSequence<uint32, Indices...>, Types...> : TupleElement<Types, Indices>...
{
	template <
		typename... ArgTypes,
		typename = typename FuncTrigger <
		AndTest<
		sizeof...(ArgTypes) == sizeof...(Types) && sizeof...(ArgTypes) != 0,
		OrTest<
		sizeof...(ArgTypes) != 1,
		NotTest<DecayedFrontOfParameterPackIsSameType<TupleStorage, ArgTypes...>>
		>
		>::Value
		>::Type
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
	template <uint32 Index> FORCE_INLINE typename TupleElementHelper<Index, Types...>::Type& Get() { return TupleElementHelper<Index, Types...>::Get(*this); }
};


template <typename InKeyType, typename InValueType>
struct TupleStorage<IntegerSequence<uint32, 0, 1>, InKeyType, InValueType>
{
private:
	// Dummy needed for partial template specialization workaround
	template <uint32 Index, typename Dummy> 
	struct GetHelper;

	template <typename Dummy>
	struct GetHelper<0, Dummy>
	{
		typedef InKeyType ResultType;

		static const InKeyType& Get(const TupleStorage& Tuple) { return Tuple.Key; }
		static InKeyType& Get(TupleStorage& Tuple) { return Tuple.Key; }
	};

	template <typename Dummy>
	struct GetHelper<1, Dummy>
	{
		typedef InValueType ResultType;

		static const InValueType& Get(const TupleStorage& Tuple) { return Tuple.Value; }
		static InValueType& Get(TupleStorage& Tuple) { return Tuple.Value; }
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
	template <uint32 Index> FORCE_INLINE typename GetHelper<Index, void>::ResultType& Get() { return GetHelper<Index, void>::Get(*this); }

	InKeyType   Key;
	InValueType Value;
};






template <uint32 ArgCount, uint32 ArgToCompare>
struct EqualHelper
{
	template <typename TupleType>
	FORCEINLINE static bool Compare(const TupleType& Lhs, const TupleType& Rhs)
	{
		return Lhs.template Get<ArgToCompare>() == Rhs.template Get<ArgToCompare>() && EqualHelper<ArgCount, ArgToCompare + 1>::Compare(Lhs, Rhs);
	}
};

template <uint32 ArgCount>
struct EqualHelper<ArgCount, ArgCount>
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
	FORCEINLINE static bool Do(const TupleType& Lhs, const TupleType& Rhs)
	{
		return Lhs.template Get<ArgToCompare>() < Rhs.template Get<ArgToCompare>() || (!(Rhs.template Get<ArgToCompare>() < Lhs.template Get<ArgToCompare>()) && LessThanHelper<NumArgs, ArgToCompare + 1>::Do(Lhs, Rhs));
	}
};

template <uint32 NumArgs, uint32 ArgToCompare>
struct LessThanHelper<NumArgs, ArgToCompare, true>
{
	template <typename TupleType>
	FORCEINLINE static bool Do(const TupleType& Lhs, const TupleType& Rhs)
	{
		return Lhs.template Get<ArgToCompare>() < Rhs.template Get<ArgToCompare>();
	}
};

template <uint32 NumArgs>
struct LessThanHelper<NumArgs, NumArgs, false>
{
	template <typename TupleType>
	FORCEINLINE static bool Do(const TupleType& Lhs, const TupleType& Rhs)
	{
		return false;
	}
};





template <typename Indices, typename... Types>
struct TupleImplement;

template <uint32... Indices, typename... Types>
struct TupleImplement<IntegerSequence<uint32, Indices...>, Types...> : TupleStorage<IntegerSequence<uint32, Indices...>, Types...>
{
private:
	typedef TupleStorage<IntegerSequence<uint32, Indices...>, Types...> Super;

public:
	using Super::Get;

	template <
		typename... ArgTypes,
		typename = typename FuncTrigger<
		AndTest<
		sizeof...(ArgTypes) == sizeof...(Types) && sizeof...(ArgTypes) != 0,
		OrTest<
		sizeof...(ArgTypes) != 1,
		NotTest<DecayedFrontOfParameterPackIsSameType<TupleImplement, ArgTypes...>>
		>
		>::Value
		>::Type
	>
	explicit TupleImplement(ArgTypes&&... Args)
		: Super(Forward<ArgTypes>(Args)...)
	{
	}

	TupleImplement() = default;
	TupleImplement(TupleImplement&& Other) = default;
	TupleImplement(const TupleImplement& Other) = default;
	TupleImplement& operator=(TupleImplement&& Other) = default;
	TupleImplement& operator=(const TupleImplement& Other) = default;

	FORCE_INLINE friend bool operator==(const TupleImplement& Lhs, const TupleImplement& Rhs)
	{
		return EqualHelper<sizeof...(Types), 0>::Compare(Lhs, Rhs);
	}

	FORCE_INLINE friend bool operator!=(const TupleImplement& Lhs, const TupleImplement& Rhs)
	{
		return !(Lhs == Rhs);
	}

	FORCE_INLINE friend bool operator<(const TupleImplement& Lhs, const TupleImplement& Rhs)
	{
		return LessThanHelper<sizeof...(Types)>::Do(Lhs, Rhs);
	}

	FORCE_INLINE friend bool operator<=(const TupleImplement& Lhs, const TupleImplement& Rhs)
	{
		return !(Rhs < Lhs);
	}

	FORCE_INLINE friend bool operator>(const TupleImplement& Lhs, const TupleImplement& Rhs)
	{
		return Rhs < Lhs;
	}

	FORCE_INLINE friend bool operator>=(const TupleImplement& Lhs, const TupleImplement& Rhs)
	{
		return !(Lhs < Rhs);
	}
};




template <typename... Types>
struct Tuple : TupleImplement<MakeIntegerSequence<uint32, sizeof...(Types)>, Types...>
{
private:
	typedef TupleImplement<MakeIntegerSequence<uint32, sizeof...(Types)>, Types...> Super;

public:
	template <
		typename... ArgTypes,
		typename = typename FuncTrigger<
		AndTest<
		sizeof...(ArgTypes) == sizeof...(Types) && sizeof...(ArgTypes) != 0,
		OrTest<
		sizeof...(ArgTypes) != 1,
		NotTest<DecayedFrontOfParameterPackIsSameType<Tuple, ArgTypes...>>
		>
		>::Value
		>::Type
	>
		explicit Tuple(ArgTypes&&... Args)
		: Super(Forward<ArgTypes>(Args)...)
	{
		// This constructor is disabled for Tuple and zero parameters because VC is incorrectly instantiating it as a move/copy/default constructor.
	}

	Tuple() = default;
	Tuple(Tuple&&) = default;
	Tuple(const Tuple&) = default;
	Tuple& operator=(Tuple&&) = default;
	Tuple& operator=(const Tuple&) = default;
};


template <typename... Types>
FORCE_INLINE Tuple<typename DecayedType<Types>::Type...> MakeTupleImpl(Types&&... Args)
{
	return Tuple<typename DecayedType<Types>::Type...>(Forward<Types>(Args)...);
}

template <typename IntegerSequence>
struct TransformTupleImplement;

template <uint32... Indices>
struct TransformTupleImplement<IntegerSequence<uint32, Indices...>>
{
	template <typename TupleType, typename FuncType>
	static decltype(auto) Do(TupleType&& Tuple, FuncType Func)
	{
		return MakeTupleImpl(Func(Forward<TupleType>(Tuple).template Get<Indices>())...);
	}
};


/**
 * Makes a Tuple from arguments.  The type of the Tuple elements are the decayed versions of the arguments.
 *
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
FORCEINLINE Tuple<typename DecayedType<Types>::Type...> MakeTuple(Types&&... Args)
{
	return MakeTupleImpl(Forward<Types>(Args)...);
}


/**
 * Creates a new TTuple by applying a functor to each of the elements.
 *
 *
 * Example:
 *
 * float        Overloaded(int32 Arg);
 * char         Overloaded(const TCHAR* Arg);
 * const TCHAR* Overloaded(const FString& Arg);
 *
 * void Func(const TTuple<int32, const TCHAR*, FString>& MyTuple)
 * {
 *     // Equivalent to:
 *     // TTuple<float, char, const TCHAR*> TransformedTuple(Overloaded(MyTuple.Get<0>()), Overloaded(MyTuple.Get<1>()), Overloaded(MyTuple.Get<2>())));
 *     auto TransformedTuple = TransformTuple(MyTuple, [](const auto& Arg) { return Overloaded(Arg); });
 * }
 */
template <typename FuncType, typename... Types>
FORCE_INLINE decltype(auto) TransformTuple(Tuple<Types...>&& Tuple, FuncType Func)
{
	return TransformTupleImplement<MakeIntegerSequence<uint32, sizeof...(Types)>>::Do(MoveTemp(Tuple), MoveTemp(Func));
}

template <typename FuncType, typename... Types>
FORCE_INLINE decltype(auto) TransformTuple(const Tuple<Types...>& Tuple, FuncType Func)
{
	return TransformTupleImplement<MakeIntegerSequence<uint32, sizeof...(Types)>>::Do(Tuple, MoveTemp(Func));
}
