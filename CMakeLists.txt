cmake_minimum_required(VERSION 3.22.1)

project(tiny_rpc)
set(CMAKE_CXX_STANDARD 23)

add_executable(tiny_rpc 
    main.cpp 
    src/common/log.cpp 
    src/common/utils.cpp 
    src/common/config.cpp
    src/net/eventloop.cpp
    src/net/fd_event.cpp
    src/net/wakeup_fd_event.cpp)

add_executable(event_loop_test
    test/event_loop_test.cpp
    src/common/log.cpp 
    src/common/utils.cpp 
    src/common/config.cpp
    src/net/eventloop.cpp
    src/net/fd_event.cpp
    src/net/wakeup_fd_event.cpp) 

set(CMAKE_CXX_FLAGS "-g") # 调试的可以添加cpp编译选项


target_link_libraries(event_loop_test
    "/usr/local/lib/libfmt.a" # fmt
    "/usr/local/lib/tinyxml/libtinyxml.a") # tinyxml
    # "/usr/local/lib/libgtest.a") # gtest

target_link_libraries(tiny_rpc
"/usr/local/lib/libfmt.a" # fmt
"/usr/local/lib/tinyxml/libtinyxml.a") # tinyxml
# "/usr/local/lib/libgtest.a") # gtest