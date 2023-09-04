cmake_minimum_required(VERSION 3.22.1)

project(tiny_rpc)
set(CMAKE_CXX_STANDARD 20)

add_executable(tiny_rpc main.cpp src/common/log.cpp src/common/utils.cpp) 

# set(CMAKE_CXX_FLAGS "-g") # 可以添加cpp编译选项
target_link_libraries(tiny_rpc "/usr/local/lib/libfmt.a") # 链接libfmt.a库