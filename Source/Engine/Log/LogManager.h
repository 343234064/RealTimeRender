/************************************
Logger to manage all the output 


*************************************/
#pragma once

#include "Global/GlobalConfigs.h"
#include "Global/GlobalType.h"
#include "Global/Utilities/String.h"
#include "HAL/Time.h"
#include "Log/LogEnum.h"
#include "Log/LogDeviceConsole.h"




struct LogEvent
{
	const LogType Type;
	const String Data;

	LogEvent(const LogType NewType, const TChar* NewData):
		Type(NewType),
		Data(NewData)
	{}
};


class LogManager
{
public:
	static LogManager* Get()
	{
		static LogManager Singleton;
		return &Singleton;
	}

	~LogManager();


	void Log(LogType Type, const TChar* ClassName, double Time, const TChar* Format, ...);
	void Flush();
	void Shutdown();



	FORCE_INLINE
	bool IsAutoInsertLineTerminator() { return AutoInsertLineTerminator; }
	FORCE_INLINE
	void SetAutoInsertLineTerminator(bool ToSet) { AutoInsertLineTerminator = ToSet; }
	FORCE_INLINE
	void SetLogTimeType(LogTime ToSet) { TimeType = ToSet; }


private:
	LogManager();

	void UnsynFlushBufferedLogs();
	void SetupOutputDevice();
	void Redirector(LogType Type, const TChar* ClassName, const TChar* TimeString, const double TimeSecond, const TChar* Data);
	String Formatter(LogType Type, const TChar* ClassName, const TChar* TimeString, const double TimeSecond, const TChar* Data);


private:
	bool AutoInsertLineTerminator;

	//Main thread id
	int32 MainThreadID;

	//The time type that output to log file
	//UTC, Local, TimeSinceStart
	LogTime TimeType;

	PlatformCriticalSection CriticalSection;

	//Log data on other thread will be bufferd and output in the main thread
	Array<LogEvent> BufferedLogEvents;

    //Output devices that can be used
	Array<LogDevice*> OutputDevices;


	//Logger output to console
	//Output the debug messages and the messages that failed to output in other devices 
	LogDevice* ConsoleLogger;

};


extern LogManager* gLogger;