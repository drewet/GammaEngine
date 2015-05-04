#define USE_CMDS
#include <malloc.h>
#include "internal.h"

VK_RESULT vkQueueSubmit(VK_QUEUE _queue, VK_UINT cmdBufferCount, const VK_CMD_BUFFER* pCmdBuffers, VK_UINT memRefCount, const VK_MEMORY_REF* pMemRefs, VK_FENCE fence)
{
// 	printf("%s()\n", __PRETTY_FUNCTION__);

	vk_Queue* queue = (vk_Queue*)_queue;
	vk_QueueEntry* last = queue->entries;
	VK_UINT i;

	for ( i = 0; i < cmdBufferCount; i++ ) {
		pthread_mutex_lock( &queue->mutex );
		while ( last && last->next ) last = last->next;
		pthread_mutex_unlock( &queue->mutex );

		vk_QueueEntry* curr = (vk_QueueEntry*)malloc(sizeof(vk_QueueEntry)); // use real malloc to avoid flooding callback with a fake function
		curr->buffer = (vk_CmdBuffer*)pCmdBuffers[i];
		curr->next = NULL;

		pthread_mutex_lock( &queue->mutex );
		if ( last ) {
			last->next = curr;
		} else {
			queue->entries = curr;
		}
		pthread_mutex_unlock( &queue->mutex );
	}

	// TODO : play with fence

	_vkFlushQueue(queue); //TESTING

	return VK_SUCCESS;
}

void _vkFlushQueue(vk_Queue* queue)
{
	uint32_t i = 0;
	vk_QueueEntry* next = NULL;

	while ( queue->entries )
	{
		pthread_mutex_lock( &queue->mutex );
		vk_CmdReadBuf* buf = (vk_CmdReadBuf*)queue->entries->buffer;
		next = queue->entries->next;
		free( queue->entries );
		queue->entries = next;
		pthread_mutex_unlock( &queue->mutex );

		if ( buf->len > 0 ) {
			for ( buf->i = 0; buf->i < buf->len - 1; )
			{
				uint32_t cmd = buf->buf[buf->i++];
				for ( i = 0; i < sizeof(cmds) / sizeof(vk_Cmd); i++ ) {
					if ( cmds[i].id == cmd ) {
						cmds[i].f( buf );
	// 					printf("err = %X (%d)\n", glGetError(), cmd);
						break;
					}
				}
			}
		}
	}
}
