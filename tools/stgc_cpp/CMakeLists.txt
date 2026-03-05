cmake_minimum_required(VERSION 3.20...4.1)
project(stgc) # StageFlow Compiler
add_compile_options(-fdiagnostics-color=always)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
# set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR})


file(GLOB_RECURSE SOURCES main.cpp)

add_executable(${PROJECT_NAME} ${SOURCES})

target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_SOURCE_DIR}/engine
)

target_compile_options(${PROJECT_NAME} PRIVATE
    $<$<CONFIG:Release>:-O3>
    $<$<CONFIG:RelWithDebInfo>:-O3 -g>
)
