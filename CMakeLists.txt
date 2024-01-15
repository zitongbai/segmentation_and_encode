cmake_minimum_required(VERSION 3.26)
project(segmentation_and_encode C)

set(CMAKE_C_STANDARD 99)
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR})

# add ./include to include path
include_directories(include)

add_library(dip
        src/dip.c)

add_library(huffman
        src/huffman.c)

add_executable(segmentation_and_encode
        main.c)

target_link_libraries(segmentation_and_encode dip huffman)