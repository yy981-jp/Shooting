cmake_minimum_required(VERSION 3.16)
project(Shooting)
add_compile_options(-fdiagnostics-color=always)

set(CMAKE_CXX_STANDARD 23)

# Qt6を使う（必要に応じてWidgetsなど追加）
find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets OpenGLWidgets)

# Qt用の自動MOCを有効にする
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

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
target_include_directories(${PROJECT_NAME} PRIVATE "C:/msys64/mingw64/include/qt6")

# Qt6のライブラリをリンク
target_link_libraries(${PROJECT_NAME}
	Qt6Core
	Qt6Gui
	Qt6Widgets
	Qt6OpenGLWidgets
)
