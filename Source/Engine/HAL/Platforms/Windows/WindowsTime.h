#pragma once

#include "Global/GlobalType.h"
#include "Global/Utilities/String.h"



struct CPUTime
{
	CPUTime(float TimeUtilization, float TimeUtilizationCores):
		CPUTimeUtilization(TimeUtilization),
		CPUTimeUtilizationCores(TimeUtilizationCores)
	{}

	CPUTime& operator+=(const CPUTime& Other)
	{
		CPUTimeUtilization += Other.CPUTimeUtilization;
		CPUTimeUtilizationCores += Other.CPUTimeUtilizationCores;
	}


	//Cpu utilization for last interval, in percentage
	float CPUTimeUtilization;

	//CPUTimeUtilizationCores = CPUTimeUtilization * CPU_Core_Num
	float CPUTimeUtilizationCores;

};



class WindowsTime
{
public:
	//Called before Main() function start
	static double InitTime();


	static TChar* DateToStr(TChar* Dest, int32 DestSize)
	{
		int32 Year, Month, Week, Day, Hour, Min, Sec, MicroSec;

		WindowsTime::SystemTime(Year, Month, Week, Day, Hour, Min, Sec, MicroSec);
		String::Snprintf(Dest, DestSize, TEXTS("%02d/%02d/%02d"), Month, Day, Year % 100);
		return Dest;
	}

	static TChar* TimeToStr(TChar* Dest, int32 DestSize)
	{
		int32 Year, Month, Week, Day, Hour, Min, Sec, MicroSec;

		WindowsTime::SystemTime(Year, Month, Week, Day, Hour, Min, Sec, MicroSec);
		String::Snprintf(Dest, DestSize, TEXTS("%02d:%02d:%02d"), Hour, Min, Sec);
		return Dest;
	}

	static String SecondToFormatTime(float Seconds);


	//Return to float may cause data loss 
	FORCE_INLINE
	static double Time_Seconds()
	{
		LARGE_INTEGER Cycles;
		::QueryPerformanceCounter(&Cycles);

		return Cycles.QuadPart * SecondsPerCycle;//+16777216.0;
	}

	FORCE_INLINE
	static uint32 Time_Cycles()
	{
		LARGE_INTEGER Cycles;
		::QueryPerformanceCounter(&Cycles);

		return (uint32)Cycles.QuadPart;
	}


	static void SystemTime(int32& Year, int32& Month, int32& Week, int32& Day, int32& Hour, int32& Min, int32& Sec, int32& MicroSec)
	{
		SYSTEMTIME SystemTime;
		::GetLocalTime(&SystemTime);
		
		Year = SystemTime.wYear;
		Month = SystemTime.wMonth;
		Week = SystemTime.wDayOfWeek;
		Day = SystemTime.wDay;
		Hour = SystemTime.wHour;
		Min = SystemTime.wMinute;
		Sec = SystemTime.wSecond;
		MicroSec = SystemTime.wMilliseconds;
	}

	static TChar* SystemTimeToStr(TChar* Dest, int32 DestSize)
	{
		int32 Year, Month, Week, Day, Hour, Min, Sec, MicroSec;

		WindowsTime::SystemTime(Year, Month, Week, Day, Hour, Min, Sec, MicroSec);
		String::Snprintf(Dest, DestSize, TEXTS("%02d/%02d/%02d %02d:%02d:%02d.%02d"), Year, Month, Day, Hour, Min, Sec, MicroSec);
		return Dest;
	}

	static void UTCTime(int32& Year, int32& Month, int32& Week, int32& Day, int32& Hour, int32& Min, int32& Sec, int32& MicroSec)
	{
		SYSTEMTIME SystemTime;
		::GetSystemTime(&SystemTime);

		Year = SystemTime.wYear;
		Month = SystemTime.wMonth;
		Week = SystemTime.wDayOfWeek;
		Day = SystemTime.wDay;
		Hour = SystemTime.wHour;
		Min = SystemTime.wMinute;
		Sec = SystemTime.wSecond;
		MicroSec = SystemTime.wMilliseconds;
	}

	static TChar* UTCTimeToStr(TChar* Dest, int32 DestSize)
	{
		int32 Year, Month, Week, Day, Hour, Min, Sec, MicroSec;

		WindowsTime::UTCTime(Year, Month, Week, Day, Hour, Min, Sec, MicroSec);
		String::Snprintf(Dest, DestSize, TEXTS("%02d/%02d/%02d %02d:%02d:%02d.%02d"), Year, Month, Day, Hour, Min, Sec, MicroSec);
		return Dest;
	}


	static void UpdateCPUTime();

	static CPUTime GetCPUTime()
	{
		return CPUTime((float)(CPUTimeUtilizationCores / Platform::NumberOfCores()), (float)CPUTimeUtilizationCores);
	}


protected:
	static double SecondsPerCycle;
	static double CPUTimeUtilizationCores;

};


typedef WindowsTime PlatformTime;

