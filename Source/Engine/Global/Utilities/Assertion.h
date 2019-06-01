/****************************************
Debug assertions


*****************************************/
#pragma once

#include "Global/GlobalConfigs.h"
#include "Global/GlobalType.h"
#include "Global/EngineVariables.h"
#include "Global/Utilities/Misc.h"

#define DESCRIPTION_LENGTH 1024
#define PREFIX_LENGTH 512




class Debug
{
public:
	template <typename... Types>
	FORCE_INLINE
	static void OnAssertionFailed(const ANSICHAR* Expression, const ANSICHAR* File, int32 Line, const ANSICHAR* Format, Types... Args)
	{
		OuputAssertionFailed(Expression, File, Line, Format, Args...);
	}

public:
	static PlatformCriticalSection DebugCriticalSection;

private:
	static void OuputAssertionFailed(const ANSICHAR* Expression, const ANSICHAR* File, int32 Line, const ANSICHAR* Format, ...);


	

};




#define DEBUG_BREAK {if(Platform::IsDebuggerPresent()) { Platform::DebugBreak(); }}

//If we ouput the assertion failed message to console
#define DEBUG_ASSERTION_TO_CONSOLE 0  


//Debug assertion macros
#if ENABLE_DEBUG_ASSERTION
#define CHECK(Expression) { \
	 if(UNLIKELY(!(Expression))) \
     { \
         Debug::OnAssertionFailed( #Expression, __FILE__, __LINE__, ""); \
         DEBUG_BREAK \
	 } \
     }

#define CHECKF(Expression, Format, ...) { \
	 if(UNLIKELY(!(Expression))) \
     { \
         Debug::OnAssertionFailed( #Expression, __FILE__, __LINE__, Format, ##__VA_ARGS__ ); \
         DEBUG_BREAK \
	 } \
     }

#else
#define CHECK(Expression)
#define CHECKF(Expression, Format, ...)

#endif