add_executable(tcp tcp_test.cc)

include_directories(${CMAKE_SOURCE_DIR}/src/include)

# 这个是tinyxml的头文件
include_directories(${CMAKE_SOURCE_DIR}/third_party/tinyxml/include)

# 这个是tinyxml的lib库
find_library(TINYXML tinyxml ${CMAKE_SOURCE_DIR}/third_party/tinyxml/lib)

target_include_directories(tcp PRIVATE ${CMAKE_SOURCE_DIR}/third_party/fmt/include)

add_library(tcp_libs STATIC
    ${CMAKE_SOURCE_DIR}/src/common/config.cc
    ${CMAKE_SOURCE_DIR}/src/common/log.cc
    ${CMAKE_SOURCE_DIR}/src/common/utils.cc
    ${CMAKE_SOURCE_DIR}/src/net/time/time_event.cc
    ${CMAKE_SOURCE_DIR}/src/net/time/timer.cc
    ${CMAKE_SOURCE_DIR}/src/net/eventloop.cc
    ${CMAKE_SOURCE_DIR}/src/net/fd_event/fd_event.cc
    ${CMAKE_SOURCE_DIR}/src/net/wakeup_fd_event.cc
    ${CMAKE_SOURCE_DIR}/src/net/tcp/ipv4_net_addr.cc


)
# 依赖
target_link_libraries(tcp PRIVATE tcp_libs)
target_link_libraries(tcp PRIVATE ${TINYXML})
target_link_libraries(tcp PRIVATE fmt)

add_test(NAME test COMMAND tcp) # 这个是添加一个子cmake目录。
