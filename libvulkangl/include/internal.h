#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <GL/gl.h>
#ifndef _WIN32
#define GL_GLEXT_PROTOTYPES
#endif
#include <GL/glext.h>

#ifdef __linux
#include <GL/glx.h>
#include <X11/Xlib.h>
#endif // __linux

#include "vulkan.h"

#define devAlloc ((vk_Device*)device)->dev->instance->pAllocCb


typedef struct vk_Instance {
	const VK_APPLICATION_INFO* pAppInfo;
	const VK_ALLOC_CALLBACKS* pAllocCb;
} vk_Instance;

#ifdef __linux
typedef struct vk_PhysicalDevice {
	vk_Instance* instance;
	Display* dpy;
	int screen;
	GLXContext ctx;
	XVisualInfo* vi;
} vk_PhysicalDevice;
#elif defined(WIN32)
typedef struct vk_PhysicalDevice {
	vk_Instance* instance;
} vk_PhysicalDevice;
#endif

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




#ifdef _WIN32
#define define_proc(base, name) PFNGL##base##PROC name
define_proc(ACTIVETEXTURE, glActiveTexture);
define_proc(GENVERTEXARRAYS, glGenVertexArrays);
define_proc(BINDVERTEXARRAY, glBindVertexArray);
define_proc(ENABLEVERTEXATTRIBARRAY, glEnableVertexAttribArray);
define_proc(DISABLEVERTEXATTRIBARRAY, glDisableVertexAttribArray);
define_proc(VERTEXATTRIBPOINTER, glVertexAttribPointer);
define_proc(VERTEXATTRIBIPOINTER, glVertexAttribIPointer);
define_proc(GENBUFFERS, glGenBuffers);
define_proc(DELETEBUFFERS, glDeleteBuffers);
define_proc(BINDBUFFER, glBindBuffer);
define_proc(BUFFERDATA, glBufferData);
define_proc(BUFFERSUBDATA, glBufferSubData);
define_proc(GETBUFFERPARAMETERIV, glGetBufferParameteriv);
define_proc(BLITFRAMEBUFFER, glBlitFramebuffer);
define_proc(GENRENDERBUFFERS, glGenRenderbuffers);
define_proc(BINDRENDERBUFFER, glBindRenderbuffer);
define_proc(RENDERBUFFERSTORAGEMULTISAMPLE, glRenderbufferStorageMultisample);
define_proc(FRAMEBUFFERRENDERBUFFER, glFramebufferRenderbuffer);
define_proc(GENFRAMEBUFFERS, glGenFramebuffers);
define_proc(DELETEFRAMEBUFFERS, glDeleteFramebuffers);
define_proc(BINDFRAMEBUFFER, glBindFramebuffer);
define_proc(FRAMEBUFFERTEXTURE, glFramebufferTexture);
define_proc(FRAMEBUFFERTEXTURE2D, glFramebufferTexture2D);
define_proc(FRAMEBUFFERTEXTURE3D, glFramebufferTexture3D);
define_proc(FRAMEBUFFERTEXTURELAYER, glFramebufferTextureLayer);
define_proc(DRAWBUFFERS, glDrawBuffers);
define_proc(CREATESHADER, glCreateShader);
define_proc(SHADERSOURCE, glShaderSource);
define_proc(COMPILESHADER, glCompileShader);
define_proc(ATTACHSHADER, glAttachShader);
define_proc(GETSHADERINFOLOG, glGetShaderInfoLog);
define_proc(DELETESHADER, glDeleteShader);
define_proc(DELETEPROGRAM, glDeleteProgram);
define_proc(CREATEPROGRAM, glCreateProgram);
define_proc(LINKPROGRAM, glLinkProgram);
define_proc(USEPROGRAM, glUseProgram);
define_proc(GETPROGRAMINFOLOG, glGetProgramInfoLog);
define_proc(BINDATTRIBLOCATION, glBindAttribLocation);
define_proc(GETUNIFORMLOCATION, glGetUniformLocation);
define_proc(GETATTRIBLOCATION, glGetAttribLocation);
define_proc(UNIFORM1I, glUniform1i);
define_proc(UNIFORM2I, glUniform2i);
define_proc(UNIFORM3I, glUniform3i);
define_proc(UNIFORM4I, glUniform4i);
define_proc(UNIFORM1F, glUniform1f);
define_proc(UNIFORM2F, glUniform2f);
define_proc(UNIFORM3F, glUniform3f);
define_proc(UNIFORM4F, glUniform4f);
define_proc(UNIFORM1FV, glUniform1fv);
define_proc(UNIFORM2FV, glUniform2fv);
define_proc(UNIFORM3FV, glUniform3fv);
define_proc(UNIFORM4FV, glUniform4fv);
define_proc(UNIFORM1IV, glUniform1iv);
define_proc(UNIFORM2IV, glUniform2iv);
define_proc(UNIFORM3IV, glUniform3iv);
define_proc(UNIFORM4IV, glUniform4iv);
define_proc(UNIFORMMATRIX3FV, glUniformMatrix3fv);
define_proc(UNIFORMMATRIX4FV, glUniformMatrix4fv);
define_proc(GETUNIFORMFV, glGetUniformfv);
define_proc(PATCHPARAMETERI, glPatchParameteri);
define_proc(BINDBUFFERBASE, glBindBufferBase);
define_proc(UNIFORMBLOCKBINDING, glUniformBlockBinding);
define_proc(MAPBUFFER, glMapBuffer);
define_proc(UNMAPBUFFER, glUnmapBuffer);
#endif
