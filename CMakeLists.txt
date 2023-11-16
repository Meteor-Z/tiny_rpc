cmake_minimum_required(VERSION 3.22.1)

project(tiny_rpc)
set(CMAKE_CXX_STANDARD 17)

# 设置生成 compile_commands
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# 显示编译时间
set_property(GLOBAL PROPERTY RULE_LAUNCH_COMPILE "${CMAKE_COMMAND} -E time")
set_property(GLOBAL PROPERTY RULE_LAUNCH_LINK "${CMAKE_COMMAND} -E time")


include_directories(${CMAKE_SOURCE_DIR}/src/include)
include_directories(${CMAKE_SOURCE_DIR}/third_party/fmt/include)
add_subdirectory(third_party/fmt)

# 以下是测试文件
add_subdirectory(test/timer_event)
add_subdirectory(test/event_loop)
add_subdirectory(test/io_thread_group)
add_subdirectory(test/tcp)
add_subdirectory(test/tcp_server)
add_subdirectory(test/tcp_client)