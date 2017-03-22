#pragma once

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


//Include Headers
#include "Platform/Common.h"
#include "Platform/Memory.h"
#if defined( BF_PLATFORM_WINDOWS )
#include "Platform/Win_Time.h"
#include "Platform/Win_Tls.h"
#elif defined( BF_PLATFORM_OSX )
#include "Platform/Posix_Time.h"
#include "Platform/Posix_Memory.h"
#elif defined( BF_PLATFORM_IOS )
#include "Platform/Posix_Time.h"
#include "Platform/Posix_Memory.h"
#include "Platform/Posix_Tls.h"
#elif defined( BF_PLATFORM_ANDROID )
#include "Platform/Posix_Time.h"
#include "Platform/Posix_Memory.h"
#else
#error "Unrecognzied Platform"
#endif
#include "Platform/Thread.h"
