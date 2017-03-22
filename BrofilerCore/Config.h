#pragma once

#if defined(_WIN32)
#define BF_PLATFORM_WINDOWS (1)
#elif defined( __APPLE__ )
#include "TargetConditionals.h"
#if TARGET_OS_OSX
#define BF_PLATFORM_OSX (1)
#elif TARGET_OS_IPHONE
#define BF_PLATFORM_IOS (1)
#else
#error "Unknown Apple Platform"
#endif
#endif

// Compiler family
#ifdef __clang__
#define BF_WITH_CLANG (1)
#define BF_WITH_GCC (1)
#elif __GNUC__
#define BF_WITH_GCC (1)
#elif defined(_MSC_VER)
#define BF_WITH_MSVC (1)
#endif

// Inline
#ifdef _WIN32
#define BF_FORCEINLINE __forceline
#else
#define BF_FORCEINLINE inline __attribute__((always_inline))
#endif
