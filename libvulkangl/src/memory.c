#include <limits.h>
#include "internal.h"

/* Virtual HEAPS :
 *  0 = non-mappable (driver-allocated GL objects)
 *  1 = mappable (glBuffer*)
 *  2 = mappable (glFrameBuffer/Texture)
 */


VK_RESULT vkGetObjectInfo(VK_BASE_OBJECT object, VK_ENUM infoType, VK_SIZE* pDataSize, VK_VOID* pData)
{
	printf("%s()\n", __PRETTY_FUNCTION__);

	if ( infoType == VK_INFO_TYPE_MEMORY_REQUIREMENTS ) {
		VK_MEMORY_REQUIREMENTS* memReqs = (VK_MEMORY_REQUIREMENTS*)pData;
		memReqs->alignment = 16;
		memReqs->heapCount = 2;
		memReqs->size = 0; // driver-allocated GL object
		memReqs->heaps[0] = 0;
		memReqs->heaps[1] = 1;
	}

	return VK_SUCCESS;
}


VK_RESULT vkGetMemoryHeapCount(VK_DEVICE device, VK_UINT* pCount)
{
	printf("%s()\n", __PRETTY_FUNCTION__);

	*pCount = 3;

	return VK_SUCCESS;
}


VK_RESULT vkGetMemoryHeapInfo(VK_DEVICE device, VK_UINT heapId, VK_ENUM infoType, VK_SIZE* pDataSize, VK_VOID* pData)
{
	printf("%s()\n", __PRETTY_FUNCTION__);

	if ( infoType == VK_INFO_TYPE_MEMORY_HEAP_PROPERTIES ) {
		VK_MEMORY_HEAP_PROPERTIES* heapProps = (VK_MEMORY_HEAP_PROPERTIES*)pData;
		heapProps->heapSize = UINT_MAX;
		heapProps->pageSize = 512;
		if ( heapId == 1 ) {
			heapProps->flags = VK_MEMORY_HEAP_CPU_VISIBLE;
		}
	}

	return VK_SUCCESS;
}


VK_RESULT vkAllocMemory(VK_DEVICE device, const VK_MEMORY_ALLOC_INFO* pAllocInfo, VK_GPU_MEMORY* pMem)
{
	printf("%s()\n", __PRETTY_FUNCTION__);

	vk_GpuMemory* memory = (vk_GpuMemory*)devAlloc->pfnAlloc( sizeof(vk_GpuMemory), 16, 0 );
	memset( memory, 0, sizeof( *memory ) );
	memory->device = (vk_Device*)device;
	*pMem = (uint64_t)memory;

	memory->heap = pAllocInfo->heaps[0];
	memory->size = pAllocInfo->size;

	if ( pAllocInfo->heaps[0] == 0 ) {
		memory->type = 0;
	} else if ( pAllocInfo->heaps[0] == 1 ) {
		memory->type = GL_COPY_READ_BUFFER;
// 		memory->type = GL_ARRAY_BUFFER;
		glGenBuffers( 1, &memory->glId );
		glBindBuffer( memory->type, memory->glId );
		glBufferData( memory->type, pAllocInfo->size, NULL, GL_STATIC_DRAW );
		glBindBuffer( memory->type, 0 );
	}

	return VK_SUCCESS;
}


VK_RESULT vkBindObjectMemory(VK_OBJECT object, VK_GPU_MEMORY mem, VK_GPU_SIZE offset)
{
	printf("%s()\n", __PRETTY_FUNCTION__);

	vk_GpuMemory* memory = (vk_GpuMemory*)mem;
	if ( memory->heap == 0 ) {
		memory->ptr = (void*)object;
	}

	return VK_SUCCESS;
}


VK_RESULT vkMapMemory(VK_GPU_MEMORY mem, VK_FLAGS flags, VK_VOID** ppData)
{
// 	printf("%s()\n", __PRETTY_FUNCTION__);

	vk_GpuMemory* memory = (vk_GpuMemory*)mem;

	glBindBuffer( memory->type, memory->glId );
	*ppData = glMapBuffer( memory->type, GL_WRITE_ONLY );

	return VK_SUCCESS;
}


VK_RESULT vkUnmapMemory(VK_GPU_MEMORY mem)
{
// 	printf("%s()\n", __PRETTY_FUNCTION__);

	vk_GpuMemory* memory = (vk_GpuMemory*)mem;

	glUnmapBuffer( memory->type );
	glBindBuffer( memory->type, 0 );

	return VK_SUCCESS;
}
