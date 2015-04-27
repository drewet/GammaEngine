cmake_minimum_required(VERSION 2.6)
project(gammaengine)

find_package(PNG REQUIRED)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -Wall -Wno-unknown-pragmas -I./fake -g3")

include_directories(include)
include_directories(fake)

add_executable(gammaengine
	src/main.cpp
	src/gememory.cpp
	src/Instance.cpp
	src/Renderer.cpp
	src/Object.cpp
	src/ObjectLoaderObj.cpp
	src/Image.cpp
	src/ImageLoaderPng.cpp
	src/Vertex.cpp
	src/Vector.cpp
	src/Matrix.cpp
	src/File.cpp
	src/Debug.cpp
	fake/dummy_vk.cpp)

target_link_libraries(gammaengine ${PNG_LIBRARIES})

install(TARGETS gammaengine RUNTIME DESTINATION bin)
