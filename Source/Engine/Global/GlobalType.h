#pragma once

#include <stdio.h>
#include <type_traits>
#include "HAL/Platform.h"



//==============================
//Data types
//==============================

typedef	PlatformTypes::int8		int8;		//8-bit  signed.
typedef PlatformTypes::uint8 	uint8;		//8-bit  unsigned.

typedef PlatformTypes::int16	int16;		//16-bit signed.
typedef PlatformTypes::uint16	uint16;		//16-bit unsigned.

typedef PlatformTypes::int32 	int32;		//32-bit signed.
typedef PlatformTypes::uint32	uint32;		//32-bit unsigned.

typedef PlatformTypes::int64	int64;		//64-bit signed.
typedef PlatformTypes::uint64	uint64;		//64-bit unsigned.

typedef PlatformTypes::Size_T   Size_T;     //unsigned int, has same size as pointer 

typedef PlatformTypes::ANSICHAR ANSICHAR;   //8-bit ANSI character
typedef PlatformTypes::WIDECHAR WIDECHAR;   //wide character, for UNICODE
typedef PlatformTypes::CHAR16   CHAR16;     //UTF-16, for UNICODE
typedef PlatformTypes::CHAR32   CHAR32;     //UTF-32, for UNICODE

#if   PLATFORM_TEXT_IS_CHAR16
typedef CHAR16                  TChar;      //basic char type used in string
#elif PLATFORM_TEXT_IS_CHAR32
typedef CHAR32                  TChar;      //basic char type used in string
#else PLATFORM_TEXT_IS_WCHAR
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


#define MIN_UINT8		((uint8) 0x00)
#define	MIN_UINT16		((uint16)0x0000)
#define	MIN_UINT32		((uint32)0x00000000)
#define MIN_UINT64		((uint64)0x0000000000000000)
#define MIN_INT8		((int8)	 0x80)
#define MIN_INT16		((int16) 0x8000)
#define MIN_INT32		((int32) 0x80000000)
#define MIN_INT64		((int64) 0x8000000000000000)

#define MAX_UINT8		((uint8) 0xff)
#define MAX_UINT16		((uint16)0xffff)
#define MAX_UINT32		((uint32)0xffffffff)
#define MAX_UINT64		((uint64)0xffffffffffffffff)
#define MAX_INT8		((int8)	 0x7f)
#define MAX_INT16		((int16) 0x7fff)
#define MAX_INT32		((int32) 0x7fffffff)
#define MAX_INT64		((int64) 0x7fffffffffffffff)




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



/**
 * Helper template to test if Type is ReferenceType
 */
template<typename Type> struct IsReferenceType { enum { Value = false }; };
template<typename Type> struct IsReferenceType<Type&> { enum { Value = true }; };
template<typename Type> struct IsReferenceType<Type&&> { enum { Value = true }; };



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
	static_assert((!IsReferenceType<TypeA>::Value) && (!IsReferenceType<TypeB>::Value), "Do not cast reference type");

	enum { Value = false };
};//Default set to false

template <typename TypeA>
struct IsBitwiseConstructible<TypeA, TypeA>
{
	enum { Value = (std::is_trivially_copy_constructible<TypeA>::value || std::is_pod<TypeA>::value) };
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
			IsTypeEqual<DestType, SrcType>::Value ||
			(
				IsBitwiseConstructible<DestType, SrcType>::Value &&
				IsTriviallyDestructible<SrcType>::Value
			)
		)
	};
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



/**************************
Helper template to test if
the type is bitwise comparable

****************************/
//Just using std template
template <typename Type>
struct IsBitwiseComparable
{
	enum { Value = std::is_enum<Type>::value || std::is_arithmetic<Type>::value || std::is_pointer<Type>::value };
};



/**************************
Helper template to test if
the type is trivially copy assignable

****************************/
template <typename Type>
struct IsTriviallyCopyAssignable
{
	enum { Value = __has_trivial_assign(Type) || std::is_pod<Type>::value };
};


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
template<> struct IsCharType<CHAR16>   { enum { Value = true }; };
template<> struct IsCharType<CHAR32>   { enum { Value = true }; };
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
template<> struct IsCharFixedEncoding<CHAR32>   { enum { Value = true }; };
template<> struct IsCharFixedEncoding<WIDECHAR> { enum { Value = true }; };


/**************************
Helper template to test if
the 2 char type is encoding ompatible

****************************/
template <typename CharTypeA, typename CharTypeB>
struct IsCharEncodingCompatible
{
	enum { Value = IsCharFixedEncoding<CharTypeA>::Value &&
		           IsCharFixedEncoding<CharTypeB>::Value &&
	               sizeof(CharTypeA) == sizeof(CharTypeB) };

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