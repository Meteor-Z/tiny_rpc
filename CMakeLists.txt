cmake_minimum_required(VERSION 3.22.1)

project(tiny_rpc)
set(CMAKE_CXX_STANDARD 23)


# 设置链接的时候的别名
set(libtinyxml "${CMAKE_SOURCE_DIR}/third_party/lib/tinyxml/libtinyxml.a")
set(libfmt "/usr/local/lib/libfmt.a")

include_directories(${CMAKE_SOURCE_DIR}/src/include)
include_directories(${CMAKE_SOURCE_DIR}/third_party/include)

set(all_libraries
    src/common/log.cpp 
    src/common/utils.cpp 
    src/common/config.cpp
    src/net/eventloop.cpp
    src/net/fd_event.cpp
    src/net/wakeup_fd_event.cpp
    src/net/time/time_event.cpp
    src/net/time/timer.cpp
)
add_executable (tiny_rpc
    main.cpp
    ${all_libraries}
)
target_link_libraries(tiny_rpc
    ${libtinyxml}
    ${libfmt}
)   

add_subdirectory(test/io_thread_test)

