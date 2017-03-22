#ifndef __TIME_H_
#define __TIME_H_

#if defined(__unix) || defined(__APPLE__)
#include <unistd.h>
#include <sys/time.h>
#include <sys/types.h>
#include <dirent.h>
#include <fnmatch.h>
#include <dirent.h>
#include <stdint.h>

namespace Brofiler
{
namespace Platform
{

inline void Sleep(unsigned long time_ms)
{
	usleep((time_ms << 10) - (time_ms << 4) - (time_ms << 3));

	//nanosleepµÄ·½°¸
	/*struct timespec req;
	int sec = (int)(time_ms / 1000);
	time_ms = time_ms - (sec * 1000);
	req.tv_sec = sec;
	req.tv_nsec = time_ms * 1000000L;
	while (nanosleep(&req, &req) == -1)
	{
		continue;
	}*/
}

inline int64_t GetFrequency()
{
	return 1000000;	//TODO
}
    
inline int64_t TimeMicroSeconds()
{
    struct timeval te;
    gettimeofday(&te, nullptr);
    return te.tv_sec * 1000000LL + te.tv_usec;
}

inline int64_t TimeMilliSeconds()
{
	return TimeMicroSeconds() / 1000;
}



inline LocalTime GetLocalTime()
{
	LocalTime local_time;
	time_t now;
	tm *timenow;
	time(&now);
	timenow = localtime(&now);
	local_time.year = timenow->tm_year;
	local_time.month = timenow->tm_mon;
	local_time.day = timenow->tm_mday;
	local_time.hour = timenow->tm_hour;
	local_time.minute = timenow->tm_min;
	local_time.second = timenow->tm_sec;

	struct timezone tz = { 0, 0 };
	timeval tval;
	gettimeofday(&tval, &tz);
	local_time.millisecond = (tval.tv_usec / 1000) % 1000;
	return local_time;
}

}//Platform
}//Brofiler

#endif // __unix

#endif //#pragma once
