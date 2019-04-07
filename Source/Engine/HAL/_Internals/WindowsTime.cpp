#include "HAL/Platforms/Windows/WindowsPlatform.h"
#include "HAL/Platforms/Windows/WindowsTime.h"


double WindowsTime::SecondsPerCycle = 0.0;
double WindowsTime::CPUTimeUtilizationCores = 0.0;


FORCE_INLINE
double FileTimeToSeconds(const FILETIME& FileTime)
{
	const uint64 Ticks = (uint64(FileTime.dwHighDateTime) << 32) + FileTime.dwLowDateTime;
	//Tick 10000000 times per second
	return double(Ticks) / 10000000.0;
}


double WindowsTime::InitTime()
{
	LARGE_INTEGER Frequency;
	::QueryPerformanceFrequency(&Frequency);

	SecondsPerCycle = 1.0 / Frequency.QuadPart;

	static const float CPUTimeUpdateInterval = 1.0f / 4.0f;
	//Ticker add CPUTimeUpdate

	return WindowsTime::Time_Seconds();
}


String WindowsTime::SecondToFormatTime(float Seconds)
{
	if (Seconds < 1.0)
	{
		return String::Sprintf(TEXTS("%d ms"), Platform::TruncToInt(Seconds * 1000.0f));
	}
	else if (Seconds < 10.0)
	{
		int32 Sec = Platform::TruncToInt(Seconds);
		int32 MicroSec = Platform::TruncToInt(Seconds * 1000.0f) - Sec * 1000;
		return String::Sprintf(TEXTS("%d.%02d sec"), Sec, MicroSec / 10);
	}
	else if (Seconds < 60.0)
	{
		int32 Sec = Platform::TruncToInt(Seconds);
		int32 MicroSec = Platform::TruncToInt(Seconds * 1000.0f) - Sec * 1000;
		return String::Sprintf(TEXTS("%d.%d sec"), Sec, MicroSec / 100);
	}
	else if (Seconds < 3600.0)
	{
		int32 Min = Platform::TruncToInt(Seconds / 60.0f);
		int32 Sec = Platform::TruncToInt(Seconds) - Min * 60;
		return String::Sprintf(TEXTS("%d:%02d min"), Min, Sec);
	}
	else
	{
		int32 Hour = Platform::TruncToInt(Seconds / 3600.0f);
		int32 Min = Platform::TruncToInt((Seconds - Hour * 3600.0f) / 60.0f);
		int32 Sec = Platform::TruncToInt(Seconds) - Hour * 3600 - Min * 60;
		return String::Sprintf(TEXTS("%d:%02d:%02d hours"), Hour, Min, Sec);
	}
}


void WindowsTime::UpdateCPUTime()
{
	//Cycle counter

	static double LastProcessTimeTotal = 0.0f;
	static double LastUserKernelTimeTotal = 0.0f;

	FILETIME ProcessCreateTime = { 0 };
	FILETIME ProcessExitTime = { 0 };
	FILETIME ProcessUserTime = { 0 };
	FILETIME ProcessKernelTime = { 0 };
	FILETIME ProcessCurrentTime = { 0 };

	::GetProcessTimes(::GetCurrentProcess(), &ProcessCreateTime, &ProcessExitTime, &ProcessKernelTime, &ProcessUserTime);
	::GetSystemTimeAsFileTime(&ProcessCurrentTime);

	const double CurrentUserKernelTimeTotal = FileTimeToSeconds(ProcessUserTime) + FileTimeToSeconds(ProcessKernelTime);
	const double CurrentProcessTimeTotal = FileTimeToSeconds(ProcessCurrentTime) - FileTimeToSeconds(ProcessCreateTime);

	const double IntervalUserKernelTime = CurrentUserKernelTimeTotal - LastUserKernelTimeTotal;
	const double IntervalProcessTime = CurrentProcessTimeTotal - LastProcessTimeTotal;

	if (IntervalUserKernelTime > 0.0)
	{
		CPUTimeUtilizationCores = IntervalUserKernelTime / IntervalProcessTime * 100.0;

		LastProcessTimeTotal = CurrentProcessTimeTotal;
		LastUserKernelTimeTotal = CurrentUserKernelTimeTotal;
	}

}


