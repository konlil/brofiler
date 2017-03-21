#pragma once

namespace Brofiler
{
namespace Platform
{
	class ScopedGuard;

	class Mutex
	{
		CRITICAL_SECTION criticalSection;

	public:

		NOCOPYABLE(Mutex);

		Mutex()
		{
			::InitializeCriticalSectionAndSpinCount( &criticalSection, 16 );
		}

		~Mutex()
		{
			::DeleteCriticalSection( &criticalSection );
		}

		friend class Platform::ScopedGuard;

	private:

		void Lock()
		{
			::EnterCriticalSection( &criticalSection );
		}
		void Unlock()
		{
			::LeaveCriticalSection( &criticalSection );
		}

	};
}
}