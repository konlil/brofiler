# 查找当前目录下所有源文件
# 并将名称保存到 BROFILER_WINDOWS_TEST_SRCS
aux_source_directory(. BROFILER_WINDOWS_TEST_SRCS)

# 设置包含路径
include_directories("${PROJECT_SOURCE_DIR}/ThirdParty/TaskScheduler/Scheduler/include")
include_directories("${PROJECT_SOURCE_DIR}/BrofilerCore")
include_directories("${PROJECT_SOURCE_DIR}/BrofilerTest")

set(MT_DEPENDS
	${PROJECT_SOURCE_DIR}/ThirdParty/TaskScheduler/Scheduler/Source/MTDefaultAppInterop.cpp)

# 指定生成目标
add_executable(BrofilerWindowsTest 
				main.cpp
				${MT_DEPENDS})

source_group("ThirdParty\\TaskScheduler\\Scheduler\\Source" FILES ${MT_DEPENDS})

# 添加链接库
target_link_libraries(BrofilerWindowsTest 
							TaskScheduler
							BrofilerCore
							BrofilerTest)