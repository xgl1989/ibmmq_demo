cmake_minimum_required(VERSION 3.10)
project(ibmmq_demo)

# 设置C++标准
set(CMAKE_CXX_STANDARD 11)

# 指定IBM MQ库和头文件路径
set(MQ_INCLUDE_DIR /opt/mqm/inc)
set(MQ_LIB_DIR /opt/mqm/lib64)

# 包含IBM MQ头文件路径
include_directories(${MQ_INCLUDE_DIR})

# 设置可执行文件和依赖库
add_executable(ibmmq_demo main.cpp)
target_link_libraries(ibmmq_demo ${MQ_LIB_DIR}/libmqm.so)
