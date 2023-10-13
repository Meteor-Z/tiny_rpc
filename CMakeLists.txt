cmake_minimum_required(VERSION 3.22.1)

project(tiny_rpc)
set(CMAKE_CXX_STANDARD 23)

# 这个是查询的链接
include_directories(${CMAKE_SOURCE_DIR}/src/include)
include_directories(${CMAKE_SOURCE_DIR}/third_party/include)

add_subdirectory(test/io_thread_group)
add_subdirectory(test/tcp)
add_subdirectory(test/tcp_server)