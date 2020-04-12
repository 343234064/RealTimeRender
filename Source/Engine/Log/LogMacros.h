#pragma once


#include "Global/GlobalConfigs.h"
#include "Global/GlobalType.h"
#include "Log/LogEnum.h"
#include "Global/EngineVariables.h"



//To handle fatal message output
struct FatalLog
{
	static void Output(const TChar* ClassName, int32 Line, const TChar* Format, ...);
	static void OutputToLocal(const TChar* Format, ...);
	static void Shutdown();
};


//To handle info message output
struct MessageLog
{
	//The time type that output to log file
    //UTC, Local, TimeSinceStart
	static LogTime TimeType;

	static void Output(LogType Type, const TChar* ClassName, int32 Line, double Time, const TChar* Format, ...);
	static void Shutdown();
};

#define LOG_SHUTDOWN MessageLog::Shutdown()
#define FATALLOG_SHUTDOWN FatalLog::Shutdown()




//Compile-time helpers
#define LOG_TYPE_IS_Fatal(Block1, Block2) Block1
#define LOG_TYPE_IS_Error(Block1, Block2) Block2
#define LOG_TYPE_IS_Warn(Block1, Block2)  Block2
#define LOG_TYPE_IS_Info(Block1, Block2)  Block2
#define LOG_TYPE_IS_Debug(Block1, Block2) Block2


#define IF_LOG_TYPE_IS(Type, Block1, Block2) CONCATENATE(LOG_TYPE_IS_, Type)(Block1, Block2)
#define GET_LOG_TYPE(Type) CONCATENATE(LogType::, Type)

#define GET_CLASS_NAME(Name) _TEXTS(Name)


/****DEBUG LOG*********/
#if ENABLE_DEBUG_LOG

#define DEBUG(ClassName, Format, ...) \
{ \
     MessageLog::Output(LogType::Debug, GET_CLASS_NAME(ClassName), __LINE__, -1.0, Format, ##__VA_ARGS__); \
}

#define DEBUGT(ClassName, Time, Format, ...) \
{ \
     MessageLog::Output(LogType::Debug, GET_CLASS_NAME(ClassName), __LINE__, Time, Format, ##__VA_ARGS__); \
}

#else
#  define DEBUG(ClassName, Format, ...)
#  define DEBUGT(ClassName, Time, Format, ...)

#endif



/********LOG********/
#if ENABLE_LOG

#define LOG(Type, ClassName, Format, ...) \
{ \
    static_assert(GET_LOG_TYPE(Type) != LogType::Debug, "Please use DEBUG macro to output Debug log"); \
    IF_LOG_TYPE_IS(Type, \
        { \
                FatalLog::Output(GET_CLASS_NAME(ClassName), __LINE__, Format, ##__VA_ARGS__); \
            \
        }, \
        { \
                MessageLog::Output(GET_LOG_TYPE(Type), GET_CLASS_NAME(ClassName), __LINE__, -1.0, Format, ##__VA_ARGS__); \
	    } \
    ); \
} 

#define LOGT(Type, ClassName, Time, Format, ...) \
{ \
    static_assert(GET_LOG_TYPE(Type) != LogType::Debug, "Please use DEBUG macro to output Debug log"); \
    IF_LOG_TYPE_IS(Type, \
        { \
                FatalLog::Output(GET_CLASS_NAME(ClassName), __LINE__, Format, ##__VA_ARGS__); \
                if(Platform::IsDebuggerPresent()) { Platform::DebugBreak(); } \
        }, \
        { \
                MessageLog::Output(GET_LOG_TYPE(Type), GET_CLASS_NAME(ClassName), __LINE__, Time, Format, ##__VA_ARGS__); \
	    } \
    ) \
} 

#define LOG_CHECK(Expression, Type, ClassName, Format, ...) \
{ \
    static_assert(GET_LOG_TYPE(Type) != LogType::Debug, "Please use DEBUG macro to output Debug log"); \
    if(Expression) \
    IF_LOG_TYPE_IS(Type, \
        { \
                FatalLog::Output(GET_CLASS_NAME(ClassName), __LINE__, Format, ##__VA_ARGS__); \
                if(Platform::IsDebuggerPresent()) { Platform::DebugBreak(); } \
        }, \
        { \
                MessageLog::Output(GET_LOG_TYPE(Type), GET_CLASS_NAME(ClassName), __LINE__, -1.0, Format, ##__VA_ARGS__); \
	    } \
    ) \
} 

#define LOGT_CHECK(Expression, Type, ClassName, Time, Format, ...) \
{ \
    static_assert(GET_LOG_TYPE(Type) != LogType::Debug, "Please use DEBUG macro to output Debug log"); \
    if(Expression) \
    IF_LOG_TYPE_IS(Type, \
        { \
                FatalLog::Output(GET_CLASS_NAME(ClassName), __LINE__, Format, ##__VA_ARGS__); \
                if(Platform::IsDebuggerPresent()) { Platform::DebugBreak(); } \
        }, \
        { \
                MessageLog::Output(GET_LOG_TYPE(Type), GET_CLASS_NAME(ClassName), __LINE__, Time, Format, ##__VA_ARGS__); \
	    } \
    ) \
} 

#else
#define LOG(Type, ClassName, Format, ...)
#define LOGT(Type, ClassName, Time, Format, ...)
#define LOG_CHECK(Expression, Type, ClassName, Format, ...)
#define LOGT_CHECK(Expression, Type, ClassName, Time, Format, ...)

#endif // ENABLE_LOG

