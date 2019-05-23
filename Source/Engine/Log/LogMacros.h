#pragma once


#include "Global/GlobalConfigs.h"
#include "Global/GlobalType.h"
#include "Log/LogEnum.h"



/****DEBUG LOG*********/

//If we output the debug log to console
//else output to the IDE
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
#     define DEBUGLOG(Format, ...) printf(Format, ##__VA_ARGS__)
#  else 
#     define DEBUGLOG(Format, ...) {\
                ANSICHAR Text[DEBUG_LOG_MAX]; \
		        PlatformChars::Sprintf(Text, DEBUG_LOG_MAX, Format, ##__VA_ARGS__); \
		        Platform::LocalPrintA(Text); }
#  endif
#else
#  define DEBUGLOG(Format, ...)
#endif



/********LOG*********/
struct LogMisc
{
public:
	static void LogInternal(LogVerbosity Verbosity, const TChar* Format, ...);

};


#define LOG(Verbosity, Format, ...) { LogMisc::LogInternal(Verbosity, Format, ##__VA_ARGS__); } 
#define LOG_CHECK(Expression, Verbosity, Format, ...) { if(Expression) LogMisc::LogInternal(Verbosity, Format, ##__VA_ARGS__); }