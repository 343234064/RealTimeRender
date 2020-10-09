#include "CrashReportCore.h"

#define MINIMAL_PLATFORM_HEADER
#include "HAL/Platform.h"
#include "Global/GlobalType.h"
#include "HAL/Process.h"
#include "HAL/Chars.h"
#include "Global/Utilities/String.h"
#include "Global/Utilities/Tuple.h"


static uint64 MonitorProcessId = 0;

static void* MonitorReadPipe = nullptr;

static void* MonitorWritePipe = nullptr;




void CrashReportCore::Run()
{
	Client.Init();


	if (ArgCount < 4)
	{
		if (ArgCount <= 1)
		{
			Client.SetBasicLabelText(QString::fromWCharArray(TEXTS("Nothing happens~~")));
		}
		else
		{
			TChar Message[64];
			String::Snprintf(Message, 64, TEXTS("Launch error with only %d args"), ArgCount - 1, ArgValues);
			Client.SetBasicLabelText(QString::fromWCharArray(Message));

			ANSICHAR Args[128] = "Expected arguments:\n-READ=%0u -WRITE=%0u -MONITORID=%u\nActual arguments:\n";
			for (int32 ArgIdx = 1; ArgIdx < ArgCount; ArgIdx++)
			{
				PlatformChars::Strcat(Args, 128, ArgValues[ArgIdx]);
				PlatformChars::Strcat(Args, 128, " ");
			}
			Client.SetTextBrowser(QString(Args));
		}
	}
	else
	{
		ANSICHAR ReadPipeToken[] = "-READ=";
		const ANSICHAR* ReadPipeValue = PlatformChars::Stristr(ArgValues[1], ReadPipeToken);
		if (ReadPipeValue != nullptr)
		{
			MonitorReadPipe = (void*)PlatformChars::Atoi64(ReadPipeValue + ARRAY_SIZE(ReadPipeToken) - 1);
		}
		ANSICHAR WritePipeToken[] = "-WRITE=";
		const ANSICHAR* WritePipeValue = PlatformChars::Stristr(ArgValues[2], WritePipeToken);
		if (WritePipeValue != nullptr)
		{
			MonitorWritePipe = (void*)PlatformChars::Atoi64(WritePipeValue + ARRAY_SIZE(WritePipeToken) - 1);
		}
		ANSICHAR MonitorIDToken[] = "-MONITORID=";
		const ANSICHAR* MonitorIDValue = PlatformChars::Stristr(ArgValues[3], MonitorIDToken);
		if (MonitorIDValue != nullptr)
		{
			MonitorProcessId = PlatformChars::Atoi64(MonitorIDValue + ARRAY_SIZE(MonitorIDToken) - 1);
		}

		Client.SetBasicLabelText(QString::fromWCharArray(::GetCommandLineW()));
		TChar ClientArgs[512];
		const TChar* CommandLine = TEXTS("-READ=%0u -WRITE=%0u -MONITORID=%u");
		String::Snprintf(ClientArgs, 512, CommandLine, MonitorReadPipe, MonitorWritePipe, MonitorProcessId);
		Client.SetTextBrowser(QString::fromWCharArray(ClientArgs));
	}


	if (MonitorProcessId != 0)
	{
		PlatformProcessHandle MonitorProcess;
		for (int32 Try = 0; Try < 5; Try++)
		{
#if PLATFORM_WINDOWS
			MonitorProcess = PlatformProcessHandle(::OpenProcess(PROCESS_DUP_HANDLE | PROCESS_QUERY_LIMITED_INFORMATION | PROCESS_TERMINATE | SYNCHRONIZE, 0, MonitorProcessId));
#else
			MonitorProcess = PlatformProcess::OpenProcess(MonitorProcessId);
#endif
			if (MonitorProcess.IsValid())
			{
				break;
			}
			
			PlatformProcess::Sleep(1);
		}



		PlatformProcess::CloseProc(MonitorProcess);
	}

	Client.Show();
}


int CrashReportCore::Exit()
{
	return Client.Exit();
}