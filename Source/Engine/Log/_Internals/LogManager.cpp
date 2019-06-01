#include "Global/Utilities/Assertion.h"
#include "Global/Utilities/Misc.h"
#include "Log/LogManager.h"
#include "Log/LogDeviceFile.h"
#include "Log/LogDeviceEditor.h"





LogManager::LogManager():
	AutoInsertLineTerminator(true),
	MainThreadID(Platform::GetCurrentThreadId()),
	TimeType(LogTime::Local), 
	ConsoleLogger(nullptr)
{

}


LogManager::~LogManager()
{
	CHECK(BufferedLogEvents.CurrentNum() == 0);
	CHECK(OutputDevices.CurrentNum() == 0);
	CHECK(ConsoleLogger == nullptr);

}


void LogManager::Log(LogType Type, const TChar* ClassName, double Time, const TChar* Format, ...)
{
	//Get the time as early as possible
	Time = (Time < 0.0) ? PlatformTime::Time_Seconds() - gStartTime : Time;

	const int32 TimeLength = 50;
	TChar TimeBuffer[TimeLength];
	TChar* TimeString = nullptr;

	switch (TimeType)
	{
	case LogTime::Local:
	{
		PlatformTime::SystemTimeToStr(TimeBuffer, TimeLength);
		TimeString = TimeBuffer;
		break;
	}
	case LogTime::UTC:
	{
		PlatformTime::UTCTimeToStr(TimeBuffer, TimeLength);
		TimeString = TimeBuffer;
		break;
	}
	default:
		break;
	}

	SRINTF_VARARGS(Redirector(Type, ClassName, TimeString, Time, BufferPtr));
	
}


void LogManager::SetupOutputDevice()
{
	LogDevice* FileLogger = new LogDeviceFile(TEXTS("Log.log"), true);
	if (FileLogger != nullptr) OutputDevices.Add(FileLogger);

	//EditorLogger = new LogDeviceEditor();
	//if (EditorLogger != nullptr) OutputDevices.Add(EditorLogger);

	CHECK(OutputDevices.CurrentNum() != 0);

}

void LogManager::Redirector(LogType Type, const TChar* ClassName, const TChar* TimeString, const double TimeSecond, const TChar* Data)
{
	    String ToWrite = Formatter(Type, ClassName, TimeString, TimeSecond, Data);

		LockGuard<PlatformCriticalSection> Lock(CriticalSection);

		//Output debug log immediately
#ifdef _DEBUG
		if (Type == LogType::Debug)
		{
			if (!ConsoleLogger)
			{
				ConsoleLogger = new LogDeviceConsole();
				CHECK(ConsoleLogger != nullptr);
			}

			if (ConsoleLogger)
				ConsoleLogger->Log(Data);

			return;
		}
#endif

		if (Platform::GetCurrentThreadId() != MainThreadID)
		{
			//Cache the log message if current thread is not main thread or there is still no output device 
			new(BufferedLogEvents) LogEvent(Type, Data);
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
				for (int32 LogIndex = 0; LogIndex < BufferedLogEvents.CurrentNum(); LogIndex++)
				{
					for (int32 DeviceIndex = 0; DeviceIndex < OutputDevices.CurrentNum(); DeviceIndex++)
					{
						OutputDevices[DeviceIndex]->Log(*(BufferedLogEvents[LogIndex].Data));
					}
					
				}
				BufferedLogEvents.ClearElements();

				//Output current log message
				for (int32 DeviceIndex = 0; DeviceIndex < OutputDevices.CurrentNum(); DeviceIndex++)
				{
					OutputDevices[DeviceIndex]->Log(Data);
				}
			}
			else
			{
				//Cache the log message if current thread is not main thread or there is still no output device 
				new(BufferedLogEvents) LogEvent(Type, Data);
			}

		}


}


void LogManager::UnsynFlushBufferedLogs()
{
	if (BufferedLogEvents.CurrentNum())
	{
		if (OutputDevices.CurrentNum() == 0)
		{
			SetupOutputDevice();
		}

		if (OutputDevices.CurrentNum() != 0)
		{
			//Flush buffered log messages
			for (int32 LogIndex = 0; LogIndex < BufferedLogEvents.CurrentNum(); LogIndex++)
			{
				for (int32 DeviceIndex = 0; DeviceIndex < OutputDevices.CurrentNum(); DeviceIndex++)
				{
					OutputDevices[DeviceIndex]->Log(*(BufferedLogEvents[LogIndex].Data));
				}

			}
		}
		else
		{
			if (!ConsoleLogger)
			{
				ConsoleLogger = new LogDeviceConsole();
				CHECK(ConsoleLogger != nullptr);
			}

			if (ConsoleLogger)
			{
				for (int32 LogIndex = 0; LogIndex < BufferedLogEvents.CurrentNum(); LogIndex++)
				{
					ConsoleLogger->Log(*(BufferedLogEvents[LogIndex].Data));
				}
			}

		}
		BufferedLogEvents.ClearElements();
	}
}


void LogManager::Flush()
{
	LockGuard<PlatformCriticalSection> Lock(CriticalSection);

	if (Platform::GetCurrentThreadId() == MainThreadID)
	{
		UnsynFlushBufferedLogs();

		for (int32 DeviceIndex = 0; DeviceIndex < OutputDevices.CurrentNum(); DeviceIndex++)
		{
			OutputDevices[DeviceIndex]->Flush();
		}
	}

}



void LogManager::Shutdown()
{
	CHECK(Platform::GetCurrentThreadId() == MainThreadID);

	LockGuard<PlatformCriticalSection> Lock(CriticalSection);

	UnsynFlushBufferedLogs();
	BufferedLogEvents.Empty();

	for (int32 DeviceIndex = 0; DeviceIndex < OutputDevices.CurrentNum(); DeviceIndex++)
	{
		OutputDevices[DeviceIndex]->Flush();
		OutputDevices[DeviceIndex]->Shutdown();
	}
	OutputDevices.Empty();

	if (ConsoleLogger)
	{
		delete ConsoleLogger;
		ConsoleLogger = nullptr;
	}

}


String LogManager::Formatter(LogType Type, const TChar* ClassName, const TChar* TimeString, const double TimeSecond, const TChar* Data)
{
	//Adding prefix
	const TChar* Terminal = LINE_TERMINATOR;
	
	String ToWrite;

	if(TimeString)
		ToWrite = String::Sprintf(TEXTS("[%s][%3llu f]"), TimeString, gFrameCounter % 1000);
	else
		ToWrite = String::Sprintf(TEXTS("[%07.2f s][%3llu f]"), TimeSecond, gFrameCounter % 1000);


	switch (Type)
	{
	case LogType::Debug:
		ToWrite += TEXTS("[DEBUG]");break;
	case LogType::Info:
		ToWrite += TEXTS("[INFO]");break;
	case LogType::Warn:
		ToWrite += TEXTS("[WARNING]");break;
	case LogType::Error:
		ToWrite += TEXTS("[ERROR]");break;
	case LogType::Fatal:
		ToWrite += TEXTS("[FATAL]");break;
	default:
		break;
	}

	ToWrite += ClassName;
	ToWrite += TEXTS(": ");
	ToWrite += Data;

	if (AutoInsertLineTerminator)
	{
	    ToWrite += Terminal;
	}

	return ToWrite;
}


