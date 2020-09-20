#include "HAL/Platform.h"
#include "CrashReportCore.h"



void CrashReportCore::Init()
{
	Client.Init();
}


void CrashReportCore::Show()
{
	Client.Show();
}


int CrashReportCore::Exit()
{
	return Client.Exit();
}