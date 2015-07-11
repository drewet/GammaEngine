cmake_minimum_required(VERSION 2.6)
project(gammaengine)

find_package(OpenGL REQUIRED)
find_package(PNG REQUIRED)
find_package(JPEG REQUIRED)
find_package(Freetype REQUIRED)

include_directories(${FREETYPE_INCLUDE_DIRS})

option(target "target")

if(${target} MATCHES OFF)
	if(WIN32)
		set(target windows)
	elseif(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
		set(target mac)
	elseif(UNIX)
		set(target linux)
	endif()
endif()

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


set(INCLUDES
	include/Camera.h
	include/Debug.h
	include/DeferredRenderer.h
	include/File.h
	include/Font.h
	include/gememory.h
	include/Image.h
	include/ImageLoaderJpg.h
	include/ImageLoaderPng.h
	include/Input.h
	include/Instance.h
	include/Light.h
	include/Matrix.h
	include/MeshBuilder.h
	include/Object.h
	include/ObjectLoaderObj.h
	include/PhysicalBody.h
	include/PhysicalCollider.h
	include/PhysicalGraph.h
	include/Quaternion.h
	include/Renderer2D.h
	include/Renderer.h
	include/Scene.h
	include/SkyRenderer.h
	include/Thread.h
	include/Time.h
	include/Vector.h
	include/Vertex.h
	include/Window.h
)

set(INCLUDES_TARGET
	include/${target}/BaseWindow.h
)

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
	src/Font.cpp
	src/FontLoaderTtf.cpp
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
	${FREETYPE_LIBRARIES}
	-pthread
)

if("${target}" MATCHES "windows")
	add_definitions(-DGE_WIN32)
	include_directories(include/windows)
	set(LIBS ${LIBS} -lwinmm)
	set(SOURCES ${SOURCES} src/windows/BaseWindow.cpp)
else()
	add_definitions(-DGE_LINUX)
	include_directories(include/linux)
	set(LIBS ${LIBS} ${OPENGL_LIBRARIES} -lX11 -ldl)
	set(SOURCES ${SOURCES} src/linux/BaseWindow.cpp)
endif()

add_library(gammaengine SHARED ${SOURCES})
target_link_libraries(gammaengine ${LIBS})

add_executable(testexec src/main.cpp)
target_link_libraries(testexec gammaengine)

INSTALL(TARGETS gammaengine ARCHIVE DESTINATION /usr/lib LIBRARY DESTINATION /usr/lib)
INSTALL(FILES ${INCLUDES} DESTINATION /usr/include/gammaengine)
# INSTALL(FILES ${INCLUDES_TARGET} DESTINATION /usr/include/gammaengine/${target})
INSTALL(FILES ${INCLUDES_TARGET} DESTINATION /usr/include/gammaengine)
