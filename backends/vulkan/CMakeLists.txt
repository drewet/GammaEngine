cmake_minimum_required(VERSION 2.6)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -Wall -Wno-unknown-pragmas -Wno-unused-but-set-variable -Wno-unused-variable -I./fake -g3")

include_directories(include)
include_directories(/usr/local/include/vulkangl)

add_library(backend_vulkan
	SHARED
	src/VulkanObject.cpp
	src/VulkanRenderer.cpp
	src/VulkanWindow.cpp
	src/VulkanInstance.cpp
)

SET_TARGET_PROPERTIES(backend_vulkan PROPERTIES PREFIX "")
target_link_libraries(backend_vulkan -lvulkangl -lX11 -pthread -ldl)
