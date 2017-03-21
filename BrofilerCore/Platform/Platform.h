#pragma once

// Compiler support for SSE intrinsics
#if (defined(__SSE__) || defined(_M_IX86) || defined(_M_X64))
#define SSE_INTRINSICS_SUPPORTED (1)
#endif

//Debug Assert
#if defined( BF_PLATFORM_WINDOWS)
#define BF_ASSERT(x) do{ if(!(x)) { __debugbreak(); } } while(0);
#elif defined( BF_PLATFORM_IOS || BF_PLATFORM_ANDROID )
#define BF_ASSERT(x) do{ if(!x) { *((char*)0) = 0; } } while(0);
#endif


//Include Headers
#include "Platform/Common.h"
#include "Platform/Memory.h"
#if defined( BF_PLATFORM_WINDOWS )
#include "Platform/Win_Time.h"
#elif defined( BF_PLATFORM_IOS )
#include "Platform/iOS/Time.h"
#elif defined( BF_PLATFORM_ANDROID )
#include "Platform/Posix_Time.h"
#include "Platform/Posix_Memory.h"
#else
#error "Unrecognzied Platform"
#endif
#include "Platform/Thread.h"