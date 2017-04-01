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
#elif defined(LINUX)
#define BF_PLATFORM_LINUX (1)
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
#define BF_FORCEINLINE __forceinline
#else
#define BF_FORCEINLINE inline __attribute__((always_inline))
#endif

// Compiler support for SSE intrinsics
#if (defined(__SSE__) || defined(_M_IX86) || defined(_M_X64))
#define SSE_INTRINSICS_SUPPORTED (1)
#endif

//Debug Assert
#if defined( BF_PLATFORM_WINDOWS)
#define BF_ASSERT(x, desc) do{ if(!(x)) { __debugbreak(); } } while(0);
#else
//#define BF_ASSERT(x, desc) do{ if(!(x)) { *((char*)0) = 0; } } while(0);
#define BF_ASSERT(x, desc)
#endif

//used to ignore gcc warning
#define BF_UNUSED(x) (void)(x)
