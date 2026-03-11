cmake_minimum_required(VERSION 3.20...4.1)
project(ST) # Shooting
add_compile_options(-fdiagnostics-color=always)

set(CMAKE_CXX_STANDARD 23)
# set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
# set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR})


file(GLOB_RECURSE SOURCES src/*.cpp)

add_executable(${PROJECT_NAME} ${SOURCES})

target_include_directories(${PROJECT_NAME}
    SYSTEM PRIVATE ${CMAKE_SOURCE_DIR}/engine/external
)

add_custom_target(buildinfo ALL
    COMMAND ${CMAKE_COMMAND} -P ${CMAKE_SOURCE_DIR}/cmake/gen_buildinfo.cmake
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
)

if(EMSCRIPTEN)

	# wasm出力をhtmlにする
	set(CMAKE_EXECUTABLE_SUFFIX ".html")

    target_compile_options(${PROJECT_NAME} PRIVATE
        -sUSE_SDL=2
        -sUSE_SDL_IMAGE=2
        "-sSDL2_IMAGE_FORMATS=['png']"
        -Wno-deprecated-declarations
        -Wno-nontrivial-memcall
    )

    # Emscripten用リンクオプション
	target_link_options(${PROJECT_NAME} PRIVATE
		-sUSE_SDL=2
		-sUSE_SDL_IMAGE=2
		-sSDL2_IMAGE_FORMATS=["png"]
		--preload-file ${CMAKE_SOURCE_DIR}/../assets@/assets
	)
    
else()

    find_package(PkgConfig REQUIRED)
    pkg_check_modules(SDL2 REQUIRED sdl2)
    pkg_check_modules(SDL2_IMAGE REQUIRED SDL2_image)
    pkg_check_modules(SDL2_MIXER REQUIRED SDL2_mixer)
    pkg_check_modules(SDL2_TTF REQUIRED SDL2_ttf)

    target_include_directories(${PROJECT_NAME} PRIVATE
        ${SDL2_INCLUDE_DIRS}
        ${SDL2_IMAGE_INCLUDE_DIRS}
        ${SDL2_MIXER_INCLUDE_DIRS}
        ${SDL2_TTF_INCLUDE_DIRS}
    )

    target_link_libraries(${PROJECT_NAME} PRIVATE
        ${SDL2_LIBRARIES}
        ${SDL2_IMAGE_LIBRARIES}
        ${SDL2_MIXER_LIBRARIES}
        ${SDL2_TTF_LIBRARIES}
    )

    target_compile_options(${PROJECT_NAME} PRIVATE
		$<$<CONFIG:Release>:-O3>
		$<$<CONFIG:RelWithDebInfo>:-O3 -g>
	)

endif()
