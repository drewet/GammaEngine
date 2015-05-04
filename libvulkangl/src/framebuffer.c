#include "internal.h"

VK_RESULT vkCreateColorTargetView(VK_DEVICE device, const VK_COLOR_TARGET_VIEW_CREATE_INFO* pCreateInfo, VK_COLOR_TARGET_VIEW* pView)
{
	printf("%s()\n", __PRETTY_FUNCTION__);

	vk_ColorTargetView* targetView = (vk_ColorTargetView*)devAlloc->pfnAlloc( sizeof(vk_ColorTargetView), 16, 0 );
	memcpy( targetView, pCreateInfo, sizeof(vk_ColorTargetView) );
	*pView = (uint64_t)targetView;

	return VK_SUCCESS;
}


VK_RESULT vkWsiWinCreatePresentableImage(VK_DEVICE device, const VK_WSI_WIN_PRESENTABLE_IMAGE_CREATE_INFO* pCreateInfo, VK_IMAGE* pImage, VK_GPU_MEMORY* pMem)
{
	printf("%s()\n", __PRETTY_FUNCTION__);

	vk_Image* image = (vk_Image*)devAlloc->pfnAlloc( sizeof(vk_Image), 16, 0 );
	memset( image, 0, sizeof( *image ) );
	image->device = (vk_Device*)device;
	image->state = VK_MEMORY_STATE_UNINITIALIZED;
	image->width = pCreateInfo->extent.width;
	image->height = pCreateInfo->extent.height;
	*pImage = (uint64_t)image;


	glGenFramebuffers( 1, &image->glId );
	glBindFramebuffer( GL_FRAMEBUFFER, image->glId );
/*
	glGenRenderbuffers( 1, &image->colorId );
	glBindRenderbuffer( GL_RENDERBUFFER, image->colorId );
	glRenderbufferStorage(GL_RENDERBUFFER, GL_RGBA8, pCreateInfo->extent.width, pCreateInfo->extent.height );
	glFramebufferRenderbuffer( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, image->colorId );

	glGenRenderbuffers( 1, &image->depthId );
	glBindRenderbuffer( GL_RENDERBUFFER, image->depthId );
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, pCreateInfo->extent.width, pCreateInfo->extent.height );
	glFramebufferRenderbuffer( GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, image->depthId );
*/

	glGenTextures( 1, &image->colorId );
	glBindTexture( GL_TEXTURE_2D, image->colorId );
	glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA8, pCreateInfo->extent.width, pCreateInfo->extent.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL );
	glFramebufferTexture2D( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, image->colorId, 0 );

	glGenTextures( 1, &image->depthId );
	glBindTexture( GL_TEXTURE_2D, image->depthId );
	glTexImage2D( GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, pCreateInfo->extent.width, pCreateInfo->extent.height, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE, NULL );
	glFramebufferTexture2D( GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, image->depthId, 0 );

	glBindFramebuffer( GL_FRAMEBUFFER, 0 );


	vk_GpuMemory* memory = (vk_GpuMemory*)devAlloc->pfnAlloc( sizeof(vk_GpuMemory), 16, 0 );
	memset( memory, 0, sizeof( *memory ) );
	memory->device = (vk_Device*)device;
	memory->heap = 2;
	memory->size = 0; // TESTING
	memory->type = GL_FRAMEBUFFER;
	*pMem = (uint64_t)memory;

	return VK_SUCCESS;
}


VK_RESULT vkWsiWinQueuePresent(VK_QUEUE _queue, const VK_WSI_WIN_PRESENT_INFO* pPresentInfo)
{
// 	printf("%s()\n", __PRETTY_FUNCTION__);

	vk_Queue* queue = (vk_Queue*)_queue;
	vk_PhysicalDevice* gpu = queue->device->dev;

	Window win = (Window)pPresentInfo->hWndDest;
	vk_Image* image = (vk_Image*)pPresentInfo->srcImage;
	// TODO : play with pPresentInfo->presentMode

	glXMakeCurrent( gpu->dpy, win, gpu->ctx );


	_vkFlushQueue( queue );

	glBindFramebuffer( GL_FRAMEBUFFER, 0 );

	glBindFramebuffer( GL_READ_FRAMEBUFFER, image->glId );
	glReadBuffer( GL_COLOR_ATTACHMENT0 );

	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);

	glBlitFramebuffer( 0, 0, image->width, image->height, 0, 0, image->width, image->height, GL_COLOR_BUFFER_BIT, GL_NEAREST );

	glBindFramebuffer( GL_READ_FRAMEBUFFER, 0 );

	glXSwapBuffers( gpu->dpy, win );

	return VK_SUCCESS;
}
