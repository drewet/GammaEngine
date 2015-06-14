cmake_minimum_required(VERSION 2.6)
project(gammaengine)

#if(NOT WIN32)
	find_package(PNG REQUIRED)
	find_package(JPEG REQUIRED)
#endif()

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -Wall -Wno-unknown-pragmas -Wno-unused-but-set-variable -Wno-unused-variable -g3")

include_directories(include)
# include_directories(fake)
# include_directories(/usr/local/include/vulkangl)
include_directories(libvulkangl/include)

set( CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR} )
set( CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR} )
set( CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR} )
add_subdirectory(backends/vulkan)
add_subdirectory(backends/opengl43)

set(SOURCES
# 	src/gememory.cpp
	src/Instance.cpp
	src/Light.cpp
	src/Scene.cpp
	src/Camera.cpp
	src/PhysicalBody.cpp
	src/PhysicalGraph.cpp
	src/PhysicalCollider.cpp
	src/SkyRenderer.cpp
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
# 	fake/dummy_vk.cpp
)

set(LIBS
	${PNG_LIBRARIES}
	${JPEG_LIBRARIES}
	-pthread
)

if(WIN32)
	add_definitions(-DGE_WIN32)
	include_directories(include/windows)
	set(LIBS ${LIBS} -lwinmm)
	set(SOURCES ${SOURCES} src/windows/BaseWindow.cpp)
else()
	add_definitions(-DGE_LINUX)
	include_directories(include/linux)
	set(LIBS ${LIBS} -lX11 -ldl)
	set(SOURCES ${SOURCES} src/linux/BaseWindow.cpp)
endif()

add_library(gammaengine SHARED ${SOURCES})
target_link_libraries(gammaengine ${LIBS})

add_executable(testexec src/main.cpp)
target_link_libraries(testexec gammaengine)
