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
#include "Object.h"
#include "Debug.h"
#include "File.h"
#include "gememory.h"

namespace GE {

Renderer::Renderer( Instance* instance, int devid )
	: mReady( false )
	, mInstance( instance ? instance : Instance::baseInstance() )
	, mDevId( devid )
	, mPipeline( 0 )
{
	VK_CMD_BUFFER_CREATE_INFO info;

	info.queueType = 0;
	info.flags = VK_CMD_BUFFER_OPTIMIZE_DESCRIPTOR_SET_SWITCH | VK_CMD_BUFFER_OPTIMIZE_GPU_SMALL_BATCH;

	vkCreateCommandBuffer( mInstance->device( mDevId ), &info, &mCmdBuffer );
}


Renderer::~Renderer()
{
}


int Renderer::LoadVertexShader( const std::string& file )
{
	VK_SHADER_CREATE_INFO vsInfo = { 0 };
	vsInfo.pCode = loadShader( file, &vsInfo.codeSize );

	vkCreateShader( mInstance->device( mDevId ), &vsInfo, &mVertexShader );

	mReady = false;
	return 0;
}


int Renderer::LoadFragmentShader( const std::string& file )
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


void Renderer::AddObject( Object* obj )
{
	mObjects.emplace_back( obj );
}


void Renderer::Compute()
{
	if ( !mReady ) {
		createPipeline();
	}

	vkBeginCommandBuffer( mCmdBuffer, 0 );

	VK_COLOR_TARGET_BIND_INFO colorTargetBindInfo;
//	colorTargetBindInfo.view = colorTargetView; // TODO
	colorTargetBindInfo.colorTargetState = VK_MEMORY_STATE_TARGET_RENDER_ACCESS_OPTIMAL;
	vkCmdBindTargets( mCmdBuffer, 1, &colorTargetBindInfo, nullptr );
/*	TODO TODO TODO
	vkCmdBindStateObject( mCmdBuffer, VK_STATE_BIND_MSAA, msaaState );
	vkCmdBindStateObject( mCmdBuffer, VK_STATE_BIND_VIEWPORT, viewportState );
	vkCmdBindStateObject( mCmdBuffer, VK_STATE_BIND_COLOR_BLEND, colorBlendState );
	vkCmdBindStateObject( mCmdBuffer, VK_STATE_BIND_DEPTH_STENCIL, depthStencilState );
	vkCmdBindStateObject( mCmdBuffer, VK_STATE_BIND_RASTER, rasterState );
*/
	vkCmdBindPipeline( mCmdBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, mPipeline );

	for ( decltype(mObjects)::iterator it = mObjects.begin(); it != mObjects.end(); ++it ) {
		vkCmdBindDescriptorSet( mCmdBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, 0, (*it)->descriptorSet( mInstance, mDevId ), 0 );
		// TODO / TEST : vkCmdBindVertexBuffer
		vkCmdBindIndexData( mCmdBuffer, (*it)->indicesRef( mInstance, mDevId ).mem, 0, VK_INDEX_32 );
		vkCmdDrawIndexed( mCmdBuffer, 0, (*it)->indicesCount(), 0, 0, 1 );
	}

	vkEndCommandBuffer( mCmdBuffer );
}


void Renderer::Draw()
{
	if ( !mReady ) {
		return;
	}

	mInstance->QueueSubmit( mDevId, mCmdBuffer, 0, 0 );
}


uint8_t* Renderer::loadShader( const std::string& filename, size_t* sz )
{
	File* file = new File( filename, File::READ );

	size_t size = file->Seek( 0, File::END );
	file->Rewind();

	uint8_t* data = (uint8_t*)geMalloc( size );

	file->Read( data, size );
	delete file;

	*sz = size;
	return data;
}

} // namespace GE
