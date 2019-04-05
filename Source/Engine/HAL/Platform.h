#pragma once

#include "Global/GlobalConfigs.h"

//==============================
//PlatformMisc
//
//PlatformTypes;
//PlatformAtomics;
//PlatformHelpers;
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
//Be redefined by different platforms
//==============================
/*
   Use wchar as TEXT type
*/
#ifndef PLATFORM_TEXT_IS_WCHAR
   #define PLATFORM_TEXT_IS_WCHAR 0
#endif
/*
   Use char16_t as TEXT type
*/
#ifndef PLATFORM_TEXT_IS_CHAR16
   #define PLATFORM_TEXT_IS_CHAR16 0
#endif
/*
   Use char32_t as TEXT type
*/
#ifndef PLATFORM_TEXT_IS_CHAR32
#define PLATFORM_TEXT_IS_CHAR32 0
#endif



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




//==============================
//Others
//==============================
#ifndef RESTRICT
#define RESTRICT __restrict
#endif

#ifndef FORCE_INLINE
#define FORCE_INLINE inline
#endif



