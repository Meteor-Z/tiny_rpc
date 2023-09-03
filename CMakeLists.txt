cmake_minimum_required(VERSION 3.22.1)

project(tiny_rpc)
set(CMAKE_CXX_STANDARD 20)

add_executable(tiny_rpc main.cpp src/common/log.cpp src/common/utils.cpp) 

target_link_libraries(tiny_rpc "/usr/local/lib/libfmt.a")