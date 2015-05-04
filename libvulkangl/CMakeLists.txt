project(libvulkangl)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=c99 -Wall -Wno-unused-variable -Wno-unused-function -g3")

include_directories(include)

add_library(vulkangl SHARED src/main.c src/vulkan.c src/cmdbuffer.c src/cmdinternal.c src/shader.c src/memory.c src/framebuffer.c src/queue.c src/dummy_vk.c)
target_link_libraries(vulkangl GL)

INSTALL(TARGETS vulkangl ARCHIVE DESTINATION /usr/lib LIBRARY DESTINATION /usr/lib)
INSTALL(FILES include/vulkan.h DESTINATION include/vulkangl)
