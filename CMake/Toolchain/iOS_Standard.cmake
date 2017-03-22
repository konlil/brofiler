# Toolchain for standard iOS target
if (NOT ${CMAKE_HOST_APPLE})
	message(FATAL_ERROR "The iOS version can only be built on MacOS")
endif()

option(USE_LIBCPP "Enable/Disable libc++ on iOS" ON)

set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_VERSION 1)
set(CMAKE_SYSTEM_PROCESSOR arm)

if (EXISTS "/Developer/Platforms/iPhoneOS.platform/Developer")
	set(DEVROOT "/Developer/Platforms/iPhoneOS.platform/Developer")
elseif(EXISTS /Applications/Xcode.app/Contents/Developer/Platforms/iPhoneOS.platform/Developer)
	set(DEVROOT /Applications/Xcode.app/Contents/Developer/Platforms/iPhoneOS.platform/Developer)
else()
	message(FATAL_ERROR "can't find developer root directory,set DEVROOT failed")
endif()

#目标系统版本
set(TARGET_OS_VER "7.0")
set(CMAKE_XCODE_ATTRIBUTE_IPHONEOS_DEPLOYMENT_TARGET "${TARGET_OS_VER}")

# 查找SDK版本
file( GLOB _SDKs "${DEVROOT}/SDKs/iPhoneOS*.sdk" )
set(SDKVER "6.0")
foreach(_SDK ${_SDKs})
	if(IS_DIRECTORY ${_SDK})
		string(REGEX REPLACE "${DEVROOT}/SDKs/iPhoneOS([^ ]+).sdk" "\\1" _r ${_SDK})
		if (NOT ${_r} STREQUAL ${_SDK})
			string(LENGTH ${SDKVER} len1)
			string(LENGTH ${_r} len2)
			if (len1 EQUAL len2)
				if (${SDKVER} STRLESS ${_r})
					set(SDKVER ${_r})
				endif()
			else()
				if (len1 LESS len2)
					set(SDKVER ${_r})
				endif()			
			endif()
		endif()
	endif()
endforeach()
message(STATUS "iPhoneOS SDK Version is ${SDKVER}")

# 查找xcode版本
execute_process(COMMAND xcodebuild -version OUTPUT_VARIABLE _xcode_version)
string(REGEX MATCH "Xcode [1-9.]+" _xcode_version ${_xcode_version})
if (_xcode_version)
	string(REPLACE "Xcode " "" _xcode_version ${_xcode_version})
	message(STATUS "xcode version is ${_xcode_version}")
endif()

set(SDKROOT "${DEVROOT}/SDKs/iPhoneOS${SDKVER}.sdk")
set(CMAKE_SYSROOT ${SDKROOT})
set(CMAKE_OSX_SYSROOT "iphoneos")
set(CMAKE_OSX_ARCHITECTURES "\$\(ARCHS_STANDARD\)")

set(_other_linker_flags)

if(USE_LIBCPP)
	message(STATUS "configure with libcpp ...")
	set(CMAKE_XCODE_ATTRIBUTE_GCC_VERSION "com.apple.compilers.llvm.clang.1_0")
	set(CMAKE_XCODE_ATTRIBUTE_CLANG_CXX_LANGUAGE_STANDARD "c++11")
	set(CMAKE_XCODE_ATTRIBUTE_CLANG_CXX_LIBRARY "libc++")
	set(_other_linker_flags "-lstdc++.6")
	add_definitions("-DUSE_LIBCPP")
	add_definitions("-Wno-c++11-narrowing")
	if (_xcode_version)
		if (${_xcode_version} STRLESS "5.1.1")
			message(FATAL_ERROR "USE_LIBCPP is only test on Xcode 5.1.1 please update your xcode version first")
		endif()
	endif()
else()
	message(STATUS "configure with cpp11 ...")
	set(CMAKE_XCODE_ATTRIBUTE_CLANG_CXX_LANGUAGE_STANDARD "c++11")
	set(CMAKE_XCODE_ATTRIBUTE_CLANG_CXX_LIBRARY "libstdc++")
endif()

set(CMAKE_XCODE_ATTRIBUTE_ENABLE_BITCODE "No")

if (_xcode_version)
	if (NOT ("${_xcode_version}" STRLESS "6.0"))
		set(IOS_CODE_SIGN_RESOURCE_RULES_PATH ${SDKROOT}/ResourceRules.plist)
	endif()
endif()

set(CMAKE_IMPORT_LIBRARY_SUFFIX ".a")
set(CMAKE_EXE_LINKER_FLAGS 
"-F${SDKROOT}/System/Library/Frameworks -framework Foundation -framework AudioToolbox -framework QuartzCore -framework OpenAL -framework OpenGLES -framework CoreGraphics -framework UIKit -framework CoreText ${_other_linker_flags}" 
CACHE STRING "linker flags")

include(CMakeForceCompiler)
execute_process(COMMAND xcrun -f clang OUTPUT_VARIABLE CLANG_PATH)
execute_process(COMMAND xcrun -f clang++ OUTPUT_VARIABLE CLANGXX_PATH)
string(STRIP ${CLANG_PATH} CLANG_PATH)
string(STRIP ${CLANGXX_PATH} CLANGXX_PATH)
CMAKE_FORCE_C_COMPILER("${CLANG_PATH}" GNU)
CMAKE_FORCE_CXX_COMPILER("${CLANGXX_PATH}" GNU)

add_definitions("-pipe")
add_definitions("-no-cpp-precomp")
add_definitions("--sysroot=${SDKROOT}")
add_definitions("-miphoneos-version-min=${TARGET_OS_VER}")
# add_definitions("-mno-thumb")
# add_definitions("-fexceptions")
# add_definitions("-fomit-frame-pointer")
# add_definitions("-flto")
#add_definitions("-arch armv7")
if (NOT ("${SDKVER}" STRLESS "7.0"))
	#7.0以上版本明确定义CPU架构
	add_definitions("-arch armv7")
endif()


#message(STATUS "sdk root: ${SDKROOT}")
include_directories(SYSTEM "${SDKROOT}/usr/include")
if (NOT ("${SDKVER}" STRLESS "7.0"))
	if (NOT USE_LIBCPP)
		include_directories(SYSTEM "${SDKROOT}/usr/include/c++/4.2.1")
	endif()
else()
	include_directories(SYSTEM "${SDKROOT}/usr/c++/4.2.1")
endif()


link_directories("${SDKROOT}/System/Library/Frameworks")
link_directories("${SDKROOT}/usr/lib")
link_directories("${SDKROOT}/usr/lib/system")

set (CMAKE_FIND_ROOT_PATH "${DEVROOT}" "${SDKROOT}")
set (CMAKE_FIND_ROOT_PATH_MODE_PROGRAM BOTH)
set (CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set (CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)

set(IOS TRUE)
set(IOS_VERSION ${SDKVER})