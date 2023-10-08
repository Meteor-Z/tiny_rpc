cmake_minimum_required(VERSION 3.22.1)

project(tiny_rpc)
set(CMAKE_CXX_STANDARD 23)


# # 设置链接的时候的别名

# set(libtinyxml "${CMAKE_SOURCE_DIR}/third_party/lib/tinyxml/libtinyxml.a")
# set(libfmt "/usr/lib/libfmt.so")

# include_directories(${CMAKE_SOURCE_DIR}/src/include)
# include_directories(${CMAKE_SOURCE_DIR}/third_party/include)

# # file( GLOB_RECURSE LIB_SOURCES src/**.cpp )
# # file( GLOB_RECURSE LIB_HEADERS src/**.hpp )

# # add_library(tinyr ${LIB_SOURCES} ${LIB_HEADERS})
# # target_link_libraries(tinyr
# #     ${libtinyxml}
# #     ${libfmt}
# # )

# add_executable(tiny_rpc main.cpp)
# target_link_libraries(tiny_rpc
#     ${libtinyxml}
#     ${libfmt}
#     tinyr
# )

add_subdirectory(test/io_thread_test)
