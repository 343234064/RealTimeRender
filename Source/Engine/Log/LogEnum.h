#pragma once



enum LogType
{
	//Print to IDE and Console, only in debug mode
	Debug = 0,

	//Print to screen and file
	Info,

	//Print to screen and file
	Warn,

	//Print to screen and file
	Error,

    //Print to file and IDE/Console
	Fatal

};



enum LogTime 
{
	Local = 0,

	UTC,

	//The time since the app start in seconds
	SinceStart

};
