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

#include "Instance.h"
#include "Object.h"
#include "File.h"
#include "Debug.h"

#include <algorithm>

namespace GE {


std::vector< ObjectLoader* > Object::mObjectLoaders = std::vector< ObjectLoader* >();


Object::Object()
	: mVertices( nullptr )
	, mVerticesCount( 0 )
{
}


Object::Object( const std::string filename )
	: Object()
{
	ObjectLoader* loader = nullptr;
	File* file = new File( filename, File::READ );

	std::string extension = filename.substr( filename.rfind( "." ) + 1 );
	std::string first_line = file->ReadLine();
	std::transform( first_line.begin(), first_line.end(), first_line.begin(), ::tolower );
	file->Rewind();
	uint32_t file_magic = 0;
	file->Read( &file_magic, sizeof(file_magic) );
	file->Rewind();

	for ( size_t i = 0; i < mObjectLoaders.size(); i++ ) {
		if ( mObjectLoaders.at(i)->fileType() == ObjectLoader::BINARY ) {
			if ( mObjectLoaders.at(i)->magic() == file_magic ) {
				loader = mObjectLoaders.at(i);
				break;
			}
		} else {
			std::vector< std::string > patterns = mObjectLoaders.at(i)->contentPatterns();
			for ( size_t j = 0; j < patterns.size(); j++ ) {
				std::string test_case = patterns[j];
				std::transform( test_case.begin(), test_case.end(), test_case.begin(), ::tolower );
				if ( first_line.find( test_case ) ) {
					loader = mObjectLoaders.at(i);
				}
			}
		}
	}

	if ( !loader ) {
		for ( size_t i = 0; i < mObjectLoaders.size(); i++ ) {
			std::vector< std::string > extensions = mObjectLoaders.at(i)->extensions();
			for ( size_t j = 0; j < extensions.size(); j++ ) {
				std::string test_case = extensions[j];
				std::transform( test_case.begin(), test_case.end(), test_case.begin(), ::tolower );
				if ( extension.find( test_case ) ) {
					loader = mObjectLoaders.at(i);
					break;
				}
			}
		}
	}

	if ( loader ) {
		loader = loader->NewInstance();
		loader->Load( file );
		*this = static_cast< Object >( *loader );
		delete loader;
	}

	delete file;
}


Object::~Object()
{
}


uint32_t Object::verticesCount()
{
	return mVerticesCount;
}


uint32_t Object::indicesCount()
{
	return mIndicesCount;
}


VK_DESCRIPTOR_SET Object::descriptorSet( Instance* instance, int devid )
{
	std::pair< Instance*, int > key( instance, devid );

	if ( mVkRefs.find( key ) == mVkRefs.end() ){
		AllocateGpu( instance, devid );
	}

	return std::get<0>( mVkRefs.at( key ) );
}

VK_MEMORY_REF Object::indicesRef( Instance* instance, int devid )
{
	std::pair< Instance*, int > key( instance, devid );

	if ( mVkRefs.find( key ) == mVkRefs.end() ) {
		AllocateGpu( instance, devid );
	}

	return std::get<3>( mVkRefs.at( key ) );
}


void Object::AllocateGpu( Instance* instance, int devid )
{
	std::pair< Instance*, int > key( instance, devid );

	VK_DESCRIPTOR_SET descriptorSet = {};
	VK_MEMORY_REF descriptorMemRef = {};
	VK_MEMORY_REF vertexDataMemRef = {};
	VK_CMD_BUFFER_CREATE_INFO bufferCreateInfo = { 0, 0 };
	void* bufferPointer;

	VK_DESCRIPTOR_SET_CREATE_INFO descriptorCreateInfo = {};
	descriptorCreateInfo.slots = 4;
	vkCreateDescriptorSet( instance->device( devid ), &descriptorCreateInfo, &descriptorSet );
	descriptorMemRef = instance->AllocateObject( devid, descriptorSet );

	vertexDataMemRef = instance->AllocateMappableBuffer( devid, sizeof( decltype(mVertices) ) * mVerticesCount );
	vkMapMemory( vertexDataMemRef.mem, 0, &bufferPointer );
	memcpy( bufferPointer, mVertices, sizeof( decltype(mVertices) ) * mVerticesCount );
	vkUnmapMemory( vertexDataMemRef.mem );

	VK_CMD_BUFFER initDataCmdBuffer;
	vkCreateCommandBuffer( instance->device( devid ), &bufferCreateInfo, &initDataCmdBuffer );
	vkBeginCommandBuffer( initDataCmdBuffer, 0 );
		VK_MEMORY_STATE_TRANSITION dataTransition = {};
		dataTransition.mem = vertexDataMemRef.mem;
		dataTransition.oldState = VK_MEMORY_STATE_DATA_TRANSFER;
		dataTransition.newState = VK_MEMORY_STATE_GRAPHICS_SHADER_READ_ONLY; // TEST VK_MEMORY_STATE_MULTI_SHADER_READ_ONLY
		dataTransition.offset = 0;
		dataTransition.regionSize = sizeof( decltype(mVertices) ) * mVerticesCount;
		vkCmdPrepareMemoryRegions( initDataCmdBuffer, 1, &dataTransition );
	vkEndCommandBuffer( initDataCmdBuffer );
	instance->QueueSubmit( devid, initDataCmdBuffer, &vertexDataMemRef, 1 );

	vkBeginDescriptorSetUpdate( descriptorSet );
		VK_MEMORY_VIEW_ATTACH_INFO memoryViewAttachInfo = {};
		memoryViewAttachInfo.state = VK_MEMORY_STATE_GRAPHICS_SHADER_READ_ONLY;
		memoryViewAttachInfo.mem = vertexDataMemRef.mem;
		memoryViewAttachInfo.stride = sizeof( decltype(mVertices) );
		memoryViewAttachInfo.range = sizeof( decltype(mVertices) ) * mVerticesCount;

		// U, V, W
		memoryViewAttachInfo.offset = 0;
		memoryViewAttachInfo.format.channelFormat = VK_CH_FMT_R32G32B32;
		memoryViewAttachInfo.format.numericFormat = VK_NUM_FMT_FLOAT;
		vkAttachMemoryViewDescriptors( descriptorSet, 0, 1, &memoryViewAttachInfo );

		// COLOR
		memoryViewAttachInfo.offset = sizeof( float ) * 3;
		memoryViewAttachInfo.format.channelFormat = VK_CH_FMT_R8G8B8A8;
		memoryViewAttachInfo.format.numericFormat = VK_NUM_FMT_SRGB;
		vkAttachMemoryViewDescriptors( descriptorSet, 1, 1, &memoryViewAttachInfo );

		// NX, NY, NZ
		memoryViewAttachInfo.offset = sizeof( float ) * 3 + sizeof( uint32_t );
		memoryViewAttachInfo.format.channelFormat = VK_CH_FMT_R32G32B32;
		memoryViewAttachInfo.format.numericFormat = VK_NUM_FMT_FLOAT;
		vkAttachMemoryViewDescriptors( descriptorSet, 2, 1, &memoryViewAttachInfo );

		// X, Y, Z
		memoryViewAttachInfo.offset = sizeof( float ) * 3 + sizeof( uint32_t ) + sizeof( float ) * 3;
		memoryViewAttachInfo.format.channelFormat = VK_CH_FMT_R32G32B32;
		memoryViewAttachInfo.format.numericFormat = VK_NUM_FMT_FLOAT;
		vkAttachMemoryViewDescriptors( descriptorSet, 3, 1, &memoryViewAttachInfo );
	vkEndDescriptorSetUpdate( descriptorSet );

	VK_MEMORY_REF indexMemRef = instance->AllocateMappableBuffer( devid, sizeof( uint32_t) * mIndicesCount );
	vkMapMemory( indexMemRef.mem, 0, &bufferPointer );
	memcpy( bufferPointer, mIndices, sizeof( decltype(mIndices) ) * mIndicesCount );
	vkUnmapMemory( indexMemRef.mem );

	std::tuple< VK_DESCRIPTOR_SET, VK_MEMORY_REF, VK_MEMORY_REF, VK_MEMORY_REF > data( descriptorSet, descriptorMemRef, vertexDataMemRef, indexMemRef );
	mVkRefs.insert( std::pair< decltype(key), decltype(data) > ( key, data ) );
}


ObjectLoader* Object::AddObjectLoader( ObjectLoader* loader )
{
	mObjectLoaders.insert( mObjectLoaders.begin(), loader );
	return loader;
}


} // namespace GE
