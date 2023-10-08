cmake_minimum_required(VERSION 3.22.1)

project(aaa)
set(CMAKE_CXX_STANDARD 23)

add_executable(io_thread_test test_io_thread_group.cpp)

message(检测路径 ${CMAKE_SOURCE_DIR}/src/common/config.cpp)
# set(nihao
#     ${CMAKE_SOURCE_DIR}/src/common/config.cpp
#     ${CMAKE_SOURCE_DIR}/src/common/log.cpp
#     ${CMAKE_SOURCE_DIR}/src/common/utils.cpp
#     ${CMAKE_SOURCE_DIR}/src/net/io_thread.cpp
# )


include_directories(${CMAKE_SOURCE_DIR}/src/include)
include_directories(${CMAKE_SOURCE_DIR}/third_party/include)

find_library(LIBFMT fmt /usr/local/lib)
find_library(TINYXML tinyxml ${CMAKE_SOURCE_DIR}/third_party/lib/tinyxml)

if (LIBFMT)
    message(STATUS "Found libfmt: ${LIBFMT}")
else ()
    message(FATAL_ERROR "libfmt not found")
endif ()



if (TINYXML)
    message(STATUS "Found libfmt: ${TINYXML}")
else ()
    message(FATAL_ERROR "libfmt not found")
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
target_link_libraries(io_thread_test    
    io_thread_libs
    ${TINYXML}
    ${LIBFMT}
    
)

add_test(NAME test COMMAND io_thread_test)
