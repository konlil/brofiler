set(HEADER_FILES
		Include/MTAppInterop.h
		Include/MTArrayView.h
		Include/MTAtomic.h
		Include/MTColorTable.h
		Include/MTConcurrentRingBuffer.h
		Include/MTConfig.h
		Include/MTDebug.h
		Include/MTFiberContext.h
		Include/MTFiberContext.inl
		Include/MTGroupedTask.h
		Include/MTPlatform.h
		Include/MTProfilerEventListener.h
		Include/MTQueueMPMC.h
		Include/MTScheduler.h
		Include/MTScheduler.inl
		Include/MTStackRequirements.h
		Include/MTStaticVector.h
		Include/MTTaskBucket.h
		Include/MTTaskDesc.h
		Include/MTTaskGroup.h
		Include/MTTaskPool.h
		Include/MTTaskQueue.h
		Include/MTThreadContext.h
		Include/MTTools.h
		Include/MTTypes.h
	)

set(HEADER_SCOPES
		Include/Scopes/MTScopes.h)

set(HEADER_PLATFORM_COMMON
		Include/Platform/Common/MTAtomic.h
		Include/Platform/Common/MTSpinWait.h
		Include/Platform/Common/MTThread.h
		)

set(HEADER_PLATFORM_POSIX
		Include/Platform/Posix/MTAtomic.h
		Include/Platform/Posix/MTCommon.h
		Include/Platform/Posix/MTEvent.h
		Include/Platform/Posix/MTFiber.h
		Include/Platform/Posix/MTMemory.h
		Include/Platform/Posix/MTMutex.h
		Include/Platform/Posix/MTThread.h
		Include/Platform/Posix/MTUtils.h
		)

set(HEADER_PLATFORM_WINDOWS
		Include/Platform/Windows/MicroWindows.cpp
		Include/Platform/Windows/MicroWindows.h
		Include/Platform/Windows/MTAtomic.h
		Include/Platform/Windows/MTCommon.h
		Include/Platform/Windows/MTEvent.h
		Include/Platform/Windows/MTFiber.h
		Include/Platform/Windows/MTFiberDefault.h
		Include/Platform/Windows/MTFiberOptimized.h
		Include/Platform/Windows/MTMemory.h
		Include/Platform/Windows/MTMutex.h
		Include/Platform/Windows/MTThread.h
		Include/Platform/Windows/MTUtils.h	
		)

set(ALL_HEADERS
		${HEADER_FILES}
		${HEADER_SCOPES}
		${HEADER_PLATFORM_COMMON}
		${HEADER_PLATFORM_POSIX}
		${HEADER_PLATFORM_WINDOWS})

set(SOURCE_FILES
#		Source/MTDefaultAppInterop.cpp
		Source/MTFiberContext.cpp
		Source/MTScheduler.cpp
		Source/MTThreadContext.cpp
	)

source_group("TaskScheduler\\Include" FILES ${HEADER_FILES})
source_group("TaskScheduler\\Include\\Scopes" FILES ${HEADER_SCOPES})
source_group("TaskScheduler\\Include\\Platform\\Common" FILES ${HEADER_PLATFORM_COMMON})
source_group("TaskScheduler\\Include\\Platform\\Posix" FILES ${HEADER_PLATFORM_POSIX})
source_group("TaskScheduler\\Include\\Platform\\Windows" FILES ${HEADER_PLATFORM_WINDOWS})
source_group("TaskScheduler\\Source" FILES ${SOURCE_FILES})

# 添加头文件定义
include_directories("${PROJECT_SOURCE_DIR}/ThirdParty/TaskScheduler/Scheduler/include")

# 自定义宏
add_definitions(-DUSE_BROFILER=1)
add_definitions(${COMMON_DEF})

# 生成链接库
add_library(TaskScheduler ${SOURCE_FILES} ${ALL_HEADERS})