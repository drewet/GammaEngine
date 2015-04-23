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
#include "gememory.h"
#include <malloc.h>

Instance::Instance( const char* appName, uint32_t appVersion, bool easy_instance )
{
	VK_APPLICATION_INFO appInfo;
	VK_ALLOC_CALLBACKS allocCb;

	appInfo.pAppName = appName;
	appInfo.appVersion = appVersion;
	appInfo.pEngineName = "Gamma Engine";
	appInfo.engineVersion = 0x00020000;
	appInfo.apiVersion = VK_API_VERSION;

	allocCb.pfnAlloc = &Instance::sAlloc;
	allocCb.pfnFree = &Instance::sFree;

	vkCreateInstance( &appInfo, &allocCb, &mInstance );

	if ( easy_instance ) {
		EnumerateGpus();
		CreateDevice( 0, 1 );
	}
}


void* Instance::sAlloc( size_t size, size_t align, int32_t allocType )
{
	return geMemalign( size, align, false );
}


void Instance::sFree( void* pMem )
{
	if ( pMem ) {
		geFree( pMem );
	}
}


Instance::~Instance()
{
}


int Instance::EnumerateGpus()
{
	vkEnumerateGpus( mInstance, sizeof(mGpus) / sizeof(mGpus[0]), &mGpuCount, mGpus );
	return mGpuCount;
}


void Instance::CreateDevice( int devid, int queueCount )
{
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

	vkGetDeviceQueue( mDevices[devid], 0, 0, &mQueue );
}


VK_DEVICE Instance::device( int devid )
{
	return mDevices[devid];
}
