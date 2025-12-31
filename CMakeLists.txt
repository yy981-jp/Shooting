# set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_BUILD_TYPE Releases)



cmake_minimum_required(VERSION 3.16)
project(Shooting)
add_compile_options(-fdiagnostics-color=always)

set(CMAKE_CXX_STANDARD 23)

# Use SDL2 instead of Qt
find_package(SDL2 REQUIRED)
find_package(SDL2_image REQUIRED)

set(y9inc "c:/msys64/mingw64/include/yy981")

# ソースファイル
set(SOURCES
	Shooting.cpp
	def.cpp
	BezierMover.cpp
	basic.entity.cpp
	custom.entity.cpp
	player.cpp
	create.cpp
	eventloop.cpp
	main.cpp
	
	# ${y9inc}/thread.cpp
)

# ヘッダーファイル（必要ならUIやQ_OBJECTを含むもの）
set(HEADERS
)

add_compile_options(-fexec-charset=cp932)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR})

add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS})
target_include_directories(${PROJECT_NAME} PRIVATE "${CMAKE_SOURCE_DIR}" ${SDL2_INCLUDE_DIRS})

# Link SDL2
target_link_libraries(${PROJECT_NAME} PRIVATE SDL2::SDL2main SDL2::SDL2 SDL2_image::SDL2_image)
