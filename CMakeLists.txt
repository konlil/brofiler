﻿# CMake 最低版本号
cmake_minimum_required(VERSION 3.1)

# 配置类型只支持Debug和带调试信息的Release
set(CMAKE_CONFIGURATION_TYPES "Debug;RelWithDebInfo;Release" CACHE STRING "" FORCE)

# 不生成ZERO CHECK
set(CMAKE_SUPPRESS_REGENERATION true)

# 使用C++11
set (CMAKE_CXX_STANDARD 11)

# CMAKE_SOURCE_DIR 内置变量，表示Cmake开始构建时的入口路径
# CMAKE_MODULE_PATH 
set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/CMake")

# 包含公用定义的模块 CMake/Common.cmake
include(Common)

# 项目信息
if (${CMAKE_SYSTEM_NAME} MATCHES "Linux")
else()
	project (Brofiler)
endif()
# 这时候toolchain已经定义好了平台
if(IOS OR IOS_SIM)
	message(STATUS "ios...")
	set(PLATFORM "ios")
elseif(ANDROID OR ANDROID_SIM)
	message(STATUS "android...")
	set(PLATFORM "android")
elseif(WIN32)
	message(STATUS "win32...")
	set(PLATFORM "win32")
elseif(APPLE)
	message(STATUS "mac...")
	set(PLATFORM "mac")
elseif(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
	message(STATUS "linux...")
	set(PLATFORM "linux")
	add_definitions(-DBF_PLATFORM_LINUX=1)
endif()

# 添加子目录
add_subdirectory(BrofilerCore)
add_subdirectory(BrofilerTest)

if(WIN32)
	add_subdirectory(BrofilerWindowsTest)
elseif(IOS)
	add_subdirectory(BrofilerIOSTest)
elseif(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
	add_subdirectory(BrofilerLinuxTest)
endif()
