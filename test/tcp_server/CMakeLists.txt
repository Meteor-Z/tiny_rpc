add_executable(tcp_server tcp_server_test.cpp)

include_directories(${CMAKE_SOURCE_DIR}/src/include)
include_directories(${CMAKE_SOURCE_DIR}/third_party/include)

# 找到依赖库
find_library(LIBFMT fmt /usr/local/lib)
find_library(TINYXML tinyxml ${CMAKE_SOURCE_DIR}/third_party/lib/tinyxml)

# 如果找到了这个库
if (LIBFMT)
    message(STATUS "找到了这个库 libfmt: ${LIBFMT}")
else ()
    message(FATAL_ERROR "libfmt not found")
endif ()

# 如果找到了这个库
if (TINYXML)
    message(STATUS "找到了这个库: ${TINYXML}")
else ()
    message(FATAL_ERROR "这个库没有找到")
endif ()

add_library(tcp_server_libs STATIC
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
    ${CMAKE_SOURCE_DIR}/src/net/tcp/net_addr.cpp
    ${CMAKE_SOURCE_DIR}/src/net/tcp/tcp_server.cpp
    ${CMAKE_SOURCE_DIR}/src/net/tcp/net_addr.cpp
    ${CMAKE_SOURCE_DIR}/src/net/tcp/tcp_acceptor.cpp
)
# 依赖
target_link_libraries(tcp_server PRIVATE tcp_server_libs)
target_link_libraries(tcp_server PRIVATE ${TINYXML})
target_link_libraries(tcp_server PRIVATE ${LIBFMT})

add_test(NAME tcp_server COMMAND tcp_server) # 这个是添加一个子cmake目录。
