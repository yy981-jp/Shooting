cmake_minimum_required(VERSION 3.20...4.1)
project(ST) # Shooting
add_compile_options(-fdiagnostics-color=always -fconcepts-diagnostics-depth=2)

set(CMAKE_CXX_STANDARD 23)

file(GLOB_RECURSE SOURCES src/*.cpp)

add_executable(${PROJECT_NAME} ${SOURCES})

target_link_libraries(${PROJECT_NAME} PRIVATE
    SDL2
    SDL2_image
)

target_compile_options(${PROJECT_NAME} PRIVATE
	$<$<CONFIG:Release>:-O3>
	$<$<CONFIG:RelWithDebInfo>:-O3 -g>
)
