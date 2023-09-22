cmake_minimum_required(VERSION 3.22.1)

project(tiny_rpc)
set(CMAKE_CXX_STANDARD 23)


# 设置头文件
set(libtinyxml "${CMAKE_SOURCE_DIR}/third_party/include/tinyxml/libtinyxml.a")
set(libfmt "/usr/local/lib/libfmt.a")

include_directories("third_party/include") # 增加搜索的头文件

set(all_libraries
    src/common/log.cpp 
    src/common/utils.cpp 
    src/common/config.cpp
    src/net/eventloop.cpp
    src/net/fd_event.cpp
    src/net/wakeup_fd_event.cpp
    src/net/time/time_event.cpp
    src/net/time/timer.cpp
)
add_executable (tiny_rpc
    main.cpp
    ${all_libraries}
)
target_include_directories(tiny_rpc PRIVATE "src/")



# target_link_libraries(tiny_rpc
#     ${libfmt} # fmt
#     ${libtinyxml}
# ) # tinyxml


# # event_loop 
# add_executable(event_loop_test 
#     ${all_libraries}
# )

# target_link_libraries(event_loop_test
#     ${libfmt}
#     ${libtinyxml}
# )

# # 测试 timer_event_test的测试
# add_executable(timer_event_test 
#     ${all_libraries}
# )

# target_link_libraries(timer_event_test
#     ${libfmt}
#     ${libtinyxml}
# )





