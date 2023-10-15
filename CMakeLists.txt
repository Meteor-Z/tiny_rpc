cmake_minimum_required(VERSION 3.22.1)

project(tiny_rpc)
set(CMAKE_CXX_STANDARD 23)

# 这个是查询的链接
include_directories(${CMAKE_SOURCE_DIR}/src/include)
# include_directories(${CMAKE_SOURCE_DIR}/third_party/fmt/include)

add_subdirectory(third_party/fmt)
add_subdirectory(third_party/tinyxml)

# 以下是测试文件
add_subdirectory(test/timer_event)
add_subdirectory(test/event_loop)
add_subdirectory(test/io_thread_group)
add_subdirectory(test/tcp)
add_subdirectory(test/tcp_server)