cmake_minimum_required(VERSION 2.6)
project(gammaengine)

find_package(OpenGL REQUIRED)
find_package(PNG REQUIRED)
find_package(JPEG REQUIRED)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -Wall -Wno-unknown-pragmas -Wno-unused-but-set-variable -Wno-unused-variable -I./fake -g3")

include_directories(include)
# include_directories(fake)
include_directories(/usr/local/include/vulkangl)

add_subdirectory(backends/vulkan)
add_subdirectory(backends/opengl43)

add_executable(gammaengine
	src/main.cpp
# 	src/gememory.cpp
	src/Instance.cpp
	src/Light.cpp
	src/Scene.cpp
	src/Camera.cpp
	src/PhysicalBody.cpp
	src/PhysicalGraph.cpp
	src/PhysicalCollider.cpp
	src/MeshBuilder.cpp
	src/Object.cpp
	src/ObjectLoaderObj.cpp
	src/Image.cpp
	src/ImageLoaderPng.cpp
	src/ImageLoaderJpg.cpp
	src/Vertex.cpp
	src/Vector.cpp
	src/Quaternion.cpp
	src/Matrix.cpp
	src/File.cpp
	src/Time.cpp
	src/Input.cpp
	src/Thread.cpp
	src/Debug.cpp
	src/linux/BaseWindow.cpp
# 	fake/dummy_vk.cpp
)

target_link_libraries(gammaengine -lge ${PNG_LIBRARIES} ${JPEG_LIBRARIES} ${OPENGL_LIBRARIES} -lvulkangl -lX11 -pthread -ldl)

install(TARGETS gammaengine RUNTIME DESTINATION bin)
