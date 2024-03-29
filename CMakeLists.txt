cmake_minimum_required(VERSION 3.22.1)
# 设置编译器为gcc/g++
SET(CMAKE_C_COMPILER /usr/bin/gcc)
SET(CMAKE_CXX_COMPILER /usr/bin/g++)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON) 

project(tiny_rpc)
# 设置必须跟上面的一样，即c++17

# 设置生成 compile_commands
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# 显示编译时间
set_property(GLOBAL PROPERTY RULE_LAUNCH_COMPILE "${CMAKE_COMMAND} -E time")
set_property(GLOBAL PROPERTY RULE_LAUNCH_LINK "${CMAKE_COMMAND} -E time")

# 设置成debug模式
set(CMAKE_BUILD_TYPE "Debug")
# 设置调试的指令
add_compile_options(-Wall -Wextra -Wshadow -Wpedantic) # fsanitize=undefined -fsanitize=address

# 加入本项目的头文件
include_directories(${CMAKE_SOURCE_DIR}/src/include)

# fmt的头文件
include_directories(${CMAKE_SOURCE_DIR}/third_party/fmt/include)
add_subdirectory(third_party/fmt)


# 以下是测试文件
# add_subdirectory(test/timer_event)
# add_subdirectory(test/event_loop)
# # add_subdirectory(test/io_thread_group)
# add_subdirectory(test/tcp)
# add_subdirectory(test/tcp_server)
# add_subdirectory(test/tcp_client)
# add_subdirectory(test/tcp_dispatchor)
add_subdirectory(test/rpc)
# add_subdirectory(test/tcp_channel)