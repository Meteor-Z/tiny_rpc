# 测试 event_loop
add_executable(event_loop event_loop_test.cpp)

include_directories(${CMAKE_SOURCE_DIR}/src/include)
include_directories(${CMAKE_SOURCE_DIR}/third_party/include)

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

add_test(NAME event_loop COMMAND event_loop) # 这个是添加一个子cmake目录。
