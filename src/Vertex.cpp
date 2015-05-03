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

#include "Vertex.h"

namespace GE {

Vertex::Vertex()
	: u( 0.0f ), v( 0.0f ), w( 0.0f ), _align1( 0.0f )
	, color( { 0.0f } )
	, nx( 0.0f ), ny( 0.0f ), nz( 0.0f ), _align2( 0.0f )
	, x( 0.0f ), y( 0.0f ), z( 0.0f ), weight( 0.0f )
{
}


void Vertex::UpdateDescriptorSet( VK_DESCRIPTOR_SET descriptorSet, VK_MEMORY_VIEW_ATTACH_INFO* memoryViewAttachInfo )
{
	memoryViewAttachInfo->stride = sizeof( Vertex );

	// U, V, W, align1
	memoryViewAttachInfo->offset = 0;
	memoryViewAttachInfo->format.channelFormat = VK_CH_FMT_R32G32B32A32;
	memoryViewAttachInfo->format.numericFormat = VK_NUM_FMT_FLOAT;
	vkAttachMemoryViewDescriptors( descriptorSet, 0, 1, memoryViewAttachInfo );

	// COLOR
	memoryViewAttachInfo->offset = sizeof( float ) * 4;
// 		memoryViewAttachInfo->format.channelFormat = VK_CH_FMT_R8G8B8A8;
// 		memoryViewAttachInfo->format.numericFormat = VK_NUM_FMT_SRGB;
	memoryViewAttachInfo->format.channelFormat = VK_CH_FMT_R32G32B32A32;
	memoryViewAttachInfo->format.numericFormat = VK_NUM_FMT_FLOAT;
	vkAttachMemoryViewDescriptors( descriptorSet, 1, 1, memoryViewAttachInfo );

	// NX, NY, NZ, align2
// 		memoryViewAttachInfo->offset = sizeof( float ) * 4 + sizeof( uint32_t );
	memoryViewAttachInfo->offset = sizeof( float ) * 4 + sizeof( float ) * 4;
	memoryViewAttachInfo->format.channelFormat = VK_CH_FMT_R32G32B32A32;
	memoryViewAttachInfo->format.numericFormat = VK_NUM_FMT_FLOAT;
	vkAttachMemoryViewDescriptors( descriptorSet, 2, 1, memoryViewAttachInfo );

	// X, Y, Z, weight
// 		memoryViewAttachInfo->offset = sizeof( float ) * 4 + sizeof( uint32_t ) + sizeof( float ) * 4;
	memoryViewAttachInfo->offset = sizeof( float ) * 4 + sizeof( float ) * 4 + sizeof( float ) * 4;
	memoryViewAttachInfo->format.channelFormat = VK_CH_FMT_R32G32B32A32;
	memoryViewAttachInfo->format.numericFormat = VK_NUM_FMT_FLOAT;
	vkAttachMemoryViewDescriptors( descriptorSet, 3, 1, memoryViewAttachInfo );
}


} // namespace GE
