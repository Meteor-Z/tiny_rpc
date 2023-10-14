# cmake_minimum_required(VERSION 3.22.1)

# project()
# set(CMAKE_CXX_STANDARD 23)

add_executable(io_thread_group io_thread_group_test.cpp)

include_directories(${CMAKE_SOURCE_DIR}/src/include)

# 这个是tinyxml的头文件
include_directories(${CMAKE_SOURCE_DIR}/third_party/tinyxml/include)

# 这个是tinyxml的lib库
find_library(TINYXML tinyxml ${CMAKE_SOURCE_DIR}/third_party/tinyxml/lib)

if (TINYXML) 
    message("yes")
else ()
    message("no")
endif()

add_library(io_thread_libs STATIC
    ${CMAKE_SOURCE_DIR}/src/common/config.cpp
    ${CMAKE_SOURCE_DIR}/src/common/log.cpp
    ${CMAKE_SOURCE_DIR}/src/common/utils.cpp
    ${CMAKE_SOURCE_DIR}/src/net/time/time_event.cpp
    ${CMAKE_SOURCE_DIR}/src/net/time/timer.cpp
    ${CMAKE_SOURCE_DIR}/src/net/eventloop.cpp
    ${CMAKE_SOURCE_DIR}/src/net/fd_event.cpp
    ${CMAKE_SOURCE_DIR}/src/net/wakeup_fd_event.cpp
    ${CMAKE_SOURCE_DIR}/src/net/io_thread/io_thread.cpp
    ${CMAKE_SOURCE_DIR}/src/net/io_thread/io_thread_group.cpp
)
# 依赖
target_link_libraries(io_thread_group PRIVATE io_thread_libs)
target_link_libraries(io_thread_group PRIVATE ${TINYXML})
target_link_libraries(io_thread_group PRIVATE ${LIBFMT})

add_test(NAME test COMMAND io_thread_group) # 这个是添加一个子cmake目录。
