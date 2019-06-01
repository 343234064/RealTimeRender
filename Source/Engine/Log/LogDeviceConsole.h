/************************************
Log device which output to IDE/Console window
Only avaliable while debug mode is on

*************************************/
#pragma once

#include "Log/LogDevice.h"

#if SHOW_DEBUG_CONSOLE || FOR_TEST_CPP
#include <locale.h>
#endif


class LogDeviceConsole : public LogDevice
{
public:
	LogDeviceConsole()
	{
#if SHOW_DEBUG_CONSOLE || FOR_TEST_CPP
		ANSICHAR* locale = setlocale(LC_ALL, "");
		setlocale(LC_ALL, locale);

		//Cancel the stdout buffer to immediately output the debug message 
		setvbuf(stdout, NULL, _IONBF, 0);
#endif
	}

	virtual ~LogDeviceConsole()
	{
		Shutdown();
	}

	FORCE_INLINE
	void Serialize(const TChar* Data) override
	{
		//Output to the console window
#if SHOW_DEBUG_CONSOLE || FOR_TEST_CPP
		wprintf(Data);//2 byte width
#endif
		//Output to the IDE
		Platform::LocalPrintW(Data);
	}

	void Flush() override
	{
		//fflush(stdout);
	}

	void Shutdown() override
	{
		Flush();
	}



protected:


};
