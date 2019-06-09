/************************************
Log device which output fatal errors to file


*************************************/
#pragma once

#include "Log/LogDevice.h"
#include <fstream>



class LogDeviceFatal : public LogDevice
{
public:
	LogDeviceFatal();

	virtual ~LogDeviceFatal()
	{
		Shutdown();
	}

	void Log(const TChar* Data) override { Serialize(Data); }
	void Serialize(const TChar* Data) override;

	void Flush() override 
	{
		if (Writer.is_open())
			Writer.flush();
	}

	void FlushFatalHist();
	void Shutdown() override;

protected:
	void WriteFatalHistToFile();

protected:
	bool InitFailed;
	std::ofstream Writer;

};

extern LogDeviceFatal gFatalLogger;