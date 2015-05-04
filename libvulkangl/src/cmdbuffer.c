// #define USE_CMDS
#include "internal.h"

VK_RESULT vkCreateCommandBuffer(VK_DEVICE device, const VK_CMD_BUFFER_CREATE_INFO* pCreateInfo, VK_CMD_BUFFER* pCmdBuffer)
{
	printf("%s()\n", __PRETTY_FUNCTION__);

	vk_CmdBuffer* buffer = (vk_CmdBuffer*)devAlloc->pfnAlloc( sizeof(vk_CmdBuffer), 16, 0 );
	memset( buffer, 0, sizeof( *buffer ) );
	buffer->device = (vk_Device*)device;
	*pCmdBuffer = (uint64_t)buffer;

	return VK_SUCCESS;
}


VK_RESULT vkBeginCommandBuffer(VK_CMD_BUFFER cmdBuffer, VK_FLAGS flags)
{
	printf("%s()\n", __PRETTY_FUNCTION__);

	vk_CmdBuffer* buffer = (vk_CmdBuffer*)cmdBuffer;
	vk_Device* device = buffer->device;

	if ( buffer->buf ) {
		devAlloc->pfnFree( buffer->buf );
	}

	buffer->iBuf = 0;
	buffer->maxBuf = 1024;
	buffer->buf = (uint32_t*)devAlloc->pfnAlloc( sizeof(uint32_t) * buffer->maxBuf, 16, 0 );
	memset( buffer->buf, 0, sizeof(uint32_t) * buffer->maxBuf );

	return VK_SUCCESS; 
}


VK_RESULT vkEndCommandBuffer(VK_CMD_BUFFER cmdBuffer)
{
	printf("%s()\n", __PRETTY_FUNCTION__);
	return VK_SUCCESS; 
}


static void BufferGrow(vk_CmdBuffer* buffer, uint32_t reqSize)
{
	vk_Device* device = buffer->device;

	if ( buffer->iBuf + 1 + reqSize >= buffer->maxBuf ) {
		uint32_t* newBuf = (uint32_t*)devAlloc->pfnAlloc( sizeof(uint32_t) * ( buffer->maxBuf + 1024 ), 16, 0 );
		memcpy( newBuf, buffer->buf, sizeof(uint32_t) * buffer->maxBuf );
		devAlloc->pfnFree( buffer->buf );
		buffer->buf = newBuf;
		buffer->maxBuf += 1024;
	}
}


static void BufferU32(vk_CmdBuffer* buffer, uint32_t v)
{
	BufferGrow( buffer, 1 );
	buffer->buf[buffer->iBuf] = v;
	buffer->iBuf++;
}


static void BufferU64(vk_CmdBuffer* buffer, uint64_t v)
{
	BufferGrow( buffer, 2 );
	( (uint64_t*)&buffer->buf[buffer->iBuf] )[0] = v;
	buffer->iBuf += 2;
}


void vkCmdBindTargets(VK_CMD_BUFFER cmdBuffer, VK_UINT colorTargetCount, const VK_COLOR_TARGET_BIND_INFO* pColorTargets, const VK_DEPTH_STENCIL_BIND_INFO* pDepthTarget)
{
	printf("%s()\n", __PRETTY_FUNCTION__);

	vk_CmdBuffer* buffer = (vk_CmdBuffer*)cmdBuffer;

	BufferU32( buffer, CmdBindTarget );
	BufferU64( buffer,  pColorTargets->view );
	BufferU64( buffer,  pDepthTarget->view );
}


void vkCmdBindStateObject(VK_CMD_BUFFER cmdBuffer, VK_ENUM stateBindPoint, VK_STATE_OBJECT state)
{
	printf("%s()\n", __PRETTY_FUNCTION__);
}


void vkCmdBindDescriptorSet(VK_CMD_BUFFER cmdBuffer, VK_ENUM pipelineBindPoint, VK_UINT index, VK_DESCRIPTOR_SET descriptorSet, VK_UINT slotOffset)
{
	printf("%s()\n", __PRETTY_FUNCTION__);
	vk_CmdBuffer* buffer = (vk_CmdBuffer*)cmdBuffer;

	BufferU32( buffer, CmdBindDescriptorSet );
	BufferU32( buffer, pipelineBindPoint );
	BufferU32( buffer, index );
	BufferU64( buffer, descriptorSet );
	BufferU32( buffer, slotOffset );
}


void vkCmdBindPipeline(VK_CMD_BUFFER cmdBuffer, VK_ENUM pipelineBindPoint, VK_PIPELINE pipeline)
{
	printf("%s()\n", __PRETTY_FUNCTION__);
	vk_CmdBuffer* buffer = (vk_CmdBuffer*)cmdBuffer;

	BufferU32( buffer, CmdBindPipeline );
	BufferU32( buffer, pipelineBindPoint );
	BufferU64( buffer, pipeline );
}


void vkCmdBindVertexBuffer(VK_CMD_BUFFER cmdBuffer, VK_GPU_MEMORY mem, VK_GPU_SIZE offset, VK_GPU_SIZE unknown)
{
	printf("%s()\n", __PRETTY_FUNCTION__);
	vk_CmdBuffer* buffer = (vk_CmdBuffer*)cmdBuffer;

	BufferU32( buffer, CmdBindVertexBuffer );
	BufferU64( buffer, mem );
	BufferU64( buffer, offset );
	BufferU64( buffer, unknown );
}


void vkCmdBindIndexData(VK_CMD_BUFFER cmdBuffer, VK_GPU_MEMORY mem, VK_GPU_SIZE offset, VK_ENUM indexType)
{
	printf("%s()\n", __PRETTY_FUNCTION__);
	vk_CmdBuffer* buffer = (vk_CmdBuffer*)cmdBuffer;

	BufferU32( buffer, CmdBindIndexData );
	BufferU64( buffer, mem );
	BufferU64( buffer, offset );
	BufferU32( buffer, indexType );
}


void vkCmdDrawIndexed(VK_CMD_BUFFER cmdBuffer, VK_UINT firstIndex, VK_UINT indexCount, VK_INT vertexOffset, VK_UINT firstInstance, VK_UINT instanceCount)
{
	printf("%s()\n", __PRETTY_FUNCTION__);
	vk_CmdBuffer* buffer = (vk_CmdBuffer*)cmdBuffer;

	BufferU32( buffer, CmdDrawIndexed );
	BufferU32( buffer, firstIndex );
	BufferU32( buffer, indexCount );
	BufferU32( buffer, vertexOffset );
	BufferU32( buffer, firstInstance );
	BufferU32( buffer, instanceCount );
}


void vkCmdDraw(VK_CMD_BUFFER cmdBuffer, VK_UINT firstVertex, VK_UINT vertexCount, VK_UINT firstInstance, VK_UINT instanceCount)
{
	printf("%s()\n", __PRETTY_FUNCTION__);
	vk_CmdBuffer* buffer = (vk_CmdBuffer*)cmdBuffer;

	BufferU32( buffer, CmdDraw );
	BufferU32( buffer, firstVertex );
	BufferU32( buffer, vertexCount );
	BufferU32( buffer, firstInstance );
	BufferU32( buffer, instanceCount );
}


void vkCmdClearColorImage(VK_CMD_BUFFER cmdBuffer, VK_IMAGE image, const VK_FLOAT color[4], VK_UINT rangeCount, const VK_IMAGE_SUBRESOURCE_RANGE* pRanges)
{
	printf("%s()\n", __PRETTY_FUNCTION__);

	vk_CmdBuffer* buffer = (vk_CmdBuffer*)cmdBuffer;
	BufferU32( buffer, CmdClearColorImage );
	BufferU64( buffer, image );
	BufferU32( buffer, *(uint32_t*)&color[0] );
	BufferU32( buffer, *(uint32_t*)&color[1] );
	BufferU32( buffer, *(uint32_t*)&color[2] );
	BufferU32( buffer, *(uint32_t*)&color[3] );
	BufferU32( buffer, rangeCount );
	BufferU64( buffer, (uint64_t)pRanges );
}
