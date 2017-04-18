INCLUDE( CMakeDependentOption )

set(PLATFORM_WIN32 "win32")
set(PLATFORM_ANDROID "android")
set(PLATFORM_IOS "ios")
set(PLATFORM_LINUX "linux")

# 公用的自定义宏
add_definitions(-DMT_INSTRUMENTED_BUILD)
add_definitions(-D_ITERATOR_DEBUG_LEVEL=0)
