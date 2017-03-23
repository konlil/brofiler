#pragma once


//Include Headers
#include "Platform/Common.h"
#include "Platform/Memory.h"
#if defined( BF_PLATFORM_WINDOWS )
#include <Windows.h>
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

#include "Platform/Log.h"
