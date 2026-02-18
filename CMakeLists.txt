cmake_minimum_required(VERSION 3.20)
project(ST_root)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/build)

add_subdirectory(engine)
add_subdirectory(tools/pos)
