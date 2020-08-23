#include "Global/Utilities/Assertion.h"
#include "Global/Utilities/Misc.h"
#include "Log/LogManager.h"
#include "Log/LogDeviceFile.h"





LogManager::LogManager():
	AutoInsertLineTerminator(true),
	MainThreadID(Platform::GetCurrentThreadId())
{

}

LogManager::~LogManager()
{
	CHECK(BufferedLogEvents.CurrentNum() == 0);
	CHECK(OutputDevices.CurrentNum() == 0);
}




void LogManager::SetupOutputDevice()
{
	if (OutputDevicesLockCounter.GetCounter() == 0) 
	{
		LogDevice* ConsoleLogger = new LogDeviceConsole();
		CHECK(ConsoleLogger != nullptr);
		OutputDevices.Add(ConsoleLogger);

		LogDevice* FileLogger = new LogDeviceFile(TEXTS("Log.log"), true);
		CHECK(FileLogger != nullptr);
		OutputDevices.Add(FileLogger);

		CHECK(OutputDevices.CurrentNum() != 0);
	}

}

void LogManager::Redirector(LogType Type, const TChar* ClassName, const TChar* TimeString, const int32 Line, const double TimeSecond, const TChar* Data)
{
		String ToWrite = Formatter(Type, ClassName, TimeString, Line, TimeSecond, Data);

		if (Platform::GetCurrentThreadId() != MainThreadID)
		{
			//Cache the log message if current thread is not main thread or there is still no output device 
			//new(BufferedLogEvents) LogEvent(Type, Data);
			LockGuard<PlatformCriticalSection> Lock(BufferedLogEventsMutex);
			BufferedLogEvents.Add(LogEvent(Type, *ToWrite));
		}
		else
		{
			if (OutputDevices.CurrentNum() == 0)
			{
				SetupOutputDevice();
			}

			if (OutputDevices.CurrentNum() != 0)
			{
				//Flush buffered log messages
				OutputDevicesArray LocalOutputDevices;
				LockOutputDevices(LocalOutputDevices);

				UnsynFlushBufferedLogs(LocalOutputDevices);

				//Output current log message
				for (int32 DeviceIndex = 0; DeviceIndex < LocalOutputDevices.CurrentNum(); DeviceIndex++)
				{
					LocalOutputDevices[DeviceIndex]->Log(*ToWrite);
				}

				UnlockOutputDevices();
			}
			else
			{
				//Cache the log message if there is still no output device 
				BufferedLogEvents.Add(LogEvent(Type, *ToWrite));
			}

		}


}


void LogManager::UnsynFlushBufferedLogs(OutputDevicesArray& InOutputDevices)
{
	if (BufferedLogEvents.CurrentNum())
	{
		Array<LogEvent> LocalBufferedLogEvents;
		{
			LockGuard<PlatformCriticalSection> Lock(BufferedLogEventsMutex);

			for (int32 i = 0; i < BufferedLogEvents.CurrentNum(); i++)
			{
				LocalBufferedLogEvents.Add(LogEvent(BufferedLogEvents[i].Type, *(BufferedLogEvents[i].Data)));
			}
			BufferedLogEvents.ClearElements();
		}

		if (InOutputDevices.CurrentNum() != 0)
		{

			//Flush buffered log messages
			for (int32 LogIndex = 0; LogIndex < LocalBufferedLogEvents.CurrentNum(); LogIndex++)
			{
				for (int32 DeviceIndex = 0; DeviceIndex < InOutputDevices.CurrentNum(); DeviceIndex++)
				{
					InOutputDevices[DeviceIndex]->Log(*(LocalBufferedLogEvents[LogIndex].Data));
				}

			}
		}

		
	}
}


void LogManager::Flush()
{
	OutputDevicesArray LocalOutputDevices;
	LockOutputDevices(LocalOutputDevices);

	if (Platform::GetCurrentThreadId() == MainThreadID)
	{
		UnsynFlushBufferedLogs(LocalOutputDevices);

		for (int32 DeviceIndex = 0; DeviceIndex < LocalOutputDevices.CurrentNum(); DeviceIndex++)
		{
			LocalOutputDevices[DeviceIndex]->Flush();
		}
	}
	UnlockOutputDevices();
}

void LogManager::FlushThreadedLogs()
{
	OutputDevicesArray LocalOutputDevices;
	LockOutputDevices(LocalOutputDevices);

	UnsynFlushBufferedLogs(LocalOutputDevices);

	for (int32 DeviceIndex = 0; DeviceIndex < LocalOutputDevices.CurrentNum(); DeviceIndex++)
	{
		LocalOutputDevices[DeviceIndex]->Flush();
	}

	UnlockOutputDevices();
}


void LogManager::Shutdown()
{
	CHECK(Platform::GetCurrentThreadId() == MainThreadID);

	OutputDevicesArray LocalOutputDevices;
	{
		LockGuard<PlatformCriticalSection> Lock(OutputDevicesMutex);
		LockOutputDevices(LocalOutputDevices);
		OutputDevices.Empty();
	}

	UnsynFlushBufferedLogs(LocalOutputDevices);

	for (int32 DeviceIndex = 0; DeviceIndex < LocalOutputDevices.CurrentNum(); DeviceIndex++)
	{
		LocalOutputDevices[DeviceIndex]->Flush();
		LocalOutputDevices[DeviceIndex]->Shutdown();
	}

	UnlockOutputDevices();
}


String LogManager::Formatter(LogType Type, const TChar* ClassName, const TChar* TimeString, const int32 Line, const double TimeSecond, const TChar* Data)
{
	//Adding prefix
	const TChar* Terminal = LINE_TERMINATOR;
	
	String ToWrite;

	if(TimeString)
		ToWrite = String::Sprintf(TEXTS("[%s][%3lluf]"), TimeString, gFrameCounter % 1000);
	else
		ToWrite = String::Sprintf(TEXTS("[%07.2fs][%3lluf]"), TimeSecond, gFrameCounter % 1000);


	switch (Type)
	{
	case LogType::Debug:
		ToWrite += TEXTS("[**DEBUG**]");break;
	case LogType::Info:
		ToWrite += TEXTS("[**INFO**]");break;
	case LogType::Warn:
		ToWrite += TEXTS("[**WARN**]");break;
	case LogType::Error:
		ToWrite += TEXTS("[**ERROR**]");break;
	default:
		break;
	}

	ToWrite += String::Sprintf(TEXTS("%s, %d"), ClassName, Line);
	ToWrite += TEXTS(": \n");
	ToWrite += Data;

	if (AutoInsertLineTerminator)
	{
	    ToWrite += Terminal;
	}

	return ToWrite;
}

void LogManager::LockOutputDevices(OutputDevicesArray& OutLocalOutputDevices)
{
	LockGuard<PlatformCriticalSection> Lock(OutputDevicesMutex);
	OutputDevicesLockCounter.Increment();

	OutLocalOutputDevices.Append(OutputDevices);
}

void LogManager::UnlockOutputDevices()
{
	LockGuard<PlatformCriticalSection> Lock(OutputDevicesMutex);
	int value = OutputDevicesLockCounter.Decrement();

	CHECK(value >= 0);
}