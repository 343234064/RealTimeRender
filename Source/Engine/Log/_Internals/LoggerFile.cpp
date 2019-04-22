#include "Log/LoggerFile.h"



LoggerFile::LoggerFile(const TChar* OutputFileName, bool AppendIfExist):
	AppendIfExist(true),
    FileOpened(false)
{
	if (OutputFileName != nullptr)
	{
		PlatformChars::Strncpy(FileName, OutputFileName, ARRAY_SIZE(FileName));
	}
	else
	{
		FileName[0] = 0;
	}
}


LoggerFile::~LoggerFile()
{
	Shutdown();
}



void LoggerFile::SetFileName(const TChar* OutputFileName)
{
	Shutdown();

	PlatformChars::Strncpy(FileName, OutputFileName, ARRAY_SIZE(FileName));
}


void LoggerFile::Shutdown()
{


	FileName[0] = 0;
	FileOpened = false;
}


void LoggerFile::Flush()
{

}


void LoggerFile::Serialize(LogVerbosity Verbosity, const TChar* Data)
{

}