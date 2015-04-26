cmake_minimum_required(VERSION 2.6)
project(gammaengine)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -Wall -g3")

include_directories(include)
include_directories(fake)

add_executable(gammaengine
	src/main.cpp
	src/gememory.cpp
	src/Instance.cpp
	src/Renderer.cpp
	src/Object.cpp
	src/ObjectLoaderObj.cpp
	src/Vertex.cpp
	src/Vector.cpp
	src/File.cpp
	src/Debug.cpp
	fake/dummy_vk.cpp)

install(TARGETS gammaengine RUNTIME DESTINATION bin)
