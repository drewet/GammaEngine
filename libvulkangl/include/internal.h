#include <stdio.h>
#include <string.h>
#include <pthread.h>
#define GL_GLEXT_PROTOTYPES
#include <GL/gl.h>
#include <GL/glext.h>
#include <GL/glx.h>
#include <X11/Xlib.h>
#include "vulkan.h"

#define devAlloc ((vk_Device*)device)->dev->instance->pAllocCb


typedef struct vk_Instance {
	const VK_APPLICATION_INFO* pAppInfo;
	const VK_ALLOC_CALLBACKS* pAllocCb;
} vk_Instance;

typedef struct vk_PhysicalDevice {
	vk_Instance* instance;
	Display* dpy;
	int screen;
	GLXContext ctx;
	XVisualInfo* vi;
} vk_PhysicalDevice;

typedef struct vk_Device {
	// start VK_DEVICE_CREATE_INFO
	VK_UINT queueRecordCount;
	const VK_DEVICE_QUEUE_CREATE_INFO* pRequestedQueues;
	VK_UINT extensionCount;
	const VK_CHAR* const* ppEnabledExtensionNames;
	VK_ENUM maxValidationLevel;
	VK_FLAGS flags;
	// end VK_DEVICE_CREATE_INFO
	vk_PhysicalDevice* dev;
	pthread_mutex_t mutex;
} vk_Device;

typedef struct vk_GpuMemory {
	vk_Device* device;
	uint32_t heap;
	void* ptr;
	GLuint glId;
	GLuint type;
	size_t size;
} vk_GpuMemory;

typedef struct vk_CmdBuffer {
	vk_Device* device;
	uint32_t* buf;
	uint32_t iBuf;
	uint32_t maxBuf;
} vk_CmdBuffer;

typedef struct vk_QueueEntry {
	vk_CmdBuffer* buffer;
	struct vk_QueueEntry* next;
} vk_QueueEntry;

typedef struct vk_Queue {
	vk_Device* device;
	vk_QueueEntry* entries;
	pthread_mutex_t mutex;
} vk_Queue;

typedef struct vk_Fence {
	vk_Device* device;
	uint32_t counter;
} vk_Fence;

typedef struct vk_Shader {
	vk_Device* device;
	char* data;
	size_t len;
	GLuint glShader;
} vk_Shader;

typedef struct vk_ConstBuffer {
	GLuint glUbo;
	GLuint id;
	size_t size;
	void* data;
} vk_ConstBuffer;

typedef struct vk_Pipeline {
	vk_Device* device;
	vk_Shader* vertexShader;
	vk_Shader* fragmentShader;
	GLuint glProgram;
	vk_ConstBuffer* constBuffers;
	uint32_t nConstBuffers;
	int renderMode;
} vk_Pipeline;

typedef struct vk_DescriptorSetAttrib {
	uint32_t index;
	uint32_t size;
	uint32_t type;
	uint32_t stride;
	void* offset;
	vk_GpuMemory* memory;
} vk_DescriptorSetAttrib;

typedef struct vk_DescriptorSet {
	vk_Device* device;
	GLuint glId;
	vk_DescriptorSetAttrib attribs[16];
	uint32_t nAttribs;
} vk_DescriptorSet;

typedef struct vk_Image {
	vk_Device* device;
	uint32_t state;
	uint32_t width;
	uint32_t height;
	uint32_t depth;
	GLuint glId;
	GLuint colorId;
	GLuint depthId;
} vk_Image;

typedef struct vk_ColorTargetView {
	vk_Image* image;
	VK_FORMAT format;
	VK_UINT mipLevel;
	VK_UINT baseArraySlice;
	VK_UINT arraySize;
} vk_ColorTargetView;


typedef struct vk_CmdReadBuf {
	vk_Device* device;
	uint32_t* buf;
	uint32_t len;
	uint32_t i;
} vk_CmdReadBuf;

typedef struct vk_Cmd {
	uint32_t id;
	void (*f)(vk_CmdReadBuf*);
} vk_Cmd;

typedef enum vk_Cmds {
	CmdBindTarget,
	CmdBindPipeline,
	CmdBindDescriptorSet,
	CmdBindVertexBuffer,
	CmdBindIndexData,
	CmdDraw,
	CmdDrawIndexed,
	CmdClearColorImage
} vk_Cmds;

void fBindTarget( vk_CmdReadBuf* buf );
void fCmdBindPipeline( vk_CmdReadBuf* buf );
void fCmdBindDescriptorSet( vk_CmdReadBuf* buf );
void fCmdBindVertexBuffer( vk_CmdReadBuf* buf );
void fCmdBindIndexData( vk_CmdReadBuf* buf );
void fCmdDraw( vk_CmdReadBuf* buf );
void fCmdDrawIndexed( vk_CmdReadBuf* buf );
void fCmdClearColorImage( vk_CmdReadBuf* buf );

#ifdef USE_CMDS
static vk_Cmd cmds[] = {
	{ CmdBindTarget, fBindTarget },
	{ CmdBindPipeline, fCmdBindPipeline },
	{ CmdBindDescriptorSet, fCmdBindDescriptorSet },
	{ CmdBindVertexBuffer, fCmdBindVertexBuffer },
	{ CmdBindIndexData, fCmdBindIndexData },
	{ CmdDraw, fCmdDraw },
	{ CmdDrawIndexed, fCmdDrawIndexed },
	{ CmdClearColorImage, fCmdClearColorImage },
};
#endif // USE_CMDS

void _vkFlushQueue(vk_Queue* queue);
