#pragma once



enum LogVerbosity
{
	//Prints error to screen and file
	Error = 0,

	//Prints warning to screen and file
	Warning,

	//Prints message to screen
	ToScreen,

	//Prints message to file
	ToFile,

	//Prints message to screen and file
	ToAll
};



enum LogTime 
{
	Local = 0,

	UTC,

	//The time since the app start in seconds
	SinceStart
};
