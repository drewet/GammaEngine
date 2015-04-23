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
**/

#ifndef INSTANCE_H
#define INSTANCE_H

#include "vulkan.h"

class Instance
{
public:
	Instance( const char* appName, uint32_t appVersion, bool easy_instance = true );
	~Instance();

	int EnumerateGpus();
	void CreateDevice( int devid, int queueCount );

	VK_DEVICE device( int devid );

	static void* sAlloc( size_t size, size_t align, int32_t allocType );
	static void sFree( void* pMem );

private:
	VK_INSTANCE mInstance;
	VK_DEVICE mDevices[VK_MAX_PHYSICAL_GPUS];
	VK_QUEUE mQueue;

	uint32_t mGpuCount;
	VK_PHYSICAL_GPU mGpus[VK_MAX_PHYSICAL_GPUS];
};

#endif // INSTANCE_H
