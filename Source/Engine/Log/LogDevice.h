/************************************
LogDevice base interface


*************************************/
#pragma once

#include "Global/GlobalType.h"






class LogDevice
{
public:
	LogDevice() {}
	virtual ~LogDevice() {}

	LogDevice(const LogDevice& Other) = default;
	LogDevice(LogDevice&& Other) = default;
	LogDevice& operator=(const LogDevice& Other) = default;
	LogDevice& operator=(LogDevice&& Other) = default;


	virtual void Serialize(const TChar* Data) = 0;
	virtual void Flush() {}
	virtual void Shutdown() {}

	virtual void Logf(const TChar* Format, ...);
	virtual void Log(const TChar* Data) { Serialize(Data); }


protected:
	

};