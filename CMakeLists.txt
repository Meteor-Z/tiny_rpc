cmake_minimum_required(VERSION 3.22.1)

project(tiny_rpc)

add_executable(tiny_rpc)

target_link_libraries(tiny_rpc "/usr/local/lib/libfmt.a")