cmake_minimum_required(VERSION 2.6)
set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/CMakeModules")
project(gammaengine)

if(NOT IOS)
find_package(OpenGL REQUIRED)
endif()
find_package(PNG16 REQUIRED)
find_package(JPEG REQUIRED)
find_package(Freetype REQUIRED)

include_directories(${FREETYPE_INCLUDE_DIRS})
include_directories(${PNG16_INCLUDE_DIR})

option(target "target")
option(backend "backend")

set(mode SHARED)

if(${target} MATCHES OFF)
	if(WIN32)
		set(target windows)
	elseif(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
		set(target mac)
	elseif(UNIX)
		set(target linux)
	endif()
endif()

add_definitions(-DGE_LIB)
if( NOT ${backend} MATCHES OFF )
	add_definitions(-DGE_STATIC_BACKEND=${backend})
endif()

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -Wall -Wno-unknown-pragmas -Wno-unused-variable -g3")
if(NOT IOS)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-unused-but-set-variable")
endif()

include_directories(.)
include_directories(include)
# include_directories(fake)
# include_directories(/usr/local/include/vulkangl)
include_directories(libvulkangl/include)

set( CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR} )
set( CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR} )
set( CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR} )

if( ${backend} MATCHES OFF )
	add_subdirectory(backends/vulkan)
	add_subdirectory(backends/opengl43)
	add_subdirectory(backends/opengles20)
else()
	file(GLOB SOURCES backends/${backend}/src/*)
	include_directories(backends/${backend}/include)
	set(LIBS ${OPENGL_LIBRARIES})
endif()


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

set(SOURCES ${SOURCES}
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

set(LIBS ${LIBS}
	${PNG16_LIBRARIES}
	${JPEG_LIBRARIES}
	${FREETYPE_LIBRARIES}
	-pthread
)

if("${target}" MATCHES "windows")
	add_definitions(-DGE_WIN32)
	include_directories(include/windows)
	set(LIBS ${LIBS} ${ZLIB_LIBRARY} -lwinmm)
	set(SOURCES ${SOURCES} src/windows/BaseWindow.cpp)
elseif("${target}" MATCHES "ios")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x -stdlib=libc++ -I/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/include/c++/v1 -D__IPHONE_OS_VERSION_MIN_REQUIRED=030103")
#	set(CMAKE_CXX_FLAGS "-fobjc-abi-version=2 -fobjc-arc -nostdinc -I${CMAKE_IOS_SDK_ROOT}/usr/include -I${CMAKE_IOS_SDK_ROOT}/usr/include/c++/4.2.1 ${CMAKE_CXX_FLAGS} -Wno-unused-private-field -Wno-overloaded-virtual")
	set(CMAKE_CXX_FLAGS "-fobjc-abi-version=2 -fobjc-arc -nostdinc -I${CMAKE_IOS_SDK_ROOT}/usr/include ${CMAKE_CXX_FLAGS} -Wno-unused-private-field -Wno-overloaded-virtual -Wno-ignored-attributes -Wno-missing-braces")
	add_definitions(-DGE_IOS)
	include_directories(include/ios)
	set(SOURCES ${SOURCES} src/ios/BaseWindow.cpp)
	set(SOURCES ${SOURCES} src/ios/AppDelegate.mm)
	set(mode STATIC)
else()
	add_definitions(-DGE_LINUX)
	include_directories(include/linux)
	set(LIBS ${LIBS} ${OPENGL_LIBRARIES} -lX11 -lXext -ldl)
	set(SOURCES ${SOURCES} src/linux/BaseWindow.cpp)
endif()

message(STATUS "LIBS : ${LIBS}")

add_library(gammaengine "${mode}" ${SOURCES})
target_link_libraries(gammaengine ${LIBS})

if(NOT ("${target}" MATCHES "ios" OR "${target}" MATCHES "android"))
add_executable(testexec src/main.cpp)
target_link_libraries(testexec gammaengine ${LIBS})
endif()

INSTALL(TARGETS gammaengine ARCHIVE DESTINATION /usr/lib LIBRARY DESTINATION /usr/lib)
INSTALL(FILES ${INCLUDES} DESTINATION /usr/include/gammaengine)
# INSTALL(FILES ${INCLUDES_TARGET} DESTINATION /usr/include/gammaengine/${target})
INSTALL(FILES ${INCLUDES_TARGET} DESTINATION /usr/include/gammaengine)
