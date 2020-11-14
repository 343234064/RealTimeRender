#include "CrashReportCore.h"

#define MINIMAL_PLATFORM_HEADER
#include "HAL/Platform.h"
#include "Global/GlobalType.h"
#include "Global/Utilities/String.h"
#include "Global/Utilities/Tuple.h"
#include "HAL/Process.h"
#include "HAL/Chars.h"
#include "HAL/Time.h"
#include "HAL/Platforms/GenericCrash.h"

static uint64 MonitorProcessId = 0;

static void* MonitorReadPipe = nullptr;

static void* MonitorWritePipe = nullptr;



bool CheckCrashReport(SharedCrashContext& CrashContext, void* ReadPipe)
{
	Array<uint8> Buffer;

	// Is data available on the pipe.
	if (PlatformProcess::ReadPipeToArray(ReadPipe, Buffer))
	{
		// This is to ensure the SharedCrashContext compiled in the monitored process and this process has the same size.
		int32 TotalRead = Buffer.CurrentNum();


		auto CopyFunc = [](const Array<uint8>& SrcData, uint8* DstIt, uint8* DstEndIt)
		{
			int32 CopyCount = Min(SrcData.CurrentNum(), static_cast<int32>(DstEndIt - DstIt));

			PlatformMemory::Memcopy(DstIt, SrcData.Begin(), CopyCount);
			return DstIt + CopyCount;
		};

		// Iterators to defines the boundaries of the destination buffer in memory.
		uint8* SharedCtxIt = reinterpret_cast<uint8*>(&CrashContext);
		uint8* SharedCtxEndIt = SharedCtxIt + sizeof(SharedCrashContext);

		// Copy the data already read and update the destination iterator.
		SharedCtxIt = CopyFunc(Buffer, SharedCtxIt, SharedCtxEndIt);

		// Try to consume all the expected data within a defined period of time.
		double CurrentTime = PlatformTime::Time_Seconds();
		while (SharedCtxIt != SharedCtxEndIt && PlatformTime::Time_Seconds() - CurrentTime <= 3)
		{
			if (PlatformProcess::ReadPipeToArray(ReadPipe, Buffer)) // This is false if no data is available, but the writer may be still be writing.
			{
				TotalRead += Buffer.CurrentNum();
				SharedCtxIt = CopyFunc(Buffer, SharedCtxIt, SharedCtxEndIt); // Copy the data read.
			}
			else
			{
				PlatformProcess::Sleep(0.1); // Give the writer some time.
			}
		}


		return SharedCtxIt == SharedCtxEndIt;
	}

	return false;
}



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

	double LastTime = PlatformTime::Time_Seconds();
	const float IdealFrameRate = 30;
	const float IdealFrameTime = 1.0f / IdealFrameRate;

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

		/* Tuple pairs -> bool IsRunning. int32 ReturnCode*/
		auto GetProcessStatus = [](PlatformProcessHandle& ProcessHandle)->Tuple<bool, int32>
		{
			bool IsRunning = true;
			int32 ReturnCode = -10000;
			if (!ProcessHandle.IsValid())
			{
				IsRunning = false;
			}
			else if (!PlatformProcess::IsProcRunning(ProcessHandle))
			{
				IsRunning = false;
				int32 ReturnCodeTmp = 0;
				if (PlatformProcess::GetProcReturnCode(ProcessHandle, &ReturnCodeTmp))
				{
					ReturnCode = ReturnCodeTmp;
				}
			}

			return MakeTuple(IsRunning, ReturnCode);
		};

		Tuple<bool, int32> ProcessStatus = GetProcessStatus(MonitorProcess);
		while (ProcessStatus.Get<0>())
		{
			const double CurrentTime = PlatformTime::Time_Seconds();

			if (MonitorWritePipe && MonitorReadPipe)
			{
				SharedCrashContext CrashContext;
				if (CheckCrashReport(CrashContext, MonitorReadPipe))
				{
					Client.SetTextBrowser(QString::fromWCharArray(CrashContext.ErrorMessage));
				}
			}

			float IdealTime = PlatformTime::Time_Seconds() - LastTime;
			PlatformProcess::Sleep(Max<float>(0.0f, IdealFrameTime - IdealTime));
			if (IdealTime >= 1)
			{
				ProcessStatus = GetProcessStatus(MonitorProcess);
			}

			LastTime = CurrentTime;
		}

		PlatformProcess::CloseProc(MonitorProcess);



		
	}

	Client.Show();
}


int CrashReportCore::Exit()
{
	return Client.Exit();
}