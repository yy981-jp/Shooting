cmake_minimum_required(VERSION 3.20)
project(ST_root)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/build)

add_subdirectory(engine)

if(NOT EMSCRIPTEN)
    add_subdirectory(tools/pos)
else()
    message(STATUS "EMSCRIPTEN build: skipping tools/pos subdirectory")
endif()
