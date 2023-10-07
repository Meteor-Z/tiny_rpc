add_executable(io_thread_test test_io_thread_group.cpp)

target_link_libraries(io_thread_test
    ${libtinyxml}
    ${libfmt}
    tinyr
)

add_test(NAME test COMMAND io_thread_test)
