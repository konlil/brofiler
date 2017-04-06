#ifndef __TIME_H_
#define __TIME_H_

#include <stdint.h>
#include <windows.h>
#include <Shlwapi.h>
#pragma comment (lib, "Shlwapi.lib")

namespace Brofiler
{
namespace Platform
{

void Sleep(unsigned long time_ms)
{
	::Sleep(time_ms);
}

inline int64 GetFrequency()
{
	MW_LARGE_INTEGER frequency;
	QueryPerformanceFrequency(&frequency);
	return frequency.QuadPart;
}

inline int64_t TimeMilliSeconds()
{
	static double freq_unit = 0.0;
	if (freq_unit <= 0)
	{
		LARGE_INTEGER frequency;
		QueryPerformanceFrequency(&frequency);
		freq_unit = double(1000.0 / frequency.QuadPart);  // 以ms为单位，所以乘以1000
	}

	LARGE_INTEGER largeInteger;
	QueryPerformanceCounter(&largeInteger);
	return largeInteger.QuadPart/ freq_unit;
}

inline int64_t TimeMicroSeconds()
{
	LARGE_INTEGER frequency;
	QueryPerformanceFrequency(&frequency);

	LARGE_INTEGER largeInteger;
	QueryPerformanceCounter(&largeInteger);
	return largeInteger.QuadPart * int64_t(1000000) / frequency.QuadPart;
}

LocalTime GetLocalTime()
{
	LocalTime local_time;
	SYSTEMTIME t;
	::GetLocalTime(&t);
	local_time.year = t.wYear;
	local_time.month = t.wMonth;
	local_time.day = t.wDay;
	local_time.hour = t.wHour;
	local_time.minute = t.wMinute;
	local_time.second = t.wSecond;
	local_time.millisecond = t.wMilliseconds;
	return local_time;
}


}//Platform
}//Brofiler


#endif //#pragma once