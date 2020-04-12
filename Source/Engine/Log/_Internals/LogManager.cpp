#include "Global/Utilities/Assertion.h"
#include "Global/Utilities/Misc.h"
#include "Log/LogManager.h"
#include "Log/LogDeviceFile.h"
#include "Log/LogDeviceEditor.h"





LogManager::LogManager():
	AutoInsertLineTerminator(true),
	MainThreadID(Platform::GetCurrentThreadId()),
	ConsoleLogger(nullptr)
{

}


LogManager::~LogManager()
{
	CHECK(BufferedLogEvents.CurrentNum() == 0);
	CHECK(OutputDevices.CurrentNum() == 0);
	CHECK(ConsoleLogger == nullptr);

}




void LogManager::SetupOutputDevice()
{
	LogDevice* FileLogger = new LogDeviceFile(TEXTS("Log.log"), true);
	if (FileLogger != nullptr) OutputDevices.Add(FileLogger);

	//EditorLogger = new LogDeviceEditor();
	//if (EditorLogger != nullptr) OutputDevices.Add(EditorLogger);

	CHECK(OutputDevices.CurrentNum() != 0);

}

void LogManager::Redirector(LogType Type, const TChar* ClassName, const TChar* TimeString, const int32 Line, const double TimeSecond, const TChar* Data)
{
		String ToWrite = Formatter(Type, ClassName, TimeString, Line, TimeSecond, Data);

		LockGuard<PlatformCriticalSection> Lock(CriticalSection);

		//Output debug log immediately
#ifdef _DEBUG		
		if (!ConsoleLogger)
		{
			ConsoleLogger = new LogDeviceConsole();
			CHECK(ConsoleLogger != nullptr);
		}

		if (ConsoleLogger)
			ConsoleLogger->Log(*ToWrite);
		
		if (Type == LogType::Debug)
		{
			return;
		}
#endif

		if (Platform::GetCurrentThreadId() != MainThreadID)
		{
			//Cache the log message if current thread is not main thread or there is still no output device 
			//new(BufferedLogEvents) LogEvent(Type, Data);
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
					OutputDevices[DeviceIndex]->Log(*ToWrite);
				}
			}
			else
			{
				//Cache the log message if there is still no output device 
				BufferedLogEvents.Add(LogEvent(Type, *ToWrite));
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


