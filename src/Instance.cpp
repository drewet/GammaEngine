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
#include "Debug.h"
#include "gememory.h"

namespace GE {

Instance* Instance::mBaseInstance = nullptr;

Instance* Instance::baseInstance()
{
	return mBaseInstance;
}


Instance::Instance( const char* appName, uint32_t appVersion, bool easy_instance )
{
	fDebug( appName, appVersion, easy_instance );

	if ( !mBaseInstance ) {
		mBaseInstance = this;
	}


	mAppInfo.pAppName = appName;
	mAppInfo.appVersion = appVersion;
	mAppInfo.pEngineName = "Gamma Engine";
	mAppInfo.engineVersion = 0x00020000;
	mAppInfo.apiVersion = VK_API_VERSION;

	mAllocCb.pfnAlloc = &Instance::sAlloc;
	mAllocCb.pfnFree = &Instance::sFree;

	vkCreateInstance( &mAppInfo, &mAllocCb, &mInstance );

	if ( easy_instance ) {
		EnumerateGpus();
		CreateDevice( 0, 1 );
	}
}


void* Instance::sAlloc( size_t size, size_t align, int32_t allocType )
{
	fDebug( size, align, allocType );

	return geMemalign( size, align, false );
}


void Instance::sFree( void* pMem )
{
	fDebug( pMem );

	if ( pMem ) {
		geFree( pMem );
	}
}


Instance::~Instance()
{
}


int Instance::EnumerateGpus()
{
	fDebug0();

	vkEnumerateGpus( mInstance, sizeof(mGpus) / sizeof(mGpus[0]), &mGpuCount, mGpus );
	return mGpuCount;
}


void Instance::CreateDevice( int devid, int queueCount )
{
	fDebug( devid, queueCount );

	VK_DEVICE_QUEUE_CREATE_INFO queueInfo = {};
	queueInfo.queueType = 0;
	queueInfo.queueCount = queueCount;

	VK_DEVICE_CREATE_INFO deviceInfo = {};
	deviceInfo.queueRecordCount = 1;
	deviceInfo.pRequestedQueues = &queueInfo;
	deviceInfo.extensionCount = 0; // TODO
	deviceInfo.ppEnabledExtensionNames = nullptr; // TODO
	deviceInfo.flags |= VK_DEVICE_CREATE_VALIDATION;
	deviceInfo.maxValidationLevel = VK_VALIDATION_LEVEL_4;

	vkCreateDevice( mGpus[devid], &deviceInfo, &mDevices[devid] );

	vkGetDeviceQueue( mDevices[devid], 0, 0, &mQueues[devid] );

	VK_FENCE_CREATE_INFO fenceCreateInfo = {};
	vkCreateFence( mDevices[devid], &fenceCreateInfo, &mFences[devid] );
}


VK_MEMORY_REF Instance::AllocateObject( int devid, VK_OBJECT object )
{
	VK_MEMORY_REF ret = {};

	VK_MEMORY_REQUIREMENTS memReqs = {};
	VK_SIZE memReqsSize = sizeof(memReqs);
	vkGetObjectInfo( object, VK_INFO_TYPE_MEMORY_REQUIREMENTS, &memReqsSize, &memReqs );

	VK_MEMORY_HEAP_PROPERTIES heapProps = {};
	VK_SIZE heapPropsSize = sizeof(heapProps);
	vkGetMemoryHeapInfo( mDevices[devid], memReqs.heaps[0], VK_INFO_TYPE_MEMORY_HEAP_PROPERTIES, &heapPropsSize, &heapProps );

	if ( heapProps.pageSize <= 0 ) {
		VK_MEMORY_REF ret = { 0 };
		return ret;
	}

	VK_MEMORY_ALLOC_INFO allocInfo = {};
	VK_GPU_MEMORY memory;
	allocInfo.size = ( 1 + memReqs.size / heapProps.pageSize ) * heapProps.pageSize;
	allocInfo.alignment = 0; // TESTING/TODO : 16/32/64 perf improv ??
	allocInfo.memPriority = VK_MEMORY_PRIORITY_HIGH;
	allocInfo.heapCount = 1;
	allocInfo.heaps[0] = memReqs.heaps[0];
	vkAllocMemory( mDevices[devid], &allocInfo, &memory );

	vkBindObjectMemory( object, memory, 0 );
	ret.mem = memory;

	return ret;
}


VK_MEMORY_REF Instance::AllocateMappableBuffer( int devid, size_t size )
{
	VK_MEMORY_REF ret = {};

	// Find CPU visible (mappable) heap
	VK_UINT heapCount;
	vkGetMemoryHeapCount( mDevices[devid], &heapCount );
	VK_MEMORY_HEAP_PROPERTIES heapProps = {};
	VK_SIZE heapPropsSize = sizeof( heapProps );
	VK_UINT suitableHeap = -1;
	for ( VK_UINT i = 0; i < heapCount; i++ ) {
		vkGetMemoryHeapInfo( mDevices[devid], i, VK_INFO_TYPE_MEMORY_HEAP_PROPERTIES, &heapPropsSize, &heapProps );
		if ( heapProps.flags & VK_MEMORY_HEAP_CPU_VISIBLE ) {
			suitableHeap = i;
			break;
		}
	}


	if ( heapProps.pageSize <= 0 ) {
		ret = { 0 };
		return ret;
	}

	VK_MEMORY_ALLOC_INFO allocInfo = {};
	VK_GPU_MEMORY memory;
	allocInfo.size = ( 1 + size / heapProps.pageSize ) * heapProps.pageSize;
	allocInfo.alignment = 0;
	allocInfo.memPriority = VK_MEMORY_PRIORITY_HIGH;
	allocInfo.heapCount = 1;
	allocInfo.heaps[0] = suitableHeap;
	vkAllocMemory( mDevices[devid], &allocInfo, &memory );

	ret.mem = memory;
	return ret;
}


void Instance::QueueSubmit( int devid, VK_CMD_BUFFER buf, VK_MEMORY_REF* refs, int nRefs )
{
	vkQueueSubmit( mQueues[devid], 1, &buf, nRefs, refs, mFences[devid] );

// 	vkWaitForFences( mDevices[devid], 1, &mFences[devid], true, 0.0f ); // TESTING
}


VK_PHYSICAL_GPU Instance::gpu( int devid )
{
	return mGpus[ devid ];
}


VK_DEVICE Instance::device( int devid )
{
	return mDevices[devid];
}


VK_QUEUE Instance::queue( int devid )
{
	return mQueues[devid];
}

} // namespace GE
