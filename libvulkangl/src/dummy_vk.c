#include <stdio.h>
#include "vulkan.h"

VK_RESULT vkGetGpuInfo(
	VK_PHYSICAL_GPU gpu,
	VK_ENUM infoType,
	VK_SIZE* pDataSize,
	VK_VOID* pData){ printf("%s()\n", __PRETTY_FUNCTION__); return VK_SUCCESS; }

VK_RESULT vkGetExtensionSupport(
	VK_PHYSICAL_GPU gpu,
	const VK_CHAR* pExtName){ printf("%s()\n", __PRETTY_FUNCTION__); return VK_SUCCESS; }


VK_RESULT vkWsiWinGetDisplays(
	VK_DEVICE device,
	VK_UINT* pDisplayCount,
	VK_WSI_WIN_DISPLAY* pDisplayList){ printf("%s()\n", __PRETTY_FUNCTION__); return VK_SUCCESS; }

VK_RESULT vkWsiWinGetDisplayModeList(
	VK_WSI_WIN_DISPLAY display,
	VK_UINT* pDisplayModeCount,
	VK_WSI_WIN_DISPLAY_MODE* pDisplayModeList){ printf("%s()\n", __PRETTY_FUNCTION__); return VK_SUCCESS; }

void vkCmdPrepareImages(
	VK_CMD_BUFFER cmdBuffer,
	VK_UINT transitionCount,
	const VK_IMAGE_STATE_TRANSITION* pStateTransitions){ printf("%s()\n", __PRETTY_FUNCTION__); }

VK_RESULT vkDbgRegisterMsgCallback(
	VK_DBG_MSG_CALLBACK_FUNCTION pfnMsgCallback,
	VK_VOID* pUserData){ printf("%s()\n", __PRETTY_FUNCTION__); return VK_SUCCESS; }

VK_RESULT vkCreateViewportState(
	VK_DEVICE device,
	const VK_VIEWPORT_STATE_CREATE_INFO* pCreateInfo,
	VK_VIEWPORT_STATE_OBJECT* pState){ printf("%s()\n", __PRETTY_FUNCTION__); return VK_SUCCESS; }

VK_RESULT vkCreateMsaaState(
	VK_DEVICE device,
	const VK_MSAA_STATE_CREATE_INFO* pCreateInfo,
	VK_MSAA_STATE_OBJECT* pState){ printf("%s()\n", __PRETTY_FUNCTION__); return VK_SUCCESS; }

VK_RESULT vkCreateSampler(
	VK_DEVICE device,
	const VK_SAMPLER_CREATE_INFO* pCreateInfo,
	VK_SAMPLER* pSampler){ printf("%s()\n", __PRETTY_FUNCTION__); return VK_SUCCESS; }

void vkBeginDescriptorSetUpdate(
	VK_DESCRIPTOR_SET descriptorSet){ printf("%s()\n", __PRETTY_FUNCTION__); }

void vkEndDescriptorSetUpdate(
	VK_DESCRIPTOR_SET descriptorSet){ printf("%s()\n", __PRETTY_FUNCTION__); }

void vkAttachSamplerDescriptors(
	VK_DESCRIPTOR_SET descriptorSet,
	VK_UINT startSlot,
	VK_UINT slotCount,
	const VK_SAMPLER* pSamplers){ printf("%s()\n", __PRETTY_FUNCTION__); }

VK_RESULT vkCreateColorBlendState(
	VK_DEVICE device,
	const VK_COLOR_BLEND_STATE_CREATE_INFO* pCreateInfo,
	VK_COLOR_BLEND_STATE_OBJECT* pState){ printf("%s()\n", __PRETTY_FUNCTION__); return VK_SUCCESS; }

VK_RESULT vkCreateDepthStencilState(
	VK_DEVICE device,
	const VK_DEPTH_STENCIL_STATE_CREATE_INFO* pCreateInfo,
	VK_DEPTH_STENCIL_STATE_OBJECT* pState){ printf("%s()\n", __PRETTY_FUNCTION__); return VK_SUCCESS; }

VK_RESULT vkCreateRasterState(
	VK_DEVICE device,
	const VK_RASTER_STATE_CREATE_INFO* pCreateInfo,
	VK_RASTER_STATE_OBJECT* pState){ printf("%s()\n", __PRETTY_FUNCTION__); return VK_SUCCESS; }

VK_RESULT vkWaitForFences(
	VK_DEVICE device,
	VK_UINT fenceCount,
	const VK_FENCE* pFences,
	VK_BOOL waitAll,
	VK_FLOAT timeout){ printf("%s()\n", __PRETTY_FUNCTION__); return VK_SUCCESS; }

VK_RESULT vkCreateImage(
	VK_DEVICE device,
	const VK_IMAGE_CREATE_INFO* pCreateInfo,
	VK_IMAGE* pImage){ printf("%s()\n", __PRETTY_FUNCTION__); return VK_SUCCESS; }

VK_RESULT vkDestroyObject(
	VK_OBJECT object){ printf("%s()\n", __PRETTY_FUNCTION__); return VK_SUCCESS; }

VK_RESULT vkCreateImageView(
	VK_DEVICE device,
	const VK_IMAGE_VIEW_CREATE_INFO* pCreateInfo,
	VK_IMAGE_VIEW* pView){ printf("%s()\n", __PRETTY_FUNCTION__); return VK_SUCCESS; }

void vkAttachImageViewDescriptors(
	VK_DESCRIPTOR_SET descriptorSet,
	VK_UINT startSlot,
	VK_UINT slotCount,
	const VK_IMAGE_VIEW_ATTACH_INFO* pImageViews){ printf("%s()\n", __PRETTY_FUNCTION__); }

VK_RESULT vkGetImageSubresourceInfo(
	VK_IMAGE image,
	const VK_IMAGE_SUBRESOURCE* pSubresource,
	VK_ENUM infoType,
	VK_SIZE* pDataSize,
	VK_VOID* pData){ printf("%s()\n", __PRETTY_FUNCTION__); return VK_SUCCESS; }

VK_RESULT vkResetCommandBuffer(
	VK_CMD_BUFFER cmdBuffer){ printf("%s()\n", __PRETTY_FUNCTION__); return VK_SUCCESS; }

void vkCmdPrepareMemoryRegions(
	VK_CMD_BUFFER cmdBuffer,
	VK_UINT transitionCount,
	const VK_MEMORY_STATE_TRANSITION* pStateTransitions){ printf("%s()\n", __PRETTY_FUNCTION__); }

void vkCmdCopyMemory(
	VK_CMD_BUFFER cmdBuffer,
	VK_GPU_MEMORY srcMem,
	VK_GPU_MEMORY destMem,
	VK_UINT regionCount,
	const VK_MEMORY_COPY* pRegions){ printf("%s()\n", __PRETTY_FUNCTION__); }

void vkCmdCopyImage(
	VK_CMD_BUFFER cmdBuffer,
	VK_IMAGE srcImage,
	VK_IMAGE destImage,
	VK_UINT regionCount,
	const VK_IMAGE_COPY* pRegions){ printf("%s()\n", __PRETTY_FUNCTION__); }

VK_RESULT vkFreeMemory(
	VK_GPU_MEMORY mem){ printf("%s()\n", __PRETTY_FUNCTION__); return VK_SUCCESS; }

void vkCmdCopyMemoryToImage(
	VK_CMD_BUFFER cmdBuffer,
	VK_GPU_MEMORY srcMem,
	VK_IMAGE destImage,
	VK_UINT regionCount,
	const VK_MEMORY_IMAGE_COPY* pRegions){ printf("%s()\n", __PRETTY_FUNCTION__); }

void vkAttachNestedDescriptors(
	VK_DESCRIPTOR_SET descriptorSet,
	VK_UINT startSlot,
	VK_UINT slotCount,
	const VK_DESCRIPTOR_SET_ATTACH_INFO* pNestedDescriptorSets){ printf("%s()\n", __PRETTY_FUNCTION__); }

void vkCmdBindDynamicMemoryView(
	VK_CMD_BUFFER cmdBuffer,
	VK_ENUM pipelineBindPoint,
	const VK_MEMORY_VIEW_ATTACH_INFO* pMemView){ printf("%s()\n", __PRETTY_FUNCTION__); }

void vkCmdDispatch(
	VK_CMD_BUFFER cmdBuffer,
	VK_UINT x,
	VK_UINT y,
	VK_UINT z){ printf("%s()\n", __PRETTY_FUNCTION__); }
