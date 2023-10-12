# cmake_minimum_required(VERSION 3.22.1)

# project()
# set(CMAKE_CXX_STANDARD 23)

add_executable(io_thread_group io_thread_group_test.cpp)

# message(检测路径 ${CMAKE_SOURCE_DIR}/src/common/config.cpp)

include_directories(${CMAKE_SOURCE_DIR}/src/include)
include_directories(${CMAKE_SOURCE_DIR}/third_party/include)

# 找到这个库，然后直接安装就行了
find_library(LIBFMT fmt /usr/local/lib)
find_library(TINYXML tinyxml ${CMAKE_SOURCE_DIR}/third_party/lib/tinyxml)

if (LIBFMT)
    message(STATUS "找到了这个库 libfmt: ${LIBFMT}")
else ()
    message(FATAL_ERROR "libfmt not found")
endif ()



if (TINYXML)
    message(STATUS "找到了这个库: ${TINYXML}")
else ()
    message(FATAL_ERROR "这个库没有找到")
endif ()

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
