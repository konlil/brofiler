# 基于开源项目android CMake的代码修改
# http://android-cmake.googlecode.com
cmake_minimum_required( VERSION 2.8.5 )
set( ANDROID_ABI "armeabi-v7a")
set( ANDROID_NDK_VERSION "10")
set( ANDROID_NDK "D:\\3rd\\Android\\ndk\\win32\\android-ndk-r10e")
set( ANDROID_SDK "D:\\3rd\\Android\\sdk\\win32\\adt_win32")
set( ANDROID_SDK_HOME "D:\\3rd\\Android\\sdk\\win32\\adt_win32")
#set( CMAKE_BUILD_TYPE "Debug" CACHE STRING "Build Type" FORCE)
include("${CMAKE_CURRENT_LIST_DIR}/android.toolchain_ndk10.cmake")

set(ANDROID TRUE)
