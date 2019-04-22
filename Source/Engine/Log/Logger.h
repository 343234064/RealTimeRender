/************************************
Logger base interface


*************************************/
#pragma once

#include "Global/GlobalType.h"
#include "Global/Utilities/String.h"

enum LogVerbosity : uint8
{
	None = 0,

	//Prints error to screen and file
	Error,

	//Prints warning to screen and file
	Warning,

	//Prints message to screen
	ToScreen,

	//Prints message to file
	ToFile,

	//Prints message to screen and file
	ToAll
};



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
	virtual bool IsThreadSafe() const { return false; }

	void Logf(LogVerbosity Verbosity, const TChar* Format, ...);
	void Log(LogVerbosity Verbosity, const TChar* Data) { Serialize(Verbosity, Data); }
	void Log(LogVerbosity Verbosity, const String& Data) { Serialize(Verbosity, *Data); }

	FORCE_INLINE
	bool IsAutoInsertLineTerminator() { return AutoInsertLineTerminator; }
	FORCE_INLINE
	void SetAutoInsertLineTerminator(bool ToSet) { AutoInsertLineTerminator = ToSet; }


protected:
	bool AutoInsertLineTerminator;

};