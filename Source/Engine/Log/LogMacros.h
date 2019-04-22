#pragma once


#include "Global/GlobalConfigs.h"
#include "Global/GlobalType.h"
#include "Log/Logger.h"



//If we output the debug log to console
#define DEBUG_LOG_TO_CONSOLE 0

#if DEBUG_LOG_TO_CONSOLE
#include<stdio.h>
#else
#include "HAL/Platform.h"
#include "HAL/Chars.h"
#endif

#define DEBUG_LOG_MAX 512



#if ENABLE_DEBUG_LOG
#  if DEBUG_LOG_TO_CONSOLE
#     define DEBUGLOG(Format, ...) printf(Format, ##__VA_ARGS__);
#  else 
#     define DEBUGLOG(Format, ...) {\
                ANSICHAR Text[DEBUG_LOG_MAX]; \
		        PlatformChars::Sprintf(Text, DEBUG_LOG_MAX, Format, ##__VA_ARGS__); \
		        Platform::LocalPrintA(Text); }
#  endif
#else
#  define DEBUGLOG(Format, ...)
#endif

 
#define LOG(Logger, Verbosity, Format, ...)