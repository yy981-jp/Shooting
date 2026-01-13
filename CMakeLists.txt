cmake_minimum_required(VERSION 3.20...4.1)
project(ST) # Shooting
add_compile_options(-fdiagnostics-color=always)

set(CMAKE_CXX_STANDARD 23)

file(GLOB SOURCES src/*.cpp)

add_executable(${PROJECT_NAME} ${SOURCES})

target_link_libraries(${PROJECT_NAME}
    SDL2
    SDL2_image
)
