#include <cstdarg>
#include <string>

#include "Config.h"
#include "Common.h"
#if defined( BF_PLATFORM_WINDOWS )
#include <Windows.h>
#include "Platform/Win_Time.h"
#elif defined( BF_PLATFORM_OSX ) || defined( BF_PLATFORM_IOS ) || defined( BF_PLATFORM_ANDROID )
#include "Platform/Posix_Time.h"
#elif defined(BF_PLATFORM_LINUX)
#include "Platform/Platform.h"
#else
#error "Unrecognzied Platform"
#endif

#if defined(BF_PLATFORM_ANDROID)
#include <android/log.h>
#endif

#include "Thread.h"
#include "Log.h"

namespace Brofiler
{
namespace Platform
{
#define MAX_HEADER_LEN 64
#define MAX_BUF_LEN 4096

Mutex g_log_mutex;

void OutputLog(const char* head, const char* msg)
{
	ScopedGuard lock(g_log_mutex);

#if defined( BF_PLATFORM_ANDROID )
	__android_log_write(ANDROID_LOG_INFO, head, msg);
#else

#if defined( BF_PLATFORM_WINDOWS )
#define OUTPUT(str) ::OutputDebugString(str)
#else
#define OUTPUT(str) printf("%s", str)
#endif

	if (head && ::strcmp(head, "") != 0)
	{
		OUTPUT(head);
	}

	if (msg)
	{
		OUTPUT(msg);
		int len = (int)::strlen(msg);
		if (len == 0 || msg[len - 1] != '\n')
		{
			OUTPUT("\n");
		}
	}
#endif
}

void Log(const char* format, ...)
{
	char str_buf[MAX_BUF_LEN];

	va_list va;
	va_start(va, format);
#if _WIN32
	::_vsnprintf(str_buf, MAX_BUF_LEN, format, va);
#else
	::vsnprintf(str_buf, MAX_BUF_LEN, format, va);
#endif
	va_end(va);
	str_buf[MAX_BUF_LEN - 1] = '\0';


	LocalTime t = GetLocalTime();

	char header[MAX_HEADER_LEN];
	char *p = header;

	p += ::sprintf(p, "[%02d:%02d:%02d.%03d] ",
		t.hour, t.minute, t.second, t.millisecond);

	OutputLog(header, str_buf);
}

}
}
