#pragma once

#include <stddef.h>
#include <stdlib.h>
#if SSE_INTRINSICS_SUPPORTED
#include <xmmintrin.h>
#endif

//Windows…œ∂‘”¶_aligned_malloc
#if defined(BF_PLATFORM_WINDOWS)
#define posix_memalign(p, a, s) (((*(p)) = _aligned_malloc((s), (a))), *(p) ?0 :errno)

#if 0
static int check_align(size_t align)
{
	for (size_t i = sizeof(void *); i != 0; i *= 2)
		if (align == i)
			return 0;
	return EINVAL;
}

int posix_memalign(void **ptr, size_t align, size_t size)
{
	if (check_align(align))
		return EINVAL;

	int saved_errno = errno;
	void *p = _aligned_malloc(size, align);
	if (p == NULL)
	{
		errno = saved_errno;
		return ENOMEM;
	}

	*ptr = p;
	return 0;
}
#endif

#endif

namespace Brofiler
{
namespace Platform
{

class Memory 
{
public:
	static void* Alloc(size_t size, size_t align)
	{
		void* p = nullptr;
#if SSE_INTRINSICS_SUPPORTED
		p = _mm_malloc(size, align);
#else
#if defined(BF_PLATFORM_IOS) || defined(BF_PLATFORM_ANDROID)
        p = malloc(size);
#else
		if (posix_memalign(&p, size, align) != 0)
		{
			p = nullptr;
		}
#endif
#endif
		BF_ASSERT(p, "alloc memory failed");
		return p;
	}

	static void Free(void* p)
	{
#if SSE_INTRINSICS_SUPPORTED
		_mm_free(p);
#else
		free(p);
#endif
	}
};

}
}
