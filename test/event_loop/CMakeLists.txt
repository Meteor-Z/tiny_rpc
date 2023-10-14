# 测试 event_loop
add_executable(event_loop event_loop_test.cpp)

include_directories(${CMAKE_SOURCE_DIR}/src/include) # 自身的头文件

# 这个是tinyxml的头文件
include_directories(${CMAKE_SOURCE_DIR}/third_party/tinyxml/include)

# 这个是tinyxml的lib库
find_library(TINYXML tinyxml ${CMAKE_SOURCE_DIR}/third_party/tinyxml/lib)

add_library(event_loop_libs STATIC
    ${CMAKE_SOURCE_DIR}/src/common/config.cpp
    ${CMAKE_SOURCE_DIR}/src/common/log.cpp
    ${CMAKE_SOURCE_DIR}/src/common/utils.cpp
    ${CMAKE_SOURCE_DIR}/src/net/time/time_event.cpp
    ${CMAKE_SOURCE_DIR}/src/net/time/timer.cpp
    ${CMAKE_SOURCE_DIR}/src/net/eventloop.cpp
    ${CMAKE_SOURCE_DIR}/src/net/fd_event.cpp
    ${CMAKE_SOURCE_DIR}/src/net/wakeup_fd_event.cpp
)

# 依赖
target_link_libraries(event_loop PRIVATE event_loop_libs)
target_link_libraries(event_loop PRIVATE ${TINYXML})
target_link_libraries(event_loop PRIVATE ${LIBFMT})
target_link_directories(event_loop PRIVATE fmt)

add_test(NAME event_loop COMMAND event_loop) # 这个是添加一个子cmake目录。
