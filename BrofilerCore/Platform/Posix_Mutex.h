#ifndef __POSIX_MUTEX_H_
#define __POSIX_MUTEX_H_

namespace Brofiler
{
namespace Platform
{
		class ScopedGuard;

		//
		//
		//
		class Mutex
		{
			pthread_mutexattr_t mutexAttr;
			pthread_mutex_t mutex;

		public:

			NOCOPYABLE(Mutex);

			Mutex()
			{
				int res = pthread_mutexattr_init(&mutexAttr);
                BF_UNUSED(res);
				BF_ASSERT(res == 0, "pthread_mutexattr_init - failed");

				res = pthread_mutexattr_settype(&mutexAttr, PTHREAD_MUTEX_RECURSIVE);
				BF_ASSERT(res == 0, "pthread_mutexattr_settype - failed");

				res = pthread_mutex_init(&mutex, &mutexAttr);
				BF_ASSERT(res == 0, "pthread_mutex_init - failed");
			}

			~Mutex()
			{
				int res = pthread_mutex_destroy(&mutex);
                BF_UNUSED(res);
				BF_ASSERT(res == 0, "pthread_mutex_destroy - failed");

				res = pthread_mutexattr_destroy(&mutexAttr);
                BF_UNUSED(res);
				BF_ASSERT(res == 0, "pthread_mutexattr_destroy - failed");
			}

			friend class Platform::ScopedGuard;

		private:

			void Lock()
			{
				int res = pthread_mutex_lock(&mutex);
                BF_UNUSED(res);
				BF_ASSERT(res == 0, "pthread_mutex_lock - failed");
			}
			void Unlock()
			{
				int res = pthread_mutex_unlock(&mutex);
                BF_UNUSED(res);
				BF_ASSERT(res == 0, "pthread_mutex_unlock - failed");
			}

		};
}
}

#endif // !__POSIX_MUTEX_H_
