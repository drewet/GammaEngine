#include "vulkan.h"


VK_RESULT vkCreateInstance(
	const VK_APPLICATION_INFO* pAppInfo,
	const VK_ALLOC_CALLBACKS* pAllocCb,
	VK_INSTANCE* pInst){ return VK_SUCCESS; }

VK_RESULT vkEnumerateGpus(
	VK_INSTANCE inst,
	VK_SIZE arraySize,
	VK_UINT* pGpuCount,
	VK_PHYSICAL_GPU gpus[VK_MAX_PHYSICAL_GPUS]){ return VK_SUCCESS; }

VK_RESULT vkGetGpuInfo(
	VK_PHYSICAL_GPU gpu,
	VK_ENUM infoType,
	VK_SIZE* pDataSize,
	VK_VOID* pData){ return VK_SUCCESS; }

VK_RESULT vkGetExtensionSupport(
	VK_PHYSICAL_GPU gpu,
	const VK_CHAR* pExtName){ return VK_SUCCESS; }

VK_RESULT vkCreateDevice(
	VK_PHYSICAL_GPU gpu,
	const VK_DEVICE_CREATE_INFO* pCreateInfo,
	VK_DEVICE* pDevice){ return VK_SUCCESS; }

VK_RESULT vkWsiWinGetDisplays(
	VK_DEVICE device,
	VK_UINT* pDisplayCount,
	VK_WSI_WIN_DISPLAY* pDisplayList){ return VK_SUCCESS; }

VK_RESULT vkWsiWinGetDisplayModeList(
	VK_WSI_WIN_DISPLAY display,
	VK_UINT* pDisplayModeCount,
	VK_WSI_WIN_DISPLAY_MODE* pDisplayModeList){ return VK_SUCCESS; }

VK_RESULT vkGetDeviceQueue(
	VK_DEVICE device,
	VK_ENUM queueType,
	VK_UINT queueId,
	VK_QUEUE* pQueue){ return VK_SUCCESS; }

VK_RESULT vkWsiWinCreatePresentableImage(
	VK_DEVICE device,
	const VK_WSI_WIN_PRESENTABLE_IMAGE_CREATE_INFO* pCreateInfo,
	VK_IMAGE* pImage,
	VK_GPU_MEMORY* pMem){ return VK_SUCCESS; }

VK_RESULT vkCreateCommandBuffer(
	VK_DEVICE device,
	const VK_CMD_BUFFER_CREATE_INFO* pCreateInfo,
	VK_CMD_BUFFER* pCmdBuffer){ return VK_SUCCESS; }

VK_RESULT vkBeginCommandBuffer(
	VK_CMD_BUFFER cmdBuffer,
	VK_FLAGS flags){ return VK_SUCCESS; }

VK_RESULT vkEndCommandBuffer(
	VK_CMD_BUFFER cmdBuffer){ return VK_SUCCESS; }

VK_RESULT vkQueueSubmit(
	VK_QUEUE queue,
	VK_UINT cmdBufferCount,
	const VK_CMD_BUFFER* pCmdBuffers,
	VK_UINT memRefCount,
	const VK_MEMORY_REF* pMemRefs,
	VK_FENCE fence){ return VK_SUCCESS; }

void vkCmdPrepareImages(
	VK_CMD_BUFFER cmdBuffer,
	VK_UINT transitionCount,
	const VK_IMAGE_STATE_TRANSITION* pStateTransitions){}

void vkCmdClearColorImage(
	VK_CMD_BUFFER cmdBuffer,
	VK_IMAGE image,
	const VK_FLOAT color[4],
	VK_UINT rangeCount,
	const VK_IMAGE_SUBRESOURCE_RANGE* pRanges){}

#ifdef WIN32
VK_RESULT vkWsiWinQueuePresent(
	VK_QUEUE queue,
	const VK_WSI_WIN_PRESENT_INFO* pPresentInfo){}
#endif

VK_RESULT vkDbgRegisterMsgCallback(
	VK_DBG_MSG_CALLBACK_FUNCTION pfnMsgCallback,
	VK_VOID* pUserData){ return VK_SUCCESS; }

VK_RESULT vkCreateViewportState(
	VK_DEVICE device,
	const VK_VIEWPORT_STATE_CREATE_INFO* pCreateInfo,
	VK_VIEWPORT_STATE_OBJECT* pState){ return VK_SUCCESS; }

VK_RESULT vkGetMemoryHeapCount(
	VK_DEVICE device,
	VK_UINT* pCount){ return VK_SUCCESS; }

VK_RESULT vkCreateColorTargetView(
	VK_DEVICE device,
	const VK_COLOR_TARGET_VIEW_CREATE_INFO* pCreateInfo,
	VK_COLOR_TARGET_VIEW* pView){ return VK_SUCCESS; }

VK_RESULT vkGetMemoryHeapInfo(
	VK_DEVICE device,
	VK_UINT heapId,
	VK_ENUM infoType,
	VK_SIZE* pDataSize,
	VK_VOID* pData){ return VK_SUCCESS; }

VK_RESULT vkAllocMemory(
	VK_DEVICE device,
	const VK_MEMORY_ALLOC_INFO* pAllocInfo,
	VK_GPU_MEMORY* pMem){ return VK_SUCCESS; }

VK_RESULT vkCreateDescriptorSet(
	VK_DEVICE device,
	const VK_DESCRIPTOR_SET_CREATE_INFO* pCreateInfo,
	VK_DESCRIPTOR_SET* pDescriptorSet){ return VK_SUCCESS; }

VK_RESULT vkGetObjectInfo(
	VK_BASE_OBJECT object,
	VK_ENUM infoType,
	VK_SIZE* pDataSize,
	VK_VOID* pData){ return VK_SUCCESS; }

VK_RESULT vkBindObjectMemory(
	VK_OBJECT object,
	VK_GPU_MEMORY mem,
	VK_GPU_SIZE offset){ return VK_SUCCESS; }

VK_RESULT vkCreateFence(
	VK_DEVICE device,
	const VK_FENCE_CREATE_INFO* pCreateInfo,
	VK_FENCE* pFence){ return VK_SUCCESS; }

VK_RESULT vkCreateMsaaState(
	VK_DEVICE device,
	const VK_MSAA_STATE_CREATE_INFO* pCreateInfo,
	VK_MSAA_STATE_OBJECT* pState){ return VK_SUCCESS; }

VK_RESULT vkCreateSampler(
	VK_DEVICE device,
	const VK_SAMPLER_CREATE_INFO* pCreateInfo,
	VK_SAMPLER* pSampler){ return VK_SUCCESS; }

void vkBeginDescriptorSetUpdate(
	VK_DESCRIPTOR_SET descriptorSet){}

void vkEndDescriptorSetUpdate(
	VK_DESCRIPTOR_SET descriptorSet){}

void vkAttachSamplerDescriptors(
	VK_DESCRIPTOR_SET descriptorSet,
	VK_UINT startSlot,
	VK_UINT slotCount,
	const VK_SAMPLER* pSamplers){}

VK_RESULT vkCreateColorBlendState(
	VK_DEVICE device,
	const VK_COLOR_BLEND_STATE_CREATE_INFO* pCreateInfo,
	VK_COLOR_BLEND_STATE_OBJECT* pState){ return VK_SUCCESS; }

VK_RESULT vkCreateDepthStencilState(
	VK_DEVICE device,
	const VK_DEPTH_STENCIL_STATE_CREATE_INFO* pCreateInfo,
	VK_DEPTH_STENCIL_STATE_OBJECT* pState){ return VK_SUCCESS; }

VK_RESULT vkCreateRasterState(
	VK_DEVICE device,
	const VK_RASTER_STATE_CREATE_INFO* pCreateInfo,
	VK_RASTER_STATE_OBJECT* pState){ return VK_SUCCESS; }

VK_RESULT vkMapMemory(
	VK_GPU_MEMORY mem,
	VK_FLAGS flags,
	VK_VOID** ppData){ return VK_SUCCESS; }

VK_RESULT vkUnmapMemory(
	VK_GPU_MEMORY mem){ return VK_SUCCESS; }

VK_RESULT vkCreateShader(
	VK_DEVICE device,
	const VK_SHADER_CREATE_INFO* pCreateInfo,
	VK_SHADER* pShader){ return VK_SUCCESS; }

VK_RESULT vkCreateGraphicsPipeline(
	VK_DEVICE device,
	const VK_GRAPHICS_PIPELINE_CREATE_INFO* pCreateInfo,
	VK_PIPELINE* pPipeline){ return VK_SUCCESS; }

VK_RESULT vkCreateComputePipeline(
	VK_DEVICE device,
	const VK_COMPUTE_PIPELINE_CREATE_INFO* pCreateInfo,
	VK_PIPELINE* pPipeline){ return VK_SUCCESS; }

void vkClearDescriptorSetSlots(
	VK_DESCRIPTOR_SET descriptorSet,
	VK_UINT startSlot,
	VK_UINT slotCount){}

void vkAttachMemoryViewDescriptors(
	VK_DESCRIPTOR_SET descriptorSet,
	VK_UINT startSlot,
	VK_UINT slotCount,
	const VK_MEMORY_VIEW_ATTACH_INFO* pMemViews){}

VK_RESULT vkWaitForFences(
	VK_DEVICE device,
	VK_UINT fenceCount,
	const VK_FENCE* pFences,
	VK_BOOL waitAll,
	VK_FLOAT timeout){ return VK_SUCCESS; }

VK_RESULT vkCreateImage(
	VK_DEVICE device,
	const VK_IMAGE_CREATE_INFO* pCreateInfo,
	VK_IMAGE* pImage){ return VK_SUCCESS; }

VK_RESULT vkDestroyObject(
	VK_OBJECT object){ return VK_SUCCESS; }

VK_RESULT vkCreateImageView(
	VK_DEVICE device,
	const VK_IMAGE_VIEW_CREATE_INFO* pCreateInfo,
	VK_IMAGE_VIEW* pView){ return VK_SUCCESS; }

void vkAttachImageViewDescriptors(
	VK_DESCRIPTOR_SET descriptorSet,
	VK_UINT startSlot,
	VK_UINT slotCount,
	const VK_IMAGE_VIEW_ATTACH_INFO* pImageViews){}

VK_RESULT vkGetImageSubresourceInfo(
	VK_IMAGE image,
	const VK_IMAGE_SUBRESOURCE* pSubresource,
	VK_ENUM infoType,
	VK_SIZE* pDataSize,
	VK_VOID* pData){ return VK_SUCCESS; }

VK_RESULT vkResetCommandBuffer(
	VK_CMD_BUFFER cmdBuffer){ return VK_SUCCESS; }

void vkCmdPrepareMemoryRegions(
	VK_CMD_BUFFER cmdBuffer,
	VK_UINT transitionCount,
	const VK_MEMORY_STATE_TRANSITION* pStateTransitions){}

void vkCmdCopyMemory(
	VK_CMD_BUFFER cmdBuffer,
	VK_GPU_MEMORY srcMem,
	VK_GPU_MEMORY destMem,
	VK_UINT regionCount,
	const VK_MEMORY_COPY* pRegions){}

void vkCmdCopyImage(
	VK_CMD_BUFFER cmdBuffer,
	VK_IMAGE srcImage,
	VK_IMAGE destImage,
	VK_UINT regionCount,
	const VK_IMAGE_COPY* pRegions){}

void vkCmdBindTargets(
	VK_CMD_BUFFER cmdBuffer,
	VK_UINT colorTargetCount,
	const VK_COLOR_TARGET_BIND_INFO* pColorTargets,
	const VK_DEPTH_STENCIL_BIND_INFO* pDepthTarget){}

void vkCmdBindStateObject(
	VK_CMD_BUFFER cmdBuffer,
	VK_ENUM stateBindPoint,
	VK_STATE_OBJECT state){}

VK_RESULT vkFreeMemory(
	VK_GPU_MEMORY mem){ return VK_SUCCESS; }

void vkCmdCopyMemoryToImage(
	VK_CMD_BUFFER cmdBuffer,
	VK_GPU_MEMORY srcMem,
	VK_IMAGE destImage,
	VK_UINT regionCount,
	const VK_MEMORY_IMAGE_COPY* pRegions){}

void vkAttachNestedDescriptors(
	VK_DESCRIPTOR_SET descriptorSet,
	VK_UINT startSlot,
	VK_UINT slotCount,
	const VK_DESCRIPTOR_SET_ATTACH_INFO* pNestedDescriptorSets){}

void vkCmdBindDescriptorSet(
	VK_CMD_BUFFER cmdBuffer,
	VK_ENUM pipelineBindPoint,
	VK_UINT index,
	VK_DESCRIPTOR_SET descriptorSet,
	VK_UINT slotOffset){}

void vkCmdBindIndexData(
	VK_CMD_BUFFER cmdBuffer,
	VK_GPU_MEMORY mem,
	VK_GPU_SIZE offset,
	VK_ENUM indexType){}

void vkCmdBindPipeline(
	VK_CMD_BUFFER cmdBuffer,
	VK_ENUM pipelineBindPoint,
	VK_PIPELINE pipeline){}

void vkCmdDrawIndexed(
	VK_CMD_BUFFER cmdBuffer,
	VK_UINT firstIndex,
	VK_UINT indexCount,
	VK_INT vertexOffset,
	VK_UINT firstInstance,
	VK_UINT instanceCount){}

void vkCmdBindDynamicMemoryView(
	VK_CMD_BUFFER cmdBuffer,
	VK_ENUM pipelineBindPoint,
	const VK_MEMORY_VIEW_ATTACH_INFO* pMemView){}

void vkCmdDispatch(
	VK_CMD_BUFFER cmdBuffer,
	VK_UINT x,
	VK_UINT y,
	VK_UINT z){}

void vkCmdDraw(
	VK_CMD_BUFFER cmdBuffer,
	VK_UINT firstVertex,
	VK_UINT vertexCount,
	VK_UINT firstInstance,
	VK_UINT instanceCount){}
