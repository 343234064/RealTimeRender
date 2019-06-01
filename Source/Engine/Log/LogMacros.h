#pragma once


#include "Global/GlobalConfigs.h"
#include "Global/GlobalType.h"
#include "Log/LogEnum.h"
#include "Global/EngineVariables.h"




//To handle fatal message output
struct FatalLog
{
	static void Output(const TChar* ClassName, const TChar* Format, ...);
	static void OutputToLocal(const TChar* Format, ...);
};


//Helper macros
//Compile-time selector
#define CONCATENATE_INNER(x, y) x##y
#define CONCATENATE(x, y) CONCATENATE_INNER(x, y)
#define CONCATENATE_TRI(x, y, z) CONCATENATE(CONCATENATE_INNER(x, y), z)

#define LOG_TYPE_IS_Fatal(Block1, Block2) Block1
#define LOG_TYPE_IS_Error(Block1, Block2) Block2
#define LOG_TYPE_IS_Warn (Block1, Block2) Block2
#define LOG_TYPE_IS_Info (Block1, Block2) Block2
#define LOG_TYPE_IS_Debug(Block1, Block2) Block2

#define IF_LOG_TYPE_IS(Type, Block1, Block2) CONCATENATE(LOG_TYPE_IS_, Type)(Block1, Block2)
#define GET_LOG_TYPE(Type) CONCATENATE(LogType::, Type)

#define TEXT_BLOCK_START TEXTS("
#define TEXT_BLOCK_END  ")
#define GET_CLASS_NAME(Name) CONCATENATE_TRI(TEXT_BLOCK_START, Name, TEXT_BLOCK_END)


/****DEBUG LOG*********/
#if ENABLE_DEBUG_LOG

#define DEBUG(Format, ...) \
{ \
     gLogger->Log(LogType::Debug, -1.0, Format, ##__VA_ARGS__); \
}

#define DEBUGT(Time, Format, ...) \
{ \
     gLogger->Log(LogType::Debug, Time, Format, ##__VA_ARGS__); \
}

#else
#  define DEBUG(Format, ...)
#  define DEBUGT(Time, Format, ...)

#endif



/********LOG********/
#if ENABLE_LOG

#define LOG(Type, ClassName, Format, ...) \
{ \
    static_assert(GET_LOG_TYPE(Type) != LogType::Debug, "Please use DEBUG macro to output Debug log"); \
    IF_LOG_TYPE_IS(Type, \
        { \
		   FatalLog::Output(GET_CLASS_NAME(ClassName), Format, ##__VA_ARGS__); \
           if(Platform::IsDebuggerPresent()) { Platform::DebugBreak(); } \
        }, \
        { \
           gLogger->Log(GET_LOG_TYPE(Type), GET_CLASS_NAME(ClassName), -1.0, Format, ##__VA_ARGS__); \
	    } \
    ) \
} 

#define LOGT(Type, ClassName, Time, Format, ...) \
{ \
    static_assert(GET_LOG_TYPE(Type) != LogType::Debug, "Please use DEBUG macro to output Debug log"); \
    IF_LOG_TYPE_IS(Type, \
        { \
		   FatalLog::Handle(__FILE__, __LINE__, Format, ##__VA_ARGS__); \
        }, \
        { \
           gLogger->Log(GET_LOG_TYPE(Type), GET_CLASS_NAME(ClassName), Time, Format, ##__VA_ARGS__); \
	    } \
    ) \
} 

#define LOG_CHECK(Expression, Type, ClassName, Format, ...) \
{ \
    static_assert(GET_LOG_TYPE(Type) != LogType::Debug, "Please use DEBUG macro to output Debug log"); \
    if(Expression) \
    IF_LOG_TYPE_IS(Type, \
        { \
		   FatalLog::Handle(__FILE__, __LINE__, Format, ##__VA_ARGS__); \
        }, \
        { \
           gLogger->Log(GET_LOG_TYPE(Type), GET_CLASS_NAME(ClassName), -1.0, Format, ##__VA_ARGS__); \
	    } \
    ) \
} 

#define LOGT_CHECK(Expression, Type, ClassName, Time, Format, ...) \
{ \
    static_assert(GET_LOG_TYPE(Type) != LogType::Debug, "Please use DEBUG macro to output Debug log"); \
    if(Expression) \
    IF_LOG_TYPE_IS(Type, \
        { \
		   FatalLog::Handle(__FILE__, __LINE__, Format, ##__VA_ARGS__); \
        }, \
        { \
           gLogger->Log(GET_LOG_TYPE(Type), GET_CLASS_NAME(ClassName), Time, Format, ##__VA_ARGS__); \
	    } \
    ) \
} 

#else
#define LOG(Type, Format, ...)
#define LOGT(Type, Time, Format, ...)
#define LOG_CHECK(Expression, Time, Format, ...)
#define LOGT_CHECK(Expression, Type, Time, Format, ...)

#endif // ENABLE_LOG

