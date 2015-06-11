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

#ifndef INSTANCE_H
#define INSTANCE_H

#include <string>

namespace GE {

class Window;
class Renderer;
class Vertex;
class Object;

class Instance
{
public:
// 	Instance( const char* appName, uint32_t appVersion, bool easy_instance = true );
	Instance() : mDevId(0), mGpuCount(0), mGpus{0}, mDevices{0}, mQueues{0}, mFences{0}, mCpuRamCounter(0), mGpuRamCounter(0) {}
	virtual ~Instance(){}

	static Instance* Create( const char* appName, uint32_t appVersion, bool easy_instance = true );
	virtual int EnumerateGpus() = 0;
	virtual Instance* CreateDevice( int devid, int queueCount = 1 ) = 0;

	Window* CreateWindow( const std::string& title, int width, int height, int flags = 0 );
	Renderer* CreateRenderer();
	Object* CreateObject( Vertex* verts = nullptr, uint32_t nVerts = 0 );
	Object* LoadObject( const std::string& filename );

	void* Memalign( uintptr_t size, uintptr_t align, bool clear_mem = true );
	void* Realloc( void* last, uintptr_t size, bool clear_mem = true );
	void* Malloc( uintptr_t size, bool clear_mem = true );
	void Free( void* data );

	uint64_t gpu();
	uint64_t device();
	uint64_t queue();

	uint64_t cpuRamCounter();
	uint64_t gpuRamCounter();

	static Instance* baseInstance();
	static void* backend();

protected:
	static Instance* mBaseInstance;
	static void* sBackend;

	int mDevId;
	uint32_t mGpuCount;
	uint64_t mGpus[4];
	uint64_t mDevices[4];
	uint64_t mQueues[4];
	uint64_t mFences[4];

	uint64_t mCpuRamCounter;
	uint64_t mGpuRamCounter;
};

} // namespace GE

#endif // INSTANCE_H
