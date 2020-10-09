#pragma once

#include <stdio.h>
#include <type_traits>
#include "HAL/Platform.h"


//==============================
//Macros 
//==============================
#define CONCATENATE_INNER(x, y) x##y
#define CONCATENATE(x, y) CONCATENATE_INNER(x, y)
#define CONCATENATE_TRI(x, y, z) CONCATENATE(CONCATENATE_INNER(x, y), z)

//x to char type, len(x)<5
#define __CHAR(x)   #@x

//x to string, x will not be expanded if x is macro
#define __S(x)   #x

//x to string, x will be expanded if x is macro
#define _S(x)   __S(x)

//Concatenate x and y to a string
#define CAT_TO_STR(x, y) _S(x) "" _S(y)



//==============================
//Data types
//==============================

typedef PlatformTypes::int8	int8;			//8-bit  signed.
typedef PlatformTypes::uint8 	uint8;		//8-bit  unsigned.

typedef PlatformTypes::int16	int16;		//16-bit signed.
typedef PlatformTypes::uint16	uint16;		//16-bit unsigned.

typedef PlatformTypes::int32 	int32;		//32-bit signed.
typedef PlatformTypes::uint32	uint32;		//32-bit unsigned.

typedef PlatformTypes::int64	int64;		//64-bit signed.
typedef PlatformTypes::uint64	uint64;		//64-bit unsigned.

typedef PlatformTypes::Size_T   Size_T;		//unsigned int, has same size as pointer 

typedef PlatformTypes::ANSICHAR ANSICHAR;   //8-bit ANSI character
typedef PlatformTypes::WIDECHAR WIDECHAR;   //wide character, for UNICODE
typedef PlatformTypes::CHAR8     CHAR8;	   //UTF-8, for UNICODE
typedef PlatformTypes::CHAR16   CHAR16;     //UTF-16, for UNICODE
typedef PlatformTypes::CHAR32   CHAR32;     //UTF-32, for UNICODE

#if   PLATFORM_TEXT_IS_CHAR16
typedef CHAR16                  TChar;      //basic char type used in string
#elif PLATFORM_TEXT_IS_WCHAR
typedef WIDECHAR                TChar;      //basic char type used in string
#endif




/*
Type test

"sizeof(uint8) == 1" -> ensure bit size

"int16(uint8(-1)) == int16(0xFF)" -> cast it to larger bit type then compare(unsigned type convert to larger signed type), ensure the highest bit is not a sign bit

"int16(int8(-1)) == int16(-1)" -> cast it to larger bit type then compare(signed type convert to larger signed type), ensure the highest bit is a sign bit

"uint64(-1) > uint64(0)" -> no more larger type, so just compare 
*/
static_assert(sizeof(uint64) == 8, "uint64 must be 8 byte");
static_assert(uint64(-1) > uint64(0), "uint64 sign uncorrect");

static_assert(sizeof(uint32) == 4, "uint32 must be 4 byte");
static_assert(int64(uint32(-1)) == int64(0xFFFFFFFF), "uint32 sign uncorrect");

static_assert(sizeof(uint16) == 2, "uint16 must be 2 byte");
static_assert(int32(uint16(-1)) == int32(0xFFFF), "uint16 sign uncorrect");

static_assert(sizeof(uint8) == 1, "uint8 must be 1 byte");
static_assert(int16(uint8(-1)) == int16(0xFF), "uint8 sign uncorrect");


static_assert(sizeof(int64) == 8, "int64 must be 8 byte");
static_assert(int64(-1) < int64(0), "int64 sign uncorrect");

static_assert(sizeof(int32) == 4, "uint32 must be 4 byte");
static_assert(int64(int32(-1)) == int64(-1), "uint32 sign uncorrect");

static_assert(sizeof(uint16) == 2, "uint16 must be 2 byte");
static_assert(int32(int16(-1)) == int32(-1), "uint16 sign uncorrect");

static_assert(sizeof(int8) == 1, "uint8 must be 1 byte");
static_assert(int16(int8(-1)) == int16(-1), "uint8 sign uncorrect");

static_assert(sizeof(Size_T) == sizeof(void *), "Size_T must be as long as pointer");
static_assert(Size_T(-1) > Size_T(0), "Size_T sign uncorrect");

static_assert(sizeof(ANSICHAR) == sizeof(char), "ANSICHAR must be as long as char");
static_assert(int16(ANSICHAR(-1)) == int16(-1), "ANSICHAR sign uncorrect");

static_assert(sizeof(WIDECHAR) == 2 || sizeof(WIDECHAR) == 4, "WIDECHAR size uncorrect");
static_assert(sizeof(CHAR16) == 2, "CHAR16 size uncorrect");
static_assert(sizeof(CHAR32) == 4, "CHAR32 size uncorrect");

static_assert(sizeof(TChar) == 2, "TChar must be 2 bytes");


#define MIN_UINT8		((uint8)  0x00)
#define MIN_UINT16	((uint16)0x0000)
#define MIN_UINT32	((uint32)0x00000000)
#define MIN_UINT64	((uint64)0x0000000000000000)
#define MIN_INT8		((int8)    0x80)
#define MIN_INT16		((int16)  0x8000)
#define MIN_INT32		((int32)  0x80000000)
#define MIN_INT64		((int64)  0x8000000000000000)

#define MAX_UINT8		((uint8)  0xff)
#define MAX_UINT16	((uint16)0xffff)
#define MAX_UINT32	((uint32)0xffffffff)
#define MAX_UINT64	((uint64)0xffffffffffffffff)
#define MAX_INT8		((int8)    0x7f)
#define MAX_INT16		((int16)  0x7fff)
#define MAX_INT32		((int32)  0x7fffffff)
#define MAX_INT64		((int64)  0x7fffffffffffffff)




//==============================
//Type traits helpers
//==============================
template <typename... Types>
struct AndTest;

template <bool LHSValue, typename... RHS>
struct AndValue
{
	enum { Value = AndTest<RHS...>::Value };
};

template <typename... RHS>
struct AndValue<false, RHS...>
{
	enum { Value = false };
};

template <typename LHS, typename... RHS>
struct AndTest<LHS, RHS...> : AndValue<LHS::Value, RHS...>
{
};

template <>
struct AndTest<>
{
	enum { Value = true };
};


template <typename... Types>
struct OrTest;

template <bool LHSValue, typename... RHS>
struct OrValue
{
	enum { Value = OrTest<RHS...>::Value };
};

template <typename... RHS>
struct OrValue<true, RHS...>
{
	enum { Value = true };
};

template <typename LHS, typename... RHS>
struct OrTest<LHS, RHS...> : OrValue<LHS::Value, RHS...>
{
};

template <>
struct OrTest<>
{
	enum { Value = false };
};


template <typename Type>
struct NotTest
{
	enum { Value = !Type::Value };
};




//==============================
//Type traits
//==============================
//Returns the type T
//
template <typename T>
struct ExplicitType
{
	typedef T Type;
};


//Return the size of array
template <typename T, Size_T N>
char ( &ArraySizeHelper( const T (&)[N] ))[N];

// Number of elements in an array
#define ARRAY_SIZE( Array ) ( sizeof(ArraySizeHelper(Array)) )


/**
 * Helper template to test if Type is ReferenceType
 */
template<typename Type> struct IsReferenceType { enum { Value = false }; };
template<typename Type> struct IsReferenceType<Type&> { enum { Value = true }; };
template<typename Type> struct IsReferenceType<Type&&> { enum { Value = true }; };

/**
 * IsLValueReferenceType
 */
template<typename T> struct IsLValueReferenceType { enum { Value = false }; };
template<typename T> struct IsLValueReferenceType<T&> { enum { Value = true }; };

/**
 * IsRValueReferenceType
 */
template<typename T> struct IsRValueReferenceType { enum { Value = false }; };
template<typename T> struct IsRValueReferenceType<T&&> { enum { Value = true }; };


/**
 * Remove any references from a type.
 */
template <typename T> struct RemoveReference { typedef T Type; };
template <typename T> struct RemoveReference<T& > { typedef T Type; };
template <typename T> struct RemoveReference<T&&> { typedef T Type; };

/**
 * Remove any const/volatile qualifiers from a type.
 * "const int*" will failed, as the pointer is not const
 */
template <typename T> struct RemoveConstAndVolatile { typedef T Type; };
template <typename T> struct RemoveConstAndVolatile<const T> { typedef T Type; };
template <typename T> struct RemoveConstAndVolatile<volatile T> { typedef T Type; };
template <typename T> struct RemoveConstAndVolatile<const volatile T> { typedef T Type; };

/**
 * Removes one level of pointer from a type, e.g.:
 *
 * RemovePointer<      int32  >::Type == int32
 * RemovePointer<      int32* >::Type == int32
 * RemovePointer<      int32**>::Type == int32*
 * RemovePointer<const int32* >::Type == const int32
 */
template <typename T> struct RemovePointer { typedef T Type; };
template <typename T> struct RemovePointer<T*> { typedef T Type; };


/**************************
Helper template to test if
the type is trivial type
****************************/
template <typename Type>
struct IsTrivialType
{
	enum { Value = std::is_trivial<Type>::value };
};

/**************************
Helper template to test if
the type is integral type
****************************/
template <typename Type>
struct IsIntegralType
{
	enum { Value = std::is_integral<Type>::value };
};

/**************************
Helper template to test if
the type is numeric type
****************************/
template <typename T>
struct IsArithmeticType
{
	enum { Value = false };
};

template <> struct IsArithmeticType<float> { enum { Value = true }; };
template <> struct IsArithmeticType<double> { enum { Value = true }; };
template <> struct IsArithmeticType<long double> { enum { Value = true }; };
template <> struct IsArithmeticType<uint8> { enum { Value = true }; };
template <> struct IsArithmeticType<uint16> { enum { Value = true }; };
template <> struct IsArithmeticType<uint32> { enum { Value = true }; };
template <> struct IsArithmeticType<uint64> { enum { Value = true }; };
template <> struct IsArithmeticType<int8> { enum { Value = true }; };
template <> struct IsArithmeticType<int16> { enum { Value = true }; };
template <> struct IsArithmeticType<int32> { enum { Value = true }; };
template <> struct IsArithmeticType<int64> { enum { Value = true }; };
template <> struct IsArithmeticType<bool> { enum { Value = true }; };
template <> struct IsArithmeticType<WIDECHAR> { enum { Value = true }; };
template <> struct IsArithmeticType<ANSICHAR> { enum { Value = true }; };

template <typename T> struct IsArithmeticType<const T> { enum { Value = IsArithmeticType<T>::Value }; };
template <typename T> struct IsArithmeticType<volatile T> { enum { Value = IsArithmeticType<T>::Value }; };


/**************************
Helper template to test if
the type is char type we defined

****************************/
template <typename T>
struct IsCharType
{
	enum { Value = false };
};

template<> struct IsCharType<ANSICHAR> { enum { Value = true }; };
template<> struct IsCharType<CHAR16> { enum { Value = true }; };
template<> struct IsCharType<CHAR32> { enum { Value = true }; };
template<> struct IsCharType<WIDECHAR> { enum { Value = true }; };

/**************************
Helper template to test if
the char type is fixed encoding

****************************/
template <typename T>
struct IsCharFixedEncoding
{
	enum { Value = false };
};

template<> struct IsCharFixedEncoding<ANSICHAR> { enum { Value = true }; };
template<> struct IsCharFixedEncoding<CHAR32> { enum { Value = true }; };
template<> struct IsCharFixedEncoding<WIDECHAR> { enum { Value = true }; };


/**************************
Helper template to test if
the 2 char type is encoding ompatible

****************************/
template <typename CharTypeA, typename CharTypeB>
struct IsCharEncodingCompatible
{
	enum {
		Value = AndTest<IsCharFixedEncoding<CharTypeA>, IsCharFixedEncoding<CharTypeB>>::Value &&
		sizeof(CharTypeA) == sizeof(CharTypeB)
	};

};


/**************************
Helper template to test if
the 2 char type is compare compatible

****************************/
template<typename A, typename B>
struct IsCharComparisonCompatible
{
	enum { Value = false };
};
template<> struct IsCharComparisonCompatible <WIDECHAR, ANSICHAR> { enum { Value = true }; };
template<> struct IsCharComparisonCompatible <CHAR16, ANSICHAR> { enum { Value = true }; };
template<> struct IsCharComparisonCompatible <CHAR32, ANSICHAR> { enum { Value = true }; };

template<> struct IsCharComparisonCompatible <WIDECHAR, WIDECHAR> { enum { Value = true }; };
template<> struct IsCharComparisonCompatible <CHAR16, CHAR16> { enum { Value = true }; };
template<> struct IsCharComparisonCompatible <CHAR32, CHAR32> { enum { Value = true }; };




/**************************
Helper template to test if
the type is enum type

****************************/
template <typename Type>
struct IsEnumType
{
	enum { Value = std::is_enum<Type>::value };
};


/**************************
Helper template to test if
the type is enum class type

****************************/
template <typename Type>
struct IsEnumConvertibleToInt
{
	//If Type can convert to int, return a char& type (size 2) array 
	//else return a char type
	static char(&Resolve(int))[2];
	static char Resolve(...);

	enum { Value = sizeof(Resolve(Type())) - 1 };
};

template <typename Type>
struct IsEnumClassType
{
	enum { Value = AndTest<IsEnumType<Type>, NotTest<IsEnumConvertibleToInt<Type>>>::Value };
};


 /**************************
 Helper template to test if
 the type has no constructor
 ****************************/
template<typename Type>
struct IsZeroConstructType
{
	enum { Value = OrTest<IsEnumType<Type>, IsArithmeticType<Type>, IsCharType<Type>>::Value };

};


/**************************
Helper template to test if
the type is a pointer.
****************************/
template <typename Type>
struct IsPointerType
{
	enum { Value = false };
};

template <typename Type> struct IsPointerType<Type*> { enum { Value = true }; };

template <typename Type> struct IsPointerType<const  Type> { enum { Value = IsPointerType<Type>::Value }; };
template <typename Type> struct IsPointerType<volatile Type> { enum { Value = IsPointerType<Type>::Value }; };
template <typename Type> struct IsPointerType<const volatile Type> { enum { Value = IsPointerType<Type>::Value }; };




/**************************
Helper template to test if
the type is pod type
****************************/
template <typename Type>
struct IsPodType
{
	enum { Value = std::is_pod<Type>::value };
};


/**************************
Helper template to test if
the type is trivally assginable
****************************/
template <typename Type>
struct IsTrivalAssginable
{
	enum { Value = __has_trivial_assign(Type) };
};


/**************************
Helper template to test if
the type is trivially copy constructible

****************************/
//Just using std template
template <typename Type>
struct IsTriviallyCopyConstructible
{
	enum { Value = std::is_trivially_copy_constructible<Type>::value };
};


/***************************
Helper template to test if
Type B can be memcopyed in order to get an instance of Type A, rather than using a constructor

TypeA represents Type To Construct
TypeB represents Type To Be Copied

Type A = PODType, Type B = PODType -> true
class or struct that has no user-defined destructor, no user-defined copy assignment operator, and no nonstatic members of pointer-to-member type can be trivially memcopied

Type A = const int*, Type B = int* -> true
a non-const Derived pointer is trivially memcopyable as a const Base pointer

Type A = int*, Type B = const int* -> false
const pointer can not be memcopyed as a non-const pointer

Type A = int32, Type B = uint32 -> true
Type A = uint32, Type B = int32 -> true
signed integers can be memcpy as unsigned integers, and so vice versa
****************************/
template <typename TypeA, typename TypeB>
struct IsBitwiseConstructible
{	
	static_assert(AndTest<NotTest<IsReferenceType<TypeA>>, NotTest<IsReferenceType<TypeB>>>::Value, "Do not cast reference type");

	enum { Value = false };
};//Default set to false

template <typename TypeA>
struct IsBitwiseConstructible<TypeA, TypeA>
{
	enum { Value = OrTest<IsTriviallyCopyConstructible<TypeA>, IsPodType<TypeA>>::Value };
};

template <typename TypeA, typename TypeB>
struct IsBitwiseConstructible<const TypeA, TypeB> : IsBitwiseConstructible<TypeA, TypeB>
{
	//return to default
};

template <typename TypeA>
struct IsBitwiseConstructible<const TypeA*, TypeA*>
{
	enum { Value = true };
};

template <> struct IsBitwiseConstructible<uint8, int8> { enum { Value = true }; };
template <> struct IsBitwiseConstructible< int8, uint8> { enum { Value = true }; };

template <> struct IsBitwiseConstructible<uint16, int16> { enum { Value = true }; };
template <> struct IsBitwiseConstructible< int16, uint16> { enum { Value = true }; };

template <> struct IsBitwiseConstructible<uint32, int32> { enum { Value = true }; };
template <> struct IsBitwiseConstructible< int32, uint32> { enum { Value = true }; };

template <> struct IsBitwiseConstructible<uint64, int64> { enum { Value = true }; };
template <> struct IsBitwiseConstructible< int64, uint64> { enum { Value = true }; };


/**************************
Helper template to test if
two type are same

Example:
TypeIsEqual<TypeA, TypeB>::Value

if TypeA == TypeB, Value will be true
****************************/
template<typename TypeA, typename TypeB>
struct IsTypeEqual;

template<typename TypeA, typename TypeB>
struct IsTypeEqual
{
	enum { Value = false };
};

template<typename TypeA>
struct IsTypeEqual<TypeA, TypeA>
{
	enum { Value = true };
};






/**************************
Helper template to test if
the type is trivially destructible

****************************/
//Just using std template
template <typename Type>
struct IsTriviallyDestructible
{
	enum { Value = std::is_trivially_destructible<Type>::value };
};


/**************************
Helper template to test if
the type is bitwise relocatable

****************************/
template <typename DestType, typename SrcType>
struct IsBitwiseRelocatable
{
	enum
	{
		Value = (
			OrTest<IsTypeEqual<DestType, SrcType>,  AndTest<IsBitwiseConstructible<DestType, SrcType>, IsTriviallyDestructible<SrcType>>>::Value
		)
	};
};





/**************************
Helper template to test if
the type is bitwise comparable

****************************/
//Just using std template
template <typename Type>
struct IsBitwiseComparable
{
	enum { Value = OrTest<IsEnumType<Type>, IsArithmeticType<Type>, IsPointerType<Type>>::Value };
};



/**************************
Helper template to test if
the type is trivially copy assignable

****************************/
template <typename Type>
struct IsTriviallyCopyAssignable
{
	enum { Value = OrTest<IsTrivalAssginable<Type>, IsPodType<Type>>::Value };
};

/**************************
Gets the Nth type in a template parameter pack. N must be less than sizeof...(Types)
****************************/
template <int32 N, typename... Types>
struct GetNthTypeFromParameter;

template <int32 N, typename T, typename... OtherTypes>
struct GetNthTypeFromParameter<N, T, OtherTypes...>
{
	using Type = typename GetNthTypeFromParameter<N - 1, OtherTypes...>::Type;
};

template <typename T, typename... OtherTypes>
struct GetNthTypeFromParameter<0, T, OtherTypes...>
{
	using Type = T;
};




/**
 MoveTemp will cast a reference to an rvalue reference.
 = std::move except that it will not compile when passed an rvalue or
 const object, because we would prefer to be informed when MoveTemp will have no effect.
 */
template <typename T>
FORCE_INLINE typename RemoveReference<T>::Type&& MoveTemp(T&& Obj)
{
	typedef typename RemoveReference<T>::Type CastType;

	// Validate that we're not being passed an rvalue or a const object - the former is redundant, the latter is almost certainly a mistake
	static_assert(IsLValueReferenceType<T>::Value, "MoveTemp called on an rvalue");
	static_assert(!IsTypeEqual<CastType&, const CastType&>::Value, "MoveTemp called on a const object");

	return (CastType&&)Obj;
}

/**************************
 MoveTemp will cast a reference to an rvalue reference.
 = std::move
****************************/
template <typename T>
FORCE_INLINE typename RemoveReference<T>::Type && MoveTempIfPossible(T && Obj)
{
	typedef typename RemoveReference<T>::Type CastType;
	return (CastType&&)Obj;
}

/**************************
  CopyTemp will enforce the creation of an rvalue which can bind to rvalue reference parameters.
****************************/
template <typename T>
FORCE_INLINE T CopyTemp(T& Val)
{
	return const_cast<const T&>(Val);
}

template <typename T>
FORCE_INLINE T CopyTemp(const T& Val)
{
	return Val;
}

template <typename T>
FORCE_INLINE T&& CopyTemp(T&& Val)
{
	// If we already have an rvalue, just return it unchanged, rather than needlessly creating yet another rvalue from it.
	return MoveTemp(Val);
}

/**************************
  Forward will cast a reference to an rvalue reference.
  = std::forward.
****************************/
template <typename T>
FORCE_INLINE T&& Forward(typename RemoveReference<T>::Type& Obj)
{
	return (T&&)Obj;
}

template <typename T>
FORCE_INLINE T&& Forward(typename RemoveReference<T>::Type&& Obj)
{
	return (T&&)Obj;
}



/**************************
Function trigger

Example:
typename FuncTrigger<SomeExpression, ReturnType>::Type AFunction(Argements...) {}

if SomeExpression = true, FuncTrigger::Type will be ReturnType, the AFunction will be instantiated
else if SomeExpression = false, FuncTrigger::Type will be nothing, the AFunction will not be instantiated
****************************/
template <bool IsEnable, typename ReturnType = void>
class FuncTrigger;//declaration

//If IsEnable = true
template <typename ReturnType>
class FuncTrigger<true, ReturnType>
{
public:
	typedef ReturnType Type;
};

//If IsEnable = false
template <typename ReturnType>
class FuncTrigger<false, ReturnType>
{};


/**************************
Make Integer Sequence

Example:
MakeIntegerSequence<10>
Output:
0 1 2 3 4 5 6 7 8 9
****************************/
template <typename T, T... Indices>
struct IntegerSequence
{
};

#ifdef _MSC_VER

template <typename T, T N>
using MakeIntegerSequence = __make_integer_seq<IntegerSequence, T, N>;

#elif __has_builtin(__make_integer_seq)

template <typename T, T N>
using MakeIntegerSequence = __make_integer_seq<IntegerSequence, T, N>;

#else

namespace IntegerSequenceContent
{
	template <typename T, unsigned N>
	struct MakeIntegerSequenceImpl;
}

template <typename T, T N>
using MakeIntegerSequence = typename IntegerSequenceContent::MakeIntegerSequenceImpl<T, N>::Type;

namespace IntegerSequenceContent
{
	template<unsigned N, typename T1, typename T2>
	struct ConcatImpl;

	template<unsigned N, typename T, T... Indices1, T... Indices2>
	struct ConcatImpl<N, IntegerSequence<T, Indices1...>, IntegerSequence<T, Indices2...>> : IntegerSequence<T, Indices1..., (T(N + Indices2))...>
	{
		using Type = IntegerSequence<T, Indices1..., (T(N + Indices2))...>;
	};

	template<unsigned N, typename T1, typename T2>
	using Concat = typename ConcatImpl<N, T1, T2>::Type;


	template <typename T, unsigned N>
	struct MakeIntegerSequenceImpl : Concat<N / 2, MakeIntegerSequence<T, N / 2>, MakeIntegerSequence<T, N - N / 2>>
	{
		using Type = Concat<N / 2, MakeIntegerSequence<T, N / 2>, MakeIntegerSequence<T, N - N / 2>>;
	};

	template <typename T>
	struct MakeIntegerSequenceImpl<T, 1> : IntegerSequence<T, T(0)>
	{
		using Type = IntegerSequence<T, T(0)>;
	};

	template <typename T>
	struct MakeIntegerSequenceImpl<T, 0> : IntegerSequence<T>
	{
		using Type = IntegerSequence<T>;
	};
};

#endif


/**************************
DecayedType

Returns the decayed type of T, meaning it removes all references, qualifiers and
applies array-to-pointer and function-to-pointer conversions.
http://en.cppreference.com/w/cpp/types/decay
****************************/
template <typename T>
struct DecayNonReference
{
	typedef typename RemoveConstAndVolatile<T>::Type Type;
};

template <typename T>
struct DecayNonReference<T[]>
{
	typedef T* Type;
};

template <typename T, uint32 N>
struct DecayNonReference<T[N]>
{
	typedef T* Type;
};

template <typename RetType, typename... Params>
struct DecayNonReference<RetType(Params...)>
{
	typedef RetType(*Type)(Params...);
};


template <typename T>
struct DecayedType
{
	typedef typename DecayNonReference<typename RemoveReference<T>::Type>::Type Type;
};
