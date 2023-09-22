cmake_minimum_required(VERSION 3.22.1)

project(tiny_rpc)
set(CMAKE_CXX_STANDARD 23)


# 设置头文件
set(libtinyxml "${CMAKE_SOURCE_DIR}/third_party/include/tinyxml/libtinyxml.a")
set(libfmt "/usr/local/lib/libfmt.a")

add_executable (tiny_rpc 
    main.cpp 
    src/common/log.cpp 
    src/common/utils.cpp 
    src/common/config.cpp
    src/net/eventloop.cpp
    src/net/fd_event.cpp
    src/net/wakeup_fd_event.cpp
    src/net/time/time_event.cpp
    src/net/time/timer.cpp
)


target_link_libraries(tiny_rpc
    ${libfmt} # fmt
    ${libtinyxml}
) # tinyxml


# event_loop 
add_executable(event_loop_test 
    test/event_loop_test.cpp
    src/common/log.cpp 
    src/common/utils.cpp 
    src/common/config.cpp
    src/net/eventloop.cpp
    src/net/fd_event.cpp
    src/net/wakeup_fd_event.cpp
    src/net/time/time_event.cpp
    src/net/time/timer.cpp
)

target_link_libraries(event_loop_test
    ${libfmt}
    ${libtinyxml}
)

# 测试 timer_event_test的测试
add_executable(timer_event_test 
    test/timer_event_test.cpp
    src/common/log.cpp 
    src/common/utils.cpp 
    src/common/config.cpp
    src/net/eventloop.cpp
    src/net/fd_event.cpp
    src/net/wakeup_fd_event.cpp
    src/net/time/time_event.cpp
    src/net/time/timer.cpp
)

target_link_libraries(timer_event_test
    ${libfmt}
    ${libtinyxml}
)





