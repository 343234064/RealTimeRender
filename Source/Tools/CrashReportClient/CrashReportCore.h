#pragma once

#include "CrashReportClient.h"

class CrashReportCore
{

public:
	CrashReportCore(int Argc, char* Argv[]):
		Client(Argc, Argv),
		ArgCount(Argc),
		ArgValues(Argv)
	{}
	~CrashReportCore() {}

	void Init();

	void Show();

	int Exit();
	

private:
	CrashReportClient Client;
	int ArgCount;
	char** ArgValues;

};