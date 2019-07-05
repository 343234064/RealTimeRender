#pragma once

#include "Global/GlobalConfigs.h"

//==============================
//Platform
//
//
//==============================
#if PLATFORM_WINDOWS
#include "HAL/Platforms/Windows/WindowsPlatform.h"
#elif PLATFORM_LINUX
#include "HAL/Platforms/Linux/LinuxPlatform.h"
#elif PLATFORM_MAC
#include "HAL/Platforms/Mac/MacPlatform.h"
#else
#error Unknown platform
#endif


//==============================
//Macro configs
//Redefined by different platforms
//==============================
//==============================
//Only using 2 bytes char type as text type
//==============================
/*
   Use wchar_t as TEXTS/TChar type
   Coding type is depending on platforms
*/
#ifndef PLATFORM_TEXT_IS_WCHAR
   #define PLATFORM_TEXT_IS_WCHAR 0
#endif
/*
   Use char16_t as TEXTS/TChar type
*/
#ifndef PLATFORM_TEXT_IS_CHAR16
   #define PLATFORM_TEXT_IS_CHAR16 0
#endif

/*
   char32_t is not use
*/
//#ifndef PLATFORM_TEXT_IS_CHAR32
//#define PLATFORM_TEXT_IS_CHAR32 0
//#endif



/*
   Intel's Threading Building Blocks
   64-bit scalable memory allocator
*/
#if PLATFORM_ALLOC_SUPPORT_TBB
   #ifndef PLATFORM_ALLOC_USE_TBB
      #define PLATFORM_ALLOC_USE_TBB 0
   #endif
#endif
/*
   Jemlloc memory allocator
*/
#if PLATFORM_ALLOC_SUPPORT_JEM 
   #ifndef PLATFORM_ALLOC_USE_JEM
      #define PLATFORM_ALLOC_USE_JEM 0
   #endif
#endif

// Hints compiler that expression is unlikely to be true
#ifndef UNLIKELY	
#if (PLATFORM_LINUX) && ( defined(__clang__) || defined(__GNUC__) ) 
#define UNLIKELY(x)	__builtin_expect(!!(x), 0)
#else
#define UNLIKELY(x)	(x)
#endif
#endif


//If the platform is little-endian
/*
static const union
{
	unsigned char _Bytes[4];
	uint32_t _Value;
} Host_Order32 = { {0, 1, 2, 3} };
enum
{
	LITTLE_ENDIAN_ = 0x03020100ul,
	BIG_ENDIAN_    = 0x00010203ul
};

#ifndef PLATFORM_LITTLE_ENDIAN_RUNTIME
#   define PLATFORM_LITTLE_ENDIAN_RUNTIME ( LITTLE_ENDIAN_ == Host_Order32._Value )
#endif 
#ifndef PLATFORM_BIG_ENDIAN_RUNTIME
#   define PLATFORM_BIG_ENDIAN_RUNTIME ( BIG_ENDIAN_ == Host_Order32._Value )
#endif
*/



//==============================
//Others
//==============================
#ifndef RESTRICT
#define RESTRICT __restrict
#endif

#ifndef FORCE_INLINE
#define FORCE_INLINE inline
#endif



