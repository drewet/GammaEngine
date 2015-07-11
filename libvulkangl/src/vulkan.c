#include "internal.h"

static int glext_loaded = 0;
static void load_glext();


VK_RESULT vkCreateInstance(const VK_APPLICATION_INFO* pAppInfo, const VK_ALLOC_CALLBACKS* pAllocCb, VK_INSTANCE* pInst)
{
	printf("%s()\n", __PRETTY_FUNCTION__);

	if ( glext_loaded == 0 ) {
		load_glext();
		glext_loaded = 1;
	}

	vk_Instance* inst = (vk_Instance*)pAllocCb->pfnAlloc( sizeof(vk_Instance), 16, 0 );
	memset( inst, 0, sizeof( *inst ) );
	inst->pAppInfo = pAppInfo;
	inst->pAllocCb = pAllocCb;
	*pInst = (uint64_t)inst;

	return VK_SUCCESS;
}


VK_RESULT vkEnumerateGpus(VK_INSTANCE inst, VK_SIZE arraySize, VK_UINT* pGpuCount, VK_PHYSICAL_GPU gpus[])
{
	printf("%s()\n", __PRETTY_FUNCTION__);

	gpus[0] = (VK_PHYSICAL_GPU)((vk_Instance*)inst)->pAllocCb->pfnAlloc( sizeof(vk_PhysicalDevice), 16, 0 );
	memset( (void*)gpus[0], 0, sizeof(vk_PhysicalDevice) );
	((vk_PhysicalDevice*)gpus[0])->instance = (vk_Instance*)inst;
	*pGpuCount = 1;

	return VK_SUCCESS;
	
}


#ifdef __linux
VK_RESULT vkWsiX11AssociateConnection(VK_PHYSICAL_GPU pGpu, VK_CONNECTION_INFO* pConnectionInfo)
{
	printf("%s()\n", __PRETTY_FUNCTION__);

	const int attributes[] = {
		GLX_RGBA,
		GLX_DOUBLEBUFFER,
		GLX_RED_SIZE, 8,
		GLX_GREEN_SIZE, 8,
		GLX_BLUE_SIZE, 8,
		GLX_ALPHA_SIZE, 8,
		GLX_DEPTH_SIZE, 16,
		None
	};

	vk_PhysicalDevice* gpu = (vk_PhysicalDevice*)pGpu;
	gpu->dpy = (Display*)pConnectionInfo->dpy;
	gpu->screen = pConnectionInfo->screen;
	gpu->vi = glXChooseVisual( gpu->dpy, gpu->screen, (int*)attributes );
	gpu->ctx = glXCreateContext( gpu->dpy, gpu->vi, 0, true );
// 	glXMakeCurrent( gpu->dpy, DefaultRootWindow( gpu->dpy ), gpu->ctx );
	glXMakeCurrent( gpu->dpy, pConnectionInfo->window, gpu->ctx );

	glEnable( GL_DEPTH_TEST );
	glPointSize( 4.0f );

	return VK_SUCCESS;
}
#endif


VK_RESULT vkCreateDevice(VK_PHYSICAL_GPU gpu, const VK_DEVICE_CREATE_INFO* pCreateInfo, VK_DEVICE* pDevice)
{
	printf("%s()\n", __PRETTY_FUNCTION__);

	vk_Device* dev = (vk_Device*)((vk_PhysicalDevice*)gpu)->instance->pAllocCb->pfnAlloc( sizeof(vk_Device), 16, 0 );
	memset( dev, 0, sizeof( *dev ) );
	memcpy( dev, pCreateInfo, sizeof(VK_DEVICE_CREATE_INFO) );
	dev->dev = (vk_PhysicalDevice*)gpu;
	*pDevice = (uint64_t)dev;

	return VK_SUCCESS;
}


VK_RESULT vkGetDeviceQueue(VK_DEVICE device, VK_ENUM queueType, VK_UINT queueId, VK_QUEUE* pQueue)
{
	printf("%s()\n", __PRETTY_FUNCTION__);
	
	vk_Queue* queue = (vk_Queue*)devAlloc->pfnAlloc( sizeof(vk_Queue), 16, 0 );
	memset( queue, 0, sizeof( *queue ) );
	queue->device = (vk_Device*)device;
	pthread_mutex_init( &queue->mutex, NULL );
	*pQueue = (uint64_t)queue;

	return VK_SUCCESS;
}


VK_RESULT vkCreateFence(VK_DEVICE device, const VK_FENCE_CREATE_INFO* pCreateInfo, VK_FENCE* pFence)
{
	printf("%s()\n", __PRETTY_FUNCTION__);

	vk_Fence* fence = (vk_Fence*)devAlloc->pfnAlloc( sizeof(vk_Fence), 16, 0 );
	memset( fence, 0, sizeof( *fence ) );
	fence->device = (vk_Device*)device;
	*pFence = (uint64_t)fence;

	return VK_SUCCESS;
}


static void load_glext()
{
#ifdef _WIN32
	#include <windows.h>
	#define load_func(name) name = (void*)GetProcAddress(hOpenGL, #name); if ( !name ) { name = (void*)wglGetProcAddress(#name); }
	HMODULE hOpenGL = LoadLibrary("opengl32.dll");
	load_func( glActiveTexture );
	load_func( glGenVertexArrays );
	load_func( glBindVertexArray );
	load_func( glEnableVertexAttribArray );
	load_func( glDisableVertexAttribArray );
	load_func( glVertexAttribPointer );
	load_func( glVertexAttribIPointer );
	load_func( glGenBuffers );
	load_func( glDeleteBuffers );
	load_func( glBindBuffer );
	load_func( glBufferData );
	load_func( glBufferSubData );
	load_func( glGetBufferParameteriv );
	load_func( glBlitFramebuffer );
	load_func( glGenRenderbuffers );
	load_func( glBindRenderbuffer );
	load_func( glRenderbufferStorageMultisample );
	load_func( glFramebufferRenderbuffer );
	load_func( glGenFramebuffers );
	load_func( glDeleteFramebuffers );
	load_func( glBindFramebuffer );
	load_func( glFramebufferTexture );
	load_func( glFramebufferTexture2D );
	load_func( glFramebufferTexture3D );
	load_func( glFramebufferTextureLayer );
	load_func( glDrawBuffers );
	load_func( glCreateShader );
	load_func( glShaderSource );
	load_func( glCompileShader );
	load_func( glAttachShader );
	load_func( glGetShaderInfoLog );
	load_func( glDeleteShader );
	load_func( glDeleteProgram );
	load_func( glCreateProgram );
	load_func( glLinkProgram );
	load_func( glUseProgram );
	load_func( glGetProgramInfoLog );
	load_func( glBindAttribLocation );
	load_func( glGetUniformLocation );
	load_func( glGetAttribLocation );
	load_func( glUniform1i );
	load_func( glUniform2i );
	load_func( glUniform3i );
	load_func( glUniform4i );
	load_func( glUniform1f );
	load_func( glUniform2f );
	load_func( glUniform3f );
	load_func( glUniform4f );
	load_func( glUniform1fv );
	load_func( glUniform2fv );
	load_func( glUniform3fv );
	load_func( glUniform4fv );
	load_func( glUniform1iv );
	load_func( glUniform2iv );
	load_func( glUniform3iv );
	load_func( glUniform4iv );
	load_func( glUniformMatrix3fv );
	load_func( glUniformMatrix4fv );
	load_func( glGetUniformfv );
	load_func( glPatchParameteri );
	load_func( glBindBufferBase );
	load_func( glUniformBlockBinding );
	load_func( glMapBuffer );
	load_func( glUnmapBuffer );
#endif
}
