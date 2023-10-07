
add_executable(io_thread_test test_io_thread_group.cpp)

set(libs
    ${CMAKE_SOURCE_DIR}/src/common/log.cpp 
    ${CMAKE_SOURCE_DIR}/src/common/utils.cpp 
    ${CMAKE_SOURCE_DIR}/src/common/config.cpp
    ${CMAKE_SOURCE_DIR}/src/net/eventloop.cpp
    ${CMAKE_SOURCE_DIR}/src/net/fd_event.cpp
    ${CMAKE_SOURCE_DIR}/src/net/wakeup_fd_event.cpp
    ${CMAKE_SOURCE_DIR}/src/net/time/time_event.cpp
    ${CMAKE_SOURCE_DIR}/src/net/time/timer.cpp
)
target_link_libraries(io_thread_test
    ${libtinyxml}
    ${libfmt}
    ${libs}
)

add_test(NAME test COMMAND io_thread_test)
