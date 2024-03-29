add_executable(tcp_server tcp_server_test.cc)

include_directories(${CMAKE_SOURCE_DIR}/src/include)

# 这个是tinyxml的头文件
include_directories(${CMAKE_SOURCE_DIR}/third_party/tinyxml/include)

# 这个是tinyxml的lib库
find_library(TINYXML tinyxml ${CMAKE_SOURCE_DIR}/third_party/tinyxml/lib)

target_include_directories(tcp_server PRIVATE ${CMAKE_SOURCE_DIR}/third_party/fmt/include)

add_library(tcp_server_libs STATIC
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
    ${CMAKE_SOURCE_DIR}/src/net/coder/protobuf_coder.cc
    ${CMAKE_SOURCE_DIR}/src/net/coder/protobuf_coder.cc
    ${CMAKE_SOURCE_DIR}/src/net/rpc/rpc_dispatchor.cc
    ${CMAKE_SOURCE_DIR}/src/net/rpc/rpc_controller.cc
    ${CMAKE_SOURCE_DIR}/src/net/rpc/rpc_closure.cc
)



# 依赖
target_link_libraries(tcp_server PRIVATE tcp_server_libs)
target_link_libraries(tcp_server PRIVATE ${TINYXML})
target_link_libraries(tcp_server PRIVATE fmt)

find_package(Protobuf REQUIRED)
target_link_libraries(tcp_server PRIVATE ${Protobuf_LIBRARY})



add_test(NAME tcp_server COMMAND tcp_server) # 这个是添加一个子cmake目录。
