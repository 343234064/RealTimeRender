/************************************
Logger which output to file


*************************************/
#pragma once

#include "Log/Logger.h"



class LoggerFile : public Logger
{
public:
	LoggerFile(const TChar* OutputFileName = nullptr, bool AppendIfExist = false);
	virtual ~LoggerFile();


	virtual void Serialize(LogVerbosity Verbosity, const TChar* Data) override;
	virtual void Flush() override;
	virtual void Shutdown() override;


	void SetFileName(const TChar* OutputFileName); //Set file name will shutdown the file writer 
	const TChar* GetFileName() const { return FileName; }

protected:
	TChar FileName[1024];
	bool AppendIfExist;
	bool FileOpened;

};

