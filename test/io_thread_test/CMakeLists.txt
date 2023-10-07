
add_executable(io_thread_test test_io_thread_group.cpp)

# message(WARNING ${CMAKE_SOURCE_DIR})
set(libtinyxml ${CMAKE_SOURCE_DIR}/third_party/lib/tinyxml/libtinyxml.a)
set(libfmt /usr/local/lib/libfmt.a)



# include_directories(${CMAKE_SOURCE_DIR}/third_party/include) # 增加搜索的头文件
target_include_directories(io_thread_test PRIVATE ${CMAKE_SOURCE_DIR}/src/include/) # 增加一个搜索头文件


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

add_test(NAME io_thread_test COMMAND io_thread_test)
