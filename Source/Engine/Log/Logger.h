/************************************
Logger base interface


*************************************/
#pragma once

#include "Global/GlobalType.h"
#include "Log/LogEnum.h"





class Logger
{
public:
	Logger():
		AutoInsertLineTerminator(true)
	{}
	virtual ~Logger() {}

	Logger(const Logger& Other) = default;
	Logger(Logger&& Other) = default;
	Logger& operator=(const Logger& Other) = default;
	Logger& operator=(Logger&& Other) = default;



	virtual void Serialize(LogVerbosity Verbosity, const TChar* Data) = 0;
	virtual void Flush() {}
	virtual void Shutdown() {}


	void Logf(LogVerbosity Verbosity, const TChar* Format, ...);
    void Log(LogVerbosity Verbosity, const TChar* Data) { Serialize(Verbosity, Data); }

	FORCE_INLINE
	bool IsAutoInsertLineTerminator() { return AutoInsertLineTerminator; }
	FORCE_INLINE
	void SetAutoInsertLineTerminator(bool ToSet) { AutoInsertLineTerminator = ToSet; }


protected:
	bool AutoInsertLineTerminator;

};