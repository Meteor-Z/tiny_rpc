cmake_minimum_required(VERSION 3.22.1)

project(tiny_rpc)
set(CMAKE_CXX_STANDARD 23)


# # 设置要链接的文件
# set(libtinyxml "${CMAKE_SOURCE_DIR}/third_party/lib/tinyxml/libtinyxml.a")
# set(libfmt "/usr/local/lib/libfmt.a")

# include_directories("third_party/include") # 增加搜索的头文件

# set(all_libraries
#     src/common/log.cpp 
#     src/common/utils.cpp 
#     src/common/config.cpp
#     src/net/eventloop.cpp
#     src/net/fd_event.cpp
#     src/net/wakeup_fd_event.cpp
#     src/net/time/time_event.cpp
#     src/net/time/timer.cpp
# )
# add_executable (tiny_rpc
#     main.cpp
#     ${all_libraries}
# )

# target_include_directories(tiny_rpc PUBLIC src/include) # 组织代码结构 并且只是这个生成文件
# target_link_libraries(tiny_rpc
#     ${libtinyxml}
#     ${libfmt}
# )   

add_subdirectory(test/io_thread_test)