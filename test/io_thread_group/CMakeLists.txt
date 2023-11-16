# cmake_minimum_required(VERSION 3.22.1)

# project()
# set(CMAKE_CXX_STANDARD 23)

add_executable(io_thread_group io_thread_group_test.cc)

include_directories(${CMAKE_SOURCE_DIR}/src/include)

# 这个是tinyxml的头文件
include_directories(${CMAKE_SOURCE_DIR}/third_party/tinyxml/include)

# 这个是tinyxml的lib库
find_library(TINYXML tinyxml ${CMAKE_SOURCE_DIR}/third_party/tinyxml/lib)

target_include_directories(io_thread_group PRIVATE ${CMAKE_SOURCE_DIR}/third_party/fmt/include)

add_library(io_thread_libs STATIC
    ${CMAKE_SOURCE_DIR}/src/common/config.cc
    ${CMAKE_SOURCE_DIR}/src/common/log.cc
    ${CMAKE_SOURCE_DIR}/src/common/utils.cc
    ${CMAKE_SOURCE_DIR}/src/net/time/time_event.cc
    ${CMAKE_SOURCE_DIR}/src/net/time/timer.cc
    ${CMAKE_SOURCE_DIR}/src/net/eventloop.cc
    ${CMAKE_SOURCE_DIR}/src/net/fd_event/fd_event.cc
    ${CMAKE_SOURCE_DIR}/src/net/wakeup_fd_event.cc
    ${CMAKE_SOURCE_DIR}/src/net/io_thread/io_thread.cc
    ${CMAKE_SOURCE_DIR}/src/net/io_thread/io_thread_group.cc
)
# 依赖
target_link_libraries(io_thread_group PRIVATE io_thread_libs)
target_link_libraries(io_thread_group PRIVATE ${TINYXML})
target_link_libraries(io_thread_group PRIVATE fmt)

add_test(NAME test COMMAND io_thread_group) # 这个是添加一个子cmake目录。
