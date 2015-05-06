cmake_minimum_required(VERSION 2.6)
project(gammaengine)

find_package(OpenGL REQUIRED)
find_package(PNG REQUIRED)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -Wall -Wno-unknown-pragmas -Wno-unused-but-set-variable -Wno-unused-variable -I./fake -g3")

include_directories(include)
# include_directories(fake)
include_directories(/usr/local/include/vulkangl)

add_executable(gammaengine
	src/main.cpp
	src/gememory.cpp
	src/Instance.cpp
	src/Renderer.cpp
	src/Scene.cpp
	src/Camera.cpp
	src/PhysicalObject.cpp
	src/Object.cpp
	src/ObjectLoaderObj.cpp
	src/Image.cpp
	src/ImageLoaderPng.cpp
	src/Vertex.cpp
	src/Vector.cpp
	src/Matrix.cpp
	src/File.cpp
	src/Time.cpp
	src/Input.cpp
	src/Debug.cpp
	src/Window.cpp
	src/linux/BaseWindow.cpp
# 	fake/dummy_vk.cpp
)

target_link_libraries(gammaengine ${PNG_LIBRARIES} ${OPENGL_LIBRARIES} -lvulkangl -lX11 -pthread)

install(TARGETS gammaengine RUNTIME DESTINATION bin)
