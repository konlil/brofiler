#pragma once

#if defined(_WIN32)
#define BF_PLATFORM_WINDOWS (1)
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