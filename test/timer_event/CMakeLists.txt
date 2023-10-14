add_executable(timer_event timer_event_test.cpp)

include_directories(${CMAKE_SOURCE_DIR}/src/include)

# 这个是tinyxml的头文件
include_directories(${CMAKE_SOURCE_DIR}/third_party/tinyxml/include)

# 这个是tinyxml的lib库
find_library(TINYXML tinyxml ${CMAKE_SOURCE_DIR}/third_party/tinyxml/lib)


add_library(timer_event_libs STATIC
    ${CMAKE_SOURCE_DIR}/src/common/config.cpp
    ${CMAKE_SOURCE_DIR}/src/common/log.cpp
    ${CMAKE_SOURCE_DIR}/src/common/utils.cpp
    ${CMAKE_SOURCE_DIR}/src/net/time/time_event.cpp
    ${CMAKE_SOURCE_DIR}/src/net/time/timer.cpp
    ${CMAKE_SOURCE_DIR}/src/net/eventloop.cpp
    ${CMAKE_SOURCE_DIR}/src/net/fd_event.cpp
    ${CMAKE_SOURCE_DIR}/src/net/wakeup_fd_event.cpp
    # ${CMAKE_SOURCE_DIR}/src/net/io_thread/io_thread.cpp
    # ${CMAKE_SOURCE_DIR}/src/net/io_thread/io_thread_group.cpp
)
# 依赖
target_link_libraries(timer_event PRIVATE timer_event_libs)
target_link_libraries(timer_event PRIVATE ${TINYXML})
target_link_libraries(timer_event PRIVATE ${LIBFMT})

add_test(NAME timer_event_test COMMAND timer_event) # 这个是添加一个子cmake目录。
