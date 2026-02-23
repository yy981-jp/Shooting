cmake_minimum_required(VERSION 3.20...4.1)
project(ST_pos) # Shooting
add_compile_options(-fdiagnostics-color=always)

set(CMAKE_CXX_STANDARD 23)
# set(CMAKE_EXPORT_COMPILE_COMMANDS ON)


file(GLOB_RECURSE SOURCES src/*.cpp)

add_executable(${PROJECT_NAME} ${SOURCES})

find_package(PkgConfig REQUIRED)
pkg_check_modules(SDL2 REQUIRED sdl2)
pkg_check_modules(SDL2_IMAGE REQUIRED SDL2_image)

target_include_directories(${PROJECT_NAME} PRIVATE
    ${SDL2_INCLUDE_DIRS}
    ${SDL2_IMAGE_INCLUDE_DIRS}
)

target_link_libraries(${PROJECT_NAME} PRIVATE
    ${SDL2_LIBRARIES}
    ${SDL2_IMAGE_LIBRARIES}
)

target_compile_options(${PROJECT_NAME} PRIVATE
	$<$<CONFIG:Release>:-O3>
	$<$<CONFIG:RelWithDebInfo>:-O3 -g>
)
