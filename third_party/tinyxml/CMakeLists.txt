cmake_minimum_required(VERSION 3.8...3.26)

# include_directories(${CMAKE_SOURCE_DIR}/third_party/tinyxml/include)

# 找到库文件
find_library(TINYXML tinyxml ${CMAKE_SOURCE_DIR}/third_party/tinyxml/lib)
add_library(tinyxml TINYXML)

# 如果找到了库文件，将它链接到目标
if(TINYXML)
    message("tinyxml found")

    target_include_directories(tinyxml PUBLIC
${CMAKE_SOURCE_DIR}/third_party/tinyxml/include
)

else()
    message(FATAL_ERROR "tinyxml not found.")
endif ()   