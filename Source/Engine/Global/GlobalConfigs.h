/***********************************
Naming standard:

1.class\enum\function\global, local, member variable:
"SomeTypes"

2.macro\global constant variable:
"SOME_GLOBAL_CONSTANTS"


/!!!!!!!!!!!!!!!!!!!!!!!
	  No 32 bit App
!!!!!!!!!!!!!!!!!!!!!!!!

************************************/
#pragma once


//==============================
//Version
//==============================
#define APPLICATION_VERSION "-ver 0.01"
#define APPLICATION_VERSION_NUM 001

#define APPLICATION_IDENTITY "_RT_Renderer_001"
#define APPLICATION_NAME "RT-Renderer"



//==============================
//Platform global configs
//==============================
#ifdef _WIN32
   #define PLATFORM_WINDOWS 1
#elif __linux__
   #define PLATFORM_LINUX 1
#elif __APPLE__
   #define PLATFORM_MAC 1
#endif


#if !defined(PLATFORM_WINDOWS)
   #define PLATFORM_WINDOWS 0
#endif
#if !defined(PLATFORM_LINUX)
   #define PLATFORM_LINUX 0
#endif
#if !defined(PLATFORM_MAC)
   #define PLATFORM_MAC 0
#endif


/*************************
No 32-bit application
*************************/
#if PLATFORM_WINDOWS
   #ifndef _WIN64
      #error 32-bit application is not supported 
   #endif
#elif PLATFORM_LINUX
   #error No platform implement code here
#elif PLATFORM_MAC
   #error No platform implement code here
#endif





/*************************
Just for test 
*************************/
#define USE_FOR_TEST_CPP 0
#
#if _DEBUG
#  if USE_FOR_TEST_CPP
#    define FOR_TEST_CPP 1
#  else 
#    define MAIN_LAUNCH_CPP 1
#  endif
#else
#  define MAIN_LAUNCH_CPP 1
#endif   











