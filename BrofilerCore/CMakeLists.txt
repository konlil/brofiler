
# 查找当前目录下所有源文件
# 并将名称保存到 BROFILERCORE_SRCS
# aux_source_directory(. BROFILERCORE_SRCS)

set(API Brofiler.h)
set(Platform
		Platform/SamplingProfiler.cpp
		Platform/SamplingProfiler.h
		Platform/SchedulerTrace.cpp
		Platform/SchedulerTrace.h
		Platform/SymbolEngine.cpp
		Platform/SymbolEngine.h
		Platform/ThreadsEnumerator.h)

set(Platform_Windows
		Platform/Windows/ETW.h
		Platform/Windows/EtwTracer.cpp
		Platform/Windows/EtwTracer.h
		Platform/Windows/Sampler.cpp
		Platform/Windows/Sampler.h
		Platform/Windows/SymEngine.cpp
		Platform/Windows/SymEngine.h
		Platform/Windows/WinThreadsEnumerator.cpp)

set(Utils
		CallstackCollector.cpp
		CallstackCollector.h
		Interop.cpp
		SysCallCollector.cpp
		SysCallCollector.h)

set(Core
		Core.cpp
		Core.h
		Event.h
		Event.cpp
		EventDescriptionBoard.cpp
		EventDescriptionBoard.h)

set(Network
		Message.cpp
		Message.h
		ProfilerServer.cpp
		ProfilerServer.h
		Serialization.cpp
		Serialization.h
		Socket.h)

set(System
		Common.h
		MemoryPool.h
		Thread.h)

source_group("API" FILES ${API})
source_group("Platform" FILES ${Platform})
source_group("Platform\\Windows" FILES ${Platform_Windows})
source_group("Utils" FILES ${Utils})
source_group("Core" FILES ${Core})
source_group("Network" FILES ${Network})
source_group("System" FILES ${System})

set(SOURCE_FILES
		${API}
		${Platform}
		${Platform_Windows}
		${Utils}
		${Core}
		${Network}
		${System})

# 设置包含路径
include_directories("${PROJECT_SOURCE_DIR}/ThirdParty/TaskScheduler/Scheduler/include")
include_directories("${PROJECT_SOURCE_DIR}/BrofilerCore")

# 自定义宏
add_definitions(-DBROFILER_LIB=1)

# 生成链接库
add_library(BrofilerCore ${SOURCE_FILES})