add_executable(tcp_channel_server tcp_channel_server_test.cc)
add_executable(tcp_channel_client tcp_channel_client_test.cc)

include_directories(${CMAKE_SOURCE_DIR}/src/include)

# 这个是tinyxml的头文件
include_directories(${CMAKE_SOURCE_DIR}/third_party/tinyxml/include)
# 这个是tinyxml的lib库
find_library(TINYXML tinyxml ${CMAKE_SOURCE_DIR}/third_party/tinyxml/lib)

target_include_directories(tcp_channel_server PRIVATE ${CMAKE_SOURCE_DIR}/third_party/fmt/include)
target_include_directories(tcp_channel_client PRIVATE ${CMAKE_SOURCE_DIR}/third_party/fmt/include)

add_library(tcp_channel_libs STATIC
    ${CMAKE_SOURCE_DIR}/src/common/config.cc
    ${CMAKE_SOURCE_DIR}/src/common/log.cc
    ${CMAKE_SOURCE_DIR}/src/common/utils.cc
    ${CMAKE_SOURCE_DIR}/src/common/mutex.cc
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
    ${CMAKE_SOURCE_DIR}/src/net/tcp/tcp_acceptor.cc
    ${CMAKE_SOURCE_DIR}/src/net/tcp/tcp_connection.cc
    ${CMAKE_SOURCE_DIR}/src/net/tcp/tcp_buffer.cc
    ${CMAKE_SOURCE_DIR}/src/net/coder/protobuf_coder.cc
    ${CMAKE_SOURCE_DIR}/src/net/rpc/rpc_dispatchor.cc
    ${CMAKE_SOURCE_DIR}/src/net/rpc/rpc_controller.cc
    ${CMAKE_SOURCE_DIR}/src/net/rpc/rpc_closure.cc
    ${CMAKE_SOURCE_DIR}/src/net/tcp/tcp_client.cc
    ${CMAKE_SOURCE_DIR}/src/net/tcp/tcp_server.cc
    ${CMAKE_SOURCE_DIR}/src/net/rpc/rpc_channel.cc
    ${CMAKE_SOURCE_DIR}/src/common/msg_id_utils.cc
    order.pb.cc
)
find_package(Protobuf REQUIRED)


# Tcp Server的
target_link_libraries(tcp_channel_server PRIVATE tcp_channel_libs)
target_link_libraries(tcp_channel_server PRIVATE ${TINYXML})
target_link_libraries(tcp_channel_server PRIVATE fmt)
target_link_libraries(tcp_channel_server PRIVATE ${Protobuf_LIBRARIES})

# Tcp Client 的
target_link_libraries(tcp_channel_client PRIVATE tcp_channel_libs)
target_link_libraries(tcp_channel_client PRIVATE ${TINYXML})
target_link_libraries(tcp_channel_client PRIVATE fmt)
target_link_libraries(tcp_channel_client PRIVATE ${Protobuf_LIBRARIES})

message("库的位置：" ${Protobuf_LIBRARIES})
message("protobuf版本：" ${Protobuf_VERSION})

add_test(NAME tcp_channel_server COMMAND tcp_channel_server) # 这个是添加一个子cmake目录。
