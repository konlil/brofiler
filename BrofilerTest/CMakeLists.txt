# 查找当前目录下所有源文件
# 并将名称保存到 BROFILER_TEST_SRCS
# aux_source_directory(. BROFILER_TEST_SRCS)

set(SOURCE_FILES TestEngine.cpp TestEngine.h)

# 设置包含路径
include_directories("${PROJECT_SOURCE_DIR}/ThirdParty/TaskScheduler/Scheduler/include")
include_directories("${PROJECT_SOURCE_DIR}/BrofilerCore")

# 生成链接库
add_library(BrofilerTest ${SOURCE_FILES})