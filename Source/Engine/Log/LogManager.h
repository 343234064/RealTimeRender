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
	typedef Array<LogDevice*> OutputDevicesArray;

	static LogManager& Get()
	{
		static LogManager Singleton;
		return Singleton;
	}

	~LogManager();


	void Redirector(LogType Type, const TChar* ClassName, const TChar* TimeString, const int32 Line, const double TimeSecond, const TChar* Data);
	void Flush();
	void FlushThreadedLogs();
	void Shutdown();


	FORCE_INLINE
	bool IsAutoInsertLineTerminator() { return AutoInsertLineTerminator; }
	FORCE_INLINE
	void SetAutoInsertLineTerminator(bool ToSet) { AutoInsertLineTerminator = ToSet; }

private:
	LogManager();

	void SetupOutputDevice();
	void UnsynFlushBufferedLogs(OutputDevicesArray& InOutputDevices);
	
	String Formatter(LogType Type, const TChar* ClassName, const TChar* TimeString, const int32 Line, const double TimeSecond, const TChar* Data);

	void LockOutputDevices(OutputDevicesArray& LocalOutputDevices);
	void UnlockOutputDevices();

private:
	bool AutoInsertLineTerminator;

	//Main thread id
	int32 MainThreadID;

	//Log data on other thread will be bufferd and output in the main thread
	Array<LogEvent> BufferedLogEvents;
	PlatformCriticalSection BufferedLogEventsMutex;

	//Output devices that can be used
	OutputDevicesArray OutputDevices;
	AtomicCounter<int32> OutputDevicesLockCounter;
	PlatformCriticalSection OutputDevicesMutex;


};


extern LogManager& gLogger;