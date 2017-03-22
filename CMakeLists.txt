# CMake 最低版本号
cmake_minimum_required(VERSION 3.1)

if(IOS OR IOS_SIM)
	message(STATUS "ios...")
elseif(ANDROID OR ANDROID_SIM)
	message(STATUS "android...")
elseif(WIN32)
	message(STATUS "win32...")
elseif(APPLE)
	message(STATUS "mac...")
endif()

# 配置类型只支持Debug和带调试信息的Release
set(CMAKE_CONFIGURATION_TYPES "Debug;Release" CACHE STRING "" FORCE)

# 不生成ZERO CHECK
set(CMAKE_SUPPRESS_REGENERATION true)

# 使用C++11
set (CMAKE_CXX_STANDARD 11)

# CMAKE_SOURCE_DIR 内置变量，表示Cmake开始构建时的入口路径
# CMAKE_MODULE_PATH 
set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}")

# 项目信息
project (Brofiler)

# 包含公用定义的模块 Common.cmake
include(Common)

# 添加子目录
add_subdirectory(BrofilerCore)
add_subdirectory(BrofilerTest)
add_subdirectory(BrofilerWindowsTest)
#add_subdirectory(ThirdParty/TaskScheduler/Scheduler)