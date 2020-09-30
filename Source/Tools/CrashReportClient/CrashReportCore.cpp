#include "CrashReportCore.h"

#define MINIMAL_PLATFORM_HEADER
#include "HAL/Platform.h"
#include "Global/GlobalType.h"
#include "HAL/Chars.h"


static uint64 MonitorProcessId = 0;

static void* MonitorReadPipe = nullptr;

static void* MonitorWritePipe = nullptr;




void CrashReportCore::Run()
{
	Client.Init();

	Client.SetBasicLabelText(QString::fromWCharArray(TEXTS("Nothing happens~~")));
	Client.SetTextBrowser(QString::fromWCharArray(::GetCommandLineW()));
	
	Client.Show();
}


int CrashReportCore::Exit()
{
	return Client.Exit();
}