/*
 * The GammaEngine Library 2.0 is a multiplatform Vulkan-based game engine
 * Copyright (C) 2015  Adrien Aubry <dridri85@gmail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include <fstream>
#include <vector>

#include "Instance.h"
#include "Renderer.h"
#include "gememory.h"

Renderer::Renderer( Instance* instance, int devid )
	: mReady( false )
	, mInstance( instance )
	, mDevId( devid )
	, mPipeline( 0 )
{
	VK_CMD_BUFFER_CREATE_INFO info;

	info.queueType = 0;
	info.flags = 0;

	vkCreateCommandBuffer( instance->device( mDevId ), &info, &mCmdBuffer );
}


Renderer::~Renderer()
{
}


int Renderer::LoadVertexShader( const std::string file, int devid )
{
	VK_SHADER_CREATE_INFO vsInfo = { 0 };
	vsInfo.pCode = loadShader( file, &vsInfo.codeSize );

	vkCreateShader( mInstance->device( mDevId ), &vsInfo, &mVertexShader );

	mReady = false;
	return 0;
}


int Renderer::LoadFragmentShader( const std::string file, int devid )
{
	VK_SHADER_CREATE_INFO vsInfo = { 0 };
	vsInfo.pCode = loadShader( file, &vsInfo.codeSize );

	vkCreateShader( mInstance->device( mDevId ), &vsInfo, &mFragmentShader );

	mReady = false;
	return 0;
}


void Renderer::createPipeline()
{
	if ( mPipeline ) {
		// TODO : Free existing pipeline
	}

	VK_DESCRIPTOR_SLOT_INFO vsDescriptorSlots[2];
	VK_DESCRIPTOR_SLOT_INFO psDescriptorSlots[2];
	VK_GRAPHICS_PIPELINE_CREATE_INFO pipelineCreateInfo = {};

	vsDescriptorSlots[0].slotObjectType = VK_SLOT_SHADER_RESOURCE;
	vsDescriptorSlots[0].shaderEntityIndex = 0;
	vsDescriptorSlots[1].slotObjectType = VK_SLOT_SHADER_RESOURCE;
	vsDescriptorSlots[1].shaderEntityIndex = 1;

	psDescriptorSlots[0].slotObjectType = VK_SLOT_UNUSED;
	psDescriptorSlots[1].slotObjectType = VK_SLOT_UNUSED;

	pipelineCreateInfo.vs.shader = mVertexShader;
	pipelineCreateInfo.vs.dynamicMemoryViewMapping.slotObjectType = VK_SLOT_UNUSED;
	pipelineCreateInfo.vs.descriptorSetMapping[0].descriptorCount = 2;
	pipelineCreateInfo.vs.descriptorSetMapping[0].pDescriptorInfo = vsDescriptorSlots;

	pipelineCreateInfo.ps.shader = mFragmentShader;
	pipelineCreateInfo.ps.dynamicMemoryViewMapping.slotObjectType = VK_SLOT_UNUSED;
	pipelineCreateInfo.ps.descriptorSetMapping[0].descriptorCount = 2;
	pipelineCreateInfo.ps.descriptorSetMapping[0].pDescriptorInfo = psDescriptorSlots;

	pipelineCreateInfo.iaState.topology = VK_TOPOLOGY_TRIANGLE_LIST;
	pipelineCreateInfo.iaState.disableVertexReuse = VK_FALSE;
	pipelineCreateInfo.rsState.depthClipEnable = VK_FALSE;

	pipelineCreateInfo.cbState.logicOp = VK_LOGIC_OP_COPY;
	pipelineCreateInfo.cbState.target[0].blendEnable = VK_TRUE;
	pipelineCreateInfo.cbState.target[0].channelWriteMask = 0xF; // RGBA bits
	pipelineCreateInfo.cbState.target[0].format.channelFormat = VK_CH_FMT_R8G8B8A8;
	pipelineCreateInfo.cbState.target[0].format.numericFormat = VK_NUM_FMT_UNORM;
	pipelineCreateInfo.dbState.format.channelFormat = VK_CH_FMT_R4G4B4A4;
	pipelineCreateInfo.dbState.format.numericFormat = VK_NUM_FMT_UNDEFINED;

	vkCreateGraphicsPipeline( mInstance->device( mDevId ), &pipelineCreateInfo, &mPipeline );
	mPipelineRef = mInstance->AllocateObject( mDevId, mPipeline );

	mReady = true;
}


void Renderer::Compute()
{
	if ( !mReady ) {
		createPipeline();
	}
}


void Renderer::Draw()
{
	if ( !mReady ) {
		return;
	}

	mInstance->QueueSubmit( mDevId, mCmdBuffer, 0, 0 );
}


uint8_t* Renderer::loadShader( const std::string filename, size_t* sz )
{
	std::ifstream file( filename, std::ios::binary | std::ios::ate );

	size_t size = file.tellg();
	file.seekg( 0, std::ios::beg );

	uint8_t* data = (uint8_t*)geMalloc( size );

	file.read( (char*)data, size );
	file.close();

	*sz = size;
	return data;
}
