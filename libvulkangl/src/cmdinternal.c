#include "internal.h"


static GLuint indicesType = GL_UNSIGNED_INT;
static GLuint indicesOffset = 0;
static vk_DescriptorSet* boundDescriptorSet = NULL;
static vk_Pipeline* boundPipeline = NULL;


static uint32_t BufferU32(vk_CmdReadBuf* buffer)
{
	uint32_t ret = 0;
	if ( buffer->i + 1 < buffer->len ) {
		ret = buffer->buf[buffer->i];
		buffer->i++;
	}
	return ret;
}


static uint64_t BufferU64(vk_CmdReadBuf* buffer)
{
	uint32_t ret = 0;
	if ( buffer->i + 1 < buffer->len ) {
		ret = ( (uint64_t*)&buffer->buf[buffer->i] )[0];
		buffer->i += 2;
	}
	return ret;
}


void fBindTarget(vk_CmdReadBuf* buf)
{
// 	printf("%s()\n", __PRETTY_FUNCTION__);

	vk_ColorTargetView* targetView = (vk_ColorTargetView*)BufferU64( buf );
	uint64_t depthTarget = BufferU64( buf ); (void)depthTarget;

	vk_Image* colorImage = targetView->image;

// 	printf("  bind id = %d\n", colorImage->glId);

	glBindFramebuffer( GL_FRAMEBUFFER, colorImage->glId );

// 	printf("  bind err = %x\n", glGetError());
}

static void print_matrix(float* m){
	printf("data = %f %f %f %f\n", m[0], m[1], m[2], m[3]);
	printf("     = %f %f %f %f\n", m[4], m[5], m[6], m[7]);
	printf("     = %f %f %f %f\n", m[8], m[9], m[10], m[11]);
	printf("     = %f %f %f %f\n", m[12], m[13], m[14], m[15]);
}

void fCmdBindPipeline(vk_CmdReadBuf* buf)
{
// 	printf("%s()\n", __PRETTY_FUNCTION__);

	uint32_t i;
	uint32_t bindPoint = BufferU32( buf );
	vk_Pipeline* pipeline = (vk_Pipeline*)BufferU64( buf );

	if ( bindPoint == VK_PIPELINE_BIND_POINT_GRAPHICS ) {
// 		printf("  bind id = %d\n", pipeline->glProgram);
		boundPipeline = pipeline;
		glUseProgram( pipeline->glProgram );
// 		printf("  bind err = %x\n", glGetError());

		for ( i = 0; i < pipeline->nConstBuffers; i++ ) {
// 			print_matrix((float*)pipeline->constBuffers[i].data);
			glBindBuffer( GL_UNIFORM_BUFFER, pipeline->constBuffers[i].glUbo );
			glBindBufferBase( GL_UNIFORM_BUFFER, pipeline->constBuffers[i].id - 10, pipeline->constBuffers[i].glUbo );
// 			glBindBufferRange( GL_UNIFORM_BUFFER, pipeline->constBuffers[i].id, pipeline->constBuffers[i].glUbo, 0, pipeline->constBuffers[i].size );
			glBufferSubData( GL_UNIFORM_BUFFER, 0, pipeline->constBuffers[i].size, pipeline->constBuffers[i].data );
			glUniformBlockBinding( pipeline->glProgram, pipeline->constBuffers[i].id - 10, pipeline->constBuffers[i].id - 10 );
		}
		glBindBuffer( GL_UNIFORM_BUFFER, 0 );

// 		print_matrix( pipeline->constBuffers[0].data );
// 		print_matrix( pipeline->constBuffers[1].data );
// 		glUniformMatrix4fv(glGetUniformLocation( pipeline->glProgram, "mat_0" ), 1, GL_FALSE, pipeline->constBuffers[0].data );
// 		glUniformMatrix4fv(glGetUniformLocation( pipeline->glProgram, "mat_1" ), 1, GL_FALSE, pipeline->constBuffers[1].data );
	}
}


void fCmdBindDescriptorSet(vk_CmdReadBuf* buf)
{
	uint32_t i;
// 	printf("%s()\n", __PRETTY_FUNCTION__);

	uint32_t bindPoint = BufferU32( buf );
	uint32_t index = BufferU32( buf );
	vk_DescriptorSet* descriptorSet = (vk_DescriptorSet*)BufferU64( buf );
	uint32_t slotOffset = BufferU32( buf );

	if ( bindPoint == VK_PIPELINE_BIND_POINT_GRAPHICS ) {
// 		printf("  bind id = %d\n", descriptorSet->glId);
		glBindVertexArray( descriptorSet->glId );
// 		printf("  bind err = %x\n", glGetError());
		boundDescriptorSet = descriptorSet;

		for ( i = 0; i < sizeof(boundDescriptorSet->attribs)/sizeof(vk_DescriptorSetAttrib); i++ ) {
			if ( boundDescriptorSet->attribs[i].stride == 0 && boundDescriptorSet->attribs[i].memory ) {
// 				glBindBuffer( boundDescriptorSet->attribs[i].memory->type, boundDescriptorSet->attribs[i].memory->glId );
// 				printf("  loc = %d\n", glGetProgramResourceIndex( boundPipeline->glProgram, GL_SHADER_STORAGE_BLOCK, "ge_ObjectMatrix" ));
// 				printf("  loc = %d\n", glGetProgramResourceIndex( boundPipeline->glProgram, GL_SHADER_STORAGE_BLOCK, "ge_Matrices_2" ));
// 				glShaderStorageBlockBinding( boundPipeline->glProgram, 0, 0 );
// 				glBindBufferBase( GL_SHADER_STORAGE_BUFFER, 0, boundDescriptorSet->attribs[i].memory->glId );
				glBindBuffer( GL_UNIFORM_BUFFER, boundDescriptorSet->attribs[i].memory->glId );
				glBindBufferBase( GL_UNIFORM_BUFFER, boundDescriptorSet->attribs[i].index - 10, boundDescriptorSet->attribs[i].memory->glId );
				glUniformBlockBinding( boundPipeline->glProgram, boundDescriptorSet->attribs[i].index - 10, boundDescriptorSet->attribs[i].index - 10 );
				glBindBuffer( GL_UNIFORM_BUFFER, 0 );
			}
		}

	}
}


void fCmdBindVertexBuffer(vk_CmdReadBuf* buf)
{
// 	printf("%s()\n", __PRETTY_FUNCTION__);

	uint32_t i;
	vk_GpuMemory* memory = (vk_GpuMemory*)BufferU64(buf);
	uint64_t offset = BufferU64(buf);
	uint64_t unknown = BufferU64(buf);

	if ( memory->type == GL_COPY_READ_BUFFER ) {
		memory->type = GL_ARRAY_BUFFER;
// 		memory->type = GL_SHADER_STORAGE_BUFFER;
	}

	glBindBuffer( memory->type, memory->glId );
// 	printf("  bind err = %x\n", glGetError());

	if ( boundDescriptorSet ) {
		for ( i = 0; i < boundDescriptorSet->nAttribs; i++ ) {
			if ( memory == boundDescriptorSet->attribs[i].memory ) {
// 				glBindBufferBase(GL_SHADER_STORAGE_BUFFER, boundDescriptorSet->attribs[i].index, memory->glId );
	// 			glBindBufferBase(GL_SHADER_STORAGE_BUFFER, boundDescriptorSet->attribs[i].index, boundDescriptorSet->attribs[i].index );
	// 			printf("  enable %d\n", boundDescriptorSet->attribs[i].index);
				glEnableVertexAttribArray( boundDescriptorSet->attribs[i].index );
				glVertexAttribPointer( boundDescriptorSet->attribs[i].index, boundDescriptorSet->attribs[i].size, boundDescriptorSet->attribs[i].type, GL_FALSE, boundDescriptorSet->attribs[i].stride, boundDescriptorSet->attribs[i].offset );
			}
		}
	}
}


void fCmdBindIndexData(vk_CmdReadBuf* buf)
{
// 	printf("%s()\n", __PRETTY_FUNCTION__);

	vk_GpuMemory* memory = (vk_GpuMemory*)BufferU64(buf);
	uint64_t offset = BufferU64(buf);
	uint32_t type = BufferU32(buf);

	if ( type == VK_INDEX_32 ) {
		indicesType = GL_UNSIGNED_INT;
	}else if ( type == VK_INDEX_16 ) {
		indicesType = GL_UNSIGNED_SHORT;
	}

	indicesOffset = offset;
	if ( memory->type == GL_COPY_READ_BUFFER ) {
		memory->type = GL_ELEMENT_ARRAY_BUFFER;
	}
/*
	if ( memory->type == GL_COPY_READ_BUFFER ) {
		memory->type = GL_ELEMENT_ARRAY_BUFFER;
		GLuint realbuf = 0;
		glGenBuffers( 1, &realbuf );
		glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, realbuf );
		glBufferData( GL_ELEMENT_ARRAY_BUFFER, memory->size, NULL, GL_STATIC_DRAW );
		glBindBuffer( GL_COPY_READ_BUFFER, memory->glId );
		glCopyBufferSubData( GL_COPY_READ_BUFFER, GL_ELEMENT_ARRAY_BUFFER, 0, 0, memory->size );
		glBindBuffer( GL_COPY_READ_BUFFER, 0 );
		glDeleteBuffers( 1, &memory->glId );
		memory->glId = realbuf;
	}
*/
	glBindBuffer( memory->type, memory->glId );
}

void fCmdDrawIndexed(vk_CmdReadBuf* buf)
{
// 	printf("%s()\n", __PRETTY_FUNCTION__);

	uint32_t firstIndex = BufferU32( buf );
	uint32_t indexCount = BufferU32( buf );
	uint32_t vertexOffset = BufferU32( buf );
	uint32_t firstInstance = BufferU32( buf );
	uint32_t instanceCount = BufferU32( buf );

	// TODO : draw instanced
	(void)firstInstance;
	(void)instanceCount;

	size_t offset = ( indicesType == GL_UNSIGNED_INT ? 4 : 2 ) * firstIndex + indicesOffset;

	glDrawElements( boundPipeline->renderMode, indexCount, indicesType, (void*)offset );
}

void fCmdDraw(vk_CmdReadBuf* buf)
{
// 	printf("%s()\n", __PRETTY_FUNCTION__);

	uint32_t firstVertex = BufferU32( buf );
	uint32_t vertexCount = BufferU32( buf );
	uint32_t firstInstance = BufferU32( buf );
	uint32_t instanceCount = BufferU32( buf );
// 	printf("  vertexCount = %d\n", vertexCount);

	// TODO : draw instanced
	(void)firstInstance;
	(void)instanceCount;

	glDrawArrays( boundPipeline->renderMode, firstVertex, vertexCount );
}

void fCmdClearColorImage(vk_CmdReadBuf* buf)
{
// 	printf("%s()\n", __PRETTY_FUNCTION__);

	uint32_t f32;

	vk_Image* image = (vk_Image*)BufferU64( buf );
	f32 = BufferU32( buf ); float r = *(float*)&f32;
	f32 = BufferU32( buf ); float g = *(float*)&f32;
	f32 = BufferU32( buf ); float b = *(float*)&f32;
	f32 = BufferU32( buf ); float a = *(float*)&f32;
	uint32_t rangeCount = BufferU32( buf );
	VK_IMAGE_SUBRESOURCE_RANGE* pRanges = (VK_IMAGE_SUBRESOURCE_RANGE*)BufferU64( buf );

	glBindFramebuffer( GL_FRAMEBUFFER, image->glId );
	glClearColor( r, g, b, a );
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
	glBindFramebuffer( GL_FRAMEBUFFER, 0 );
}
