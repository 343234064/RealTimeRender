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
	static LogManager& Get()
	{
		static LogManager Singleton;
		return Singleton;
	}

	~LogManager();


	void Redirector(LogType Type, const TChar* ClassName, const TChar* TimeString, const double TimeSecond, const TChar* Data);
	void Flush();
	void Shutdown();


	FORCE_INLINE
	bool IsAutoInsertLineTerminator() { return AutoInsertLineTerminator; }
	FORCE_INLINE
	void SetAutoInsertLineTerminator(bool ToSet) { AutoInsertLineTerminator = ToSet; }

private:
	LogManager();

	void UnsynFlushBufferedLogs();
	void SetupOutputDevice();
	String Formatter(LogType Type, const TChar* ClassName, const TChar* TimeString, const double TimeSecond, const TChar* Data);


private:
	bool AutoInsertLineTerminator;

	//Main thread id
	int32 MainThreadID;


	PlatformCriticalSection CriticalSection;

	//Log data on other thread will be bufferd and output in the main thread
	Array<LogEvent> BufferedLogEvents;

    //Output devices that can be used
	Array<LogDevice*> OutputDevices;


	//Logger output to console
	//Output the debug messages and the messages that failed to output in other devices 
	LogDevice* ConsoleLogger;

};


extern LogManager& gLogger;