#pragma once
#include <thread>
#include <atomic>

#if defined( BF_PLATFORM_WINDOWS )
#include "Platform/Win_Mutex.h"
#include "Platform/Win_Thread.h"
#elif defined( BF_PLATFORM_OSX )
#include "Platform/Posix_Mutex.h"
#include "Platform/Posix_Thread.h"
#elif defined( BF_PLATFORM_IOS )
#include "Platform/iOS_Thread.h"
#elif defined( BF_PLATFORM_ANDROID )
#include "Platform/Posix_Thread.h"
#else
#error "Unrecognzied Platform"
#endif


#define THREADID std::thread::id

namespace Brofiler
{
namespace Platform
{
	//typedef
	template <class T>
	using Atomic = std::atomic<T>;

	class ScopedGuard
	{
		Platform::Mutex & mutex;

		ScopedGuard(const ScopedGuard &) : mutex(*((Platform::Mutex*)nullptr)) {}
		void operator=(const ScopedGuard &) {}

	public:

		ScopedGuard(Platform::Mutex & _mutex) : mutex(_mutex)
		{
			mutex.Lock();
		}

		~ScopedGuard()
		{
			mutex.Unlock();
		}
	};

	namespace Thread
	{
		inline THREADID CurrentThreadID() { return std::this_thread::get_id(); }
	};
}
}

