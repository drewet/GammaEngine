cmake_minimum_required(VERSION 2.6)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -Wall -Wno-unknown-pragmas -Wno-unused-but-set-variable -Wno-unused-variable -I./fake -g3")

include_directories(include)

add_library(backend_opengl43
	SHARED
	src/OpenGL43Object.cpp
	src/OpenGL43Renderer.cpp
	src/OpenGL43Renderer2D.cpp
	src/OpenGL43DeferredRenderer.cpp
	src/OpenGL43Window.cpp
	src/OpenGL43Instance.cpp
)

SET_TARGET_PROPERTIES(backend_opengl43 PROPERTIES PREFIX "")
target_link_libraries(backend_opengl43 -lvulkangl -lX11 -pthread -ldl)
