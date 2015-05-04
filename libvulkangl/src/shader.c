#include <malloc.h>
#include <string.h>
#include "internal.h"
/*
char* strdup(char*);

static int jump_word(const char* line, int i, const char* word, int jump_trailing)
{
	if ( word && word[0] ) {
		char* pos = strstr( line + i, word );
		if ( !pos ) {
			return -1;
		}
		return pos + strlen( word ) - line;
	}

	while ( line[i] && ( line[i] == ' ' || line[i] == '\t' ) ) {
		i++;
	}

	while ( line[i] && !( line[i] == ' ' || line[i] == '\t' ) ) {
		i++;
	}

	while ( jump_trailing && line[i] && ( line[i] == ' ' || line[i] == '\t' ) ) {
		i++;
	}

	return i;
}
*/
VK_RESULT vkCreateShader(VK_DEVICE device, const VK_SHADER_CREATE_INFO* pCreateInfo, VK_SHADER* pShader)
{
	printf("%s()\n", __PRETTY_FUNCTION__);

	vk_Shader* shader = (vk_Shader*)devAlloc->pfnAlloc( sizeof(vk_Shader), 16, 0 );
	memset( shader, 0, sizeof( *shader ) );
	shader->device = (vk_Device*)device;
	*pShader = (uint64_t)shader;

// 	shader->data = strdup( (char*)pCreateInfo->pCode );
// 	shader->data = (char*)pCreateInfo->pCode;
	shader->data = malloc( pCreateInfo->codeSize + 1 );
	memcpy( shader->data, pCreateInfo->pCode, pCreateInfo->codeSize );
	shader->data[pCreateInfo->codeSize] = 0;
	shader->len = pCreateInfo->codeSize;
	shader->glShader = 0;
/*
	char line[2048] = "";
	char attrib[512] = "";
	int i, j, k;
	for ( i = 0, j = 0; i < (int)shader->len; i++ ) {
		k = 0;
		strncpy( line, ((char*)pCreateInfo->pCode) + i, 1024 );
		if ( strchr( line, '\n' ) ) {
			strchr( line, '\n' )[1] = 0;
		}
		if ( strstr( line, "attribute" ) && strchr( line, ':' ) && strstr( line, "register" ) ) {
			k = jump_word( line, k, "attribute", 1 ); if(k<0)return VK_ERROR_BAD_SHADER_CODE;
			k = jump_word( line, k, "", 1 ); if(k<0)return VK_ERROR_BAD_SHADER_CODE;
			int len = jump_word( line, k, "", 0 ) - k; if(len<0)return VK_ERROR_BAD_SHADER_CODE;
			strncpy( attrib, line + k, len );
			attrib[len] = 0;
			printf("attrib = '%s'\n", attrib);
			k = jump_word( line, k, "register", 1 ); if(k<0)return VK_ERROR_BAD_SHADER_CODE;
			k = jump_word( line, k, "(", 1 ); if(k<0)return VK_ERROR_BAD_SHADER_CODE;
			k = jump_word( line, k, "t", 1 ); if(k<0)return VK_ERROR_BAD_SHADER_CODE;
			int loc = atoi( line + k );
			printf("loc = %d\n", loc);
		}
	}
*/
	return VK_SUCCESS;
}


VK_RESULT vkCreateGraphicsPipeline(VK_DEVICE device, const VK_GRAPHICS_PIPELINE_CREATE_INFO* pCreateInfo, VK_PIPELINE* pPipeline)
{
	uint32_t i;
	printf("%s()\n", __PRETTY_FUNCTION__);

	vk_Pipeline* pipeline = (vk_Pipeline*)devAlloc->pfnAlloc( sizeof(vk_Pipeline), 16, 0 );
	memset( pipeline, 0, sizeof( *pipeline ) );
	pipeline->device = (vk_Device*)device;
	*pPipeline = (uint64_t)pipeline;

	pipeline->vertexShader = (vk_Shader*)pCreateInfo->vs.shader;
	pipeline->fragmentShader = (vk_Shader*)pCreateInfo->ps.shader;

	if ( pipeline->vertexShader->glShader == 0 ) {
		pipeline->vertexShader->glShader = glCreateShader(GL_VERTEX_SHADER);
		glShaderSource( pipeline->vertexShader->glShader, 1, (const char**)&pipeline->vertexShader->data, NULL );
		glCompileShader( pipeline->vertexShader->glShader );
		char log[4096] = "";
		int logsize = 4096;
		glGetShaderInfoLog( pipeline->vertexShader->glShader, logsize, &logsize, log );
		printf("vertex compile : \n%s\n", log);
	}
	if ( pipeline->fragmentShader->glShader == 0 ) {
		pipeline->fragmentShader->glShader = glCreateShader(GL_FRAGMENT_SHADER);
		glShaderSource( pipeline->fragmentShader->glShader, 1, (const char**)&pipeline->fragmentShader->data, NULL );
		glCompileShader( pipeline->fragmentShader->glShader );
		char log[4096] = "";
		int logsize = 4096;
		glGetShaderInfoLog( pipeline->fragmentShader->glShader, logsize, &logsize, log );
		printf("fragment compile : \n%s\n", log);
	}

	pipeline->glProgram = glCreateProgram();
	printf("err = %X\n", glGetError());
	glAttachShader( pipeline->glProgram, pipeline->vertexShader->glShader );
	printf("err = %X\n", glGetError());
	glAttachShader( pipeline->glProgram, pipeline->fragmentShader->glShader );
	printf("err = %X\n", glGetError());
/*
	for ( j = 0; j < 2; j++ ) {
		for ( i = 0; i < pCreateInfo->vs.descriptorSetMapping[j].descriptorCount; i++ ) {
			// TODO : Implement recursive set
			VK_DESCRIPTOR_SLOT_INFO* slot = (VK_DESCRIPTOR_SLOT_INFO*)&pCreateInfo->vs.descriptorSetMapping[j].pDescriptorInfo[i];
			if ( slot->slotObjectType == VK_SLOT_SHADER_RESOURCE ) {
			}
		}
	}
*/
	glLinkProgram( pipeline->glProgram );
	printf("err = %X\n", glGetError());
	glUseProgram( pipeline->glProgram );
	printf("err = %X\n", glGetError());


	pipeline->nConstBuffers = pCreateInfo->vs.linkConstBufferCount;
	pipeline->constBuffers = (vk_ConstBuffer*)devAlloc->pfnAlloc( sizeof(vk_ConstBuffer) * pipeline->nConstBuffers, 16, 0 );
	for ( i = 0; i < pipeline->nConstBuffers; i++ ) {
		pipeline->constBuffers[i].id = pCreateInfo->vs.pLinkConstBufferInfo[i].bufferId;
		pipeline->constBuffers[i].size = pCreateInfo->vs.pLinkConstBufferInfo[i].bufferSize;
		pipeline->constBuffers[i].data = (void*)pCreateInfo->vs.pLinkConstBufferInfo[i].pBufferData;

		printf("err = %X\n", glGetError());
// 		printf("loc = %d\n", glGetUniformBlockIndex(pipeline->glProgram, "ge_Matrices_0"));
// 		printf("loc = %d\n", glGetUniformBlockIndex(pipeline->glProgram, "ge_Matrices_1"));
		glUniformBlockBinding( pipeline->glProgram, pipeline->constBuffers[i].id - 10, pipeline->constBuffers[i].id - 10 );
		glGenBuffers( 1, &pipeline->constBuffers[i].glUbo );
		glBindBuffer( GL_UNIFORM_BUFFER, pipeline->constBuffers[i].glUbo );
		glBufferData( GL_UNIFORM_BUFFER, pipeline->constBuffers[i].size, pipeline->constBuffers[i].data, GL_DYNAMIC_COPY );
		printf("err = %X\n", glGetError());
		glBindBufferBase( GL_UNIFORM_BUFFER, pipeline->constBuffers[i].id, pipeline->constBuffers[i].glUbo );
		printf("err = %X\n", glGetError());
		glBindBuffer( GL_UNIFORM_BUFFER, 0 );

	}

	glUseProgram( 0 );

	return VK_SUCCESS;
}


VK_RESULT vkCreateComputePipeline(VK_DEVICE device, const VK_COMPUTE_PIPELINE_CREATE_INFO* pCreateInfo, VK_PIPELINE* pPipeline)
{
	printf("%s()\n", __PRETTY_FUNCTION__);
	return VK_SUCCESS;
}

VK_RESULT vkCreateDescriptorSet(VK_DEVICE device, const VK_DESCRIPTOR_SET_CREATE_INFO* pCreateInfo, VK_DESCRIPTOR_SET* pDescriptorSet)
{
	printf("%s()\n", __PRETTY_FUNCTION__);

	vk_DescriptorSet* set = (vk_DescriptorSet*)devAlloc->pfnAlloc( sizeof(vk_DescriptorSet), 16, 0 );
	memset( set, 0, sizeof( *set ) );
	set->device = (vk_Device*)device;
	*pDescriptorSet = (uint64_t)set;

	glGenVertexArrays( 1, &set->glId );
	set->nAttribs = pCreateInfo->slots;

	return VK_SUCCESS;
}

 
void vkClearDescriptorSetSlots(VK_DESCRIPTOR_SET descriptorSet, VK_UINT startSlot, VK_UINT slotCount)
{
	printf("%s()\n", __PRETTY_FUNCTION__);
}


void vkAttachMemoryViewDescriptors(VK_DESCRIPTOR_SET descriptorSet, VK_UINT startSlot, VK_UINT slotCount, const VK_MEMORY_VIEW_ATTACH_INFO* pMemViews)
{
	printf("%s()\n", __PRETTY_FUNCTION__);
	VK_UINT i;
	vk_DescriptorSet* set = (vk_DescriptorSet*)descriptorSet;

//	glBindVertexArray( set->glId );

	for ( i = 0; i < slotCount; i++ ) {
		VK_UINT32 fmt = pMemViews[i].format.channelFormat;
		GLenum type = 0;
		GLint size = 0;
		if ( pMemViews[i].format.numericFormat == VK_NUM_FMT_FLOAT ) {
			type = GL_FLOAT;
		} else if ( pMemViews[i].format.numericFormat == VK_NUM_FMT_SRGB ) {
			type = GL_UNSIGNED_BYTE;
		}
		if ( fmt == VK_CH_FMT_R32G32B32A32 || fmt == VK_CH_FMT_R8G8B8A8 ) {
			size = 4;
		}else if ( fmt == VK_CH_FMT_R32G32B32 ) {
			size = 3;
		}
		set->attribs[startSlot + i].index = startSlot + i;
		set->attribs[startSlot + i].size = size;
		set->attribs[startSlot + i].type = type;
		set->attribs[startSlot + i].stride = pMemViews[i].stride;
		set->attribs[startSlot + i].offset = (void*)pMemViews[i].offset;
		set->attribs[startSlot + i].memory = (vk_GpuMemory*)pMemViews[i].mem;
//		glEnableVertexAttribArray( startSlot + i );
// 		glVertexAttribPointer( startSlot + i, size, type, GL_FALSE, pMemViews[i].stride, (void*)pMemViews[i].offset );
	}

//	glBindVertexArray( 0 );
}