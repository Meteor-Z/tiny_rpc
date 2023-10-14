add_executable(timer_event timer_event_test.cpp)

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
