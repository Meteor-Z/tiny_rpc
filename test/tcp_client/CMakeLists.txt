add_executable(tcp_client tcp_client_test.cc)

include_directories(${CMAKE_SOURCE_DIR}/src/include)

# 这个是tinyxml的头文件
include_directories(${CMAKE_SOURCE_DIR}/third_party/tinyxml/include)

# 这个是tinyxml的lib库
find_library(TINYXML tinyxml ${CMAKE_SOURCE_DIR}/third_party/tinyxml/lib)


add_library(tcp_client_libs STATIC
    ${CMAKE_SOURCE_DIR}/src/common/config.cc
    ${CMAKE_SOURCE_DIR}/src/common/log.cc
    ${CMAKE_SOURCE_DIR}/src/common/utils.cc
    ${CMAKE_SOURCE_DIR}/src/net/time/time_event.cc
    ${CMAKE_SOURCE_DIR}/src/net/time/timer.cc
    ${CMAKE_SOURCE_DIR}/src/net/eventloop.cc
    ${CMAKE_SOURCE_DIR}/src/net/fd_event/fd_event.cc
    ${CMAKE_SOURCE_DIR}/src/net/fd_event/fd_event_group.cc
    ${CMAKE_SOURCE_DIR}/src/net/wakeup_fd_event.cc
    ${CMAKE_SOURCE_DIR}/src/net/io_thread/io_thread.cc
    ${CMAKE_SOURCE_DIR}/src/net/io_thread/io_thread_group.cc
    ${CMAKE_SOURCE_DIR}/src/net/tcp/ipv4_net_addr.cc
    ${CMAKE_SOURCE_DIR}/src/net/tcp/tcp_server.cc
    ${CMAKE_SOURCE_DIR}/src/net/tcp/ipv4_net_addr.cc
    ${CMAKE_SOURCE_DIR}/src/net/tcp/tcp_acceptor.cc
    ${CMAKE_SOURCE_DIR}/src/net/tcp/tcp_connection.cc
    ${CMAKE_SOURCE_DIR}/src/net/tcp/tcp_buffer.cc
)
# 依赖
target_link_libraries(tcp_client PRIVATE tcp_client_libs)
target_link_libraries(tcp_client PRIVATE ${TINYXML})
target_link_libraries(tcp_client PRIVATE fmt)

add_test(NAME tcp_client COMMAND tcp_client) # 这个是添加一个子cmake目录。
