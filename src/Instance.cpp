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

#include <dlfcn.h>

#include "Instance.h"
#include "Debug.h"

#ifndef ALIGN
	#define ALIGN(x, align) (((x)+((align)-1))&~((align)-1))
#endif

namespace GE {

Instance* Instance::mBaseInstance = nullptr;
void* Instance::sBackend = nullptr;
uint64_t Instance::mBaseThread = 0;

Instance* Instance::baseInstance()
{
	return mBaseInstance;
}


uint64_t Instance::baseThread()
{
	return mBaseThread;
}


void* Instance::backend()
{
	return sBackend;
}


Instance* Instance::Create( const char* appName, uint32_t appVersion, bool easy_instance )
{
	if ( !sBackend ) {
// 		sBackend = dlopen( "build/backends/vulkan/backend_vulkan.so", RTLD_LAZY );
		sBackend = dlopen( "build/backends/opengl43/backend_opengl43.so", RTLD_LAZY );
		if ( sBackend == nullptr ) {
			gDebug() << "Backend loading error : " << dlerror() << ")\n";
			exit(0);
		}
	}

	typedef Instance* (*f_type)( const char*, uint32_t );
	f_type fCreateInstance = (f_type)dlsym( backend(), "CreateInstance" );
	if ( easy_instance ) {
		if ( !mBaseInstance ) {
			mBaseInstance = fCreateInstance( appName, appVersion );
			mBaseThread = pthread_self();
		}
		Instance* ret = mBaseInstance->CreateDevice( 0, 1 );
		if ( !mBaseInstance || mBaseInstance->device() == 0 ) {
			mBaseInstance = ret;
		}
		return ret;
	}
	return fCreateInstance( appName, appVersion );
}


Window* Instance::CreateWindow( const std::string& title, int width, int height, int flags )
{
	typedef Window* (*f_type)( Instance*, const std::string&, int, int, int );
	f_type fCreateWindow = (f_type)dlsym( backend(), "CreateWindow" );
	return fCreateWindow( this, title, width, height, flags );
}


Renderer* Instance::CreateRenderer()
{
	typedef Renderer* (*f_type)( Instance* );
	f_type fCreateRenderer = (f_type)dlsym( backend(), "CreateRenderer" );
	return fCreateRenderer( this );
}


Renderer2D* Instance::CreateRenderer2D( uint32_t width, uint32_t height )
{
	typedef Renderer2D* (*f_type)( Instance*, uint32_t, uint32_t );
	f_type fCreateRenderer2D = (f_type)dlsym( backend(), "CreateRenderer2D" );
	return fCreateRenderer2D( this, width, height );
}


DeferredRenderer* Instance::CreateDeferredRenderer( uint32_t width, uint32_t height )
{
	typedef DeferredRenderer* (*f_type)( Instance*, uint32_t, uint32_t );
	f_type fCreateDeferredRenderer = (f_type)dlsym( backend(), "CreateDeferredRenderer" );
	return fCreateDeferredRenderer( this, width, height );
}


Object* Instance::CreateObject( Vertex* verts, uint32_t nVert, uint32_t* indices, uint32_t nIndices )
{
	typedef Object* (*f_type)( Vertex*, uint32_t, uint32_t*, uint32_t );
	f_type fCreateObject = (f_type)dlsym( backend(), "CreateObject" );
	return fCreateObject( verts, nVert, indices, nIndices );
}


Object* Instance::LoadObject( const std::string& filename )
{
	typedef Object* (*f_type)( const std::string&, Instance* );
	f_type fLoadObject = (f_type)dlsym( backend(), "LoadObject" );
	return fLoadObject( filename, this );
}

/*
std::vector< Object* > Instance::LoadObjects( const std::string& filename )
{
	typedef std::vector< Object* > (*f_type)( const std::string&, Instance* );
	f_type fLoadObjects = (f_type)dlsym( backend(), "LoadObjects" );
	return fLoadObjects( filename, this );
}
*/

static uintptr_t _ge_AllocMemBlock( uintptr_t size )
{
	return (uintptr_t)malloc( size );
}


void* Instance::Memalign( uintptr_t size, uintptr_t align, bool clear_mem )
{
	uintptr_t block_sz = sizeof(uintptr_t) * 2;
	size_t fullSize = size + ( align > block_sz ? align : block_sz ) + align;
	uintptr_t addr = _ge_AllocMemBlock( fullSize );
	uintptr_t* var = (uintptr_t*)( ALIGN( addr + ( align > block_sz ? align : block_sz ), align ) - block_sz );
	var[0] = addr;
// 	var[1] = size;
	var[1] = fullSize;
	memset( (void*)(uintptr_t)&var[2], 0x0, size );
// 	mCpuRamCounter += size;
	mCpuRamCounter += fullSize;
	return (void*)(uintptr_t)&var[2];
}


void* Instance::Malloc( uintptr_t size, bool clear_mem )
{
	if ( size <= 0 ) {
		return NULL;
	}
	return Memalign( size, 16, clear_mem );
}


void Instance::Free( void* data )
{
	if ( data != NULL && data != (void*)0xDEADBEEF && data != (void*)0xBAADF00D ) {
		uintptr_t* var = (uintptr_t*)data;
		uintptr_t addr = var[-2];
		size_t size = var[-1];
		free( (void*)addr );
		mCpuRamCounter -= size;
	}
}


void* Instance::Realloc( void* last, uintptr_t size, bool clear_mem )
{
	if ( size <= 0 ) {
		Free( last );
		return NULL;
	}
	if ( last == NULL || last == (void*)0xDEADBEEF || last == (void*)0xBAADF00D ) {
		return Malloc( size, clear_mem );
	}
	uintptr_t last_size = ((uintptr_t*)last)[-1];
	
	const int align = 16;
	uintptr_t block_sz = sizeof(uintptr_t) * 2;
	size_t fullSize = size + ( align > block_sz ? align : block_sz ) + align;
	uintptr_t addr = _ge_AllocMemBlock( fullSize );
	uintptr_t* var = (uintptr_t*)( ALIGN( addr + ( align > block_sz ? align : block_sz ), align ) - block_sz );
// 	size_t fullSize = size + sizeof(uintptr_t) * 2 + 16;
// 	uintptr_t addr = _ge_AllocMemBlock( fullSize );
// 	uintptr_t* var = (uintptr_t*)(ALIGN(addr + sizeof(uintptr_t) * 2, 16 ) - sizeof(uintptr_t) * 2 );
	var[0] = addr;
	var[1] = size;
	void* new_ptr = (void*)&var[2];
	if ( clear_mem ) {
		memset( (void*)(uintptr_t)&var[2], 0x0, size );
	}

	uintptr_t sz_copy = last_size < size ? last_size : size;
	memcpy(new_ptr, last, sz_copy);

	var = (uintptr_t*)last;
	size_t lastSize = var[-1];
	free((void*)var[-2]);

	mCpuRamCounter -= lastSize;
	mCpuRamCounter += size;

	return new_ptr;
}


uint64_t Instance::cpuRamCounter()
{
	return mCpuRamCounter;
}


uint64_t Instance::gpuRamCounter()
{
	return mGpuRamCounter;
}


uint64_t Instance::gpu()
{
	return mGpus[ mDevId ];
}


uint64_t Instance::device()
{
	return mDevices[mDevId];
}


uint64_t Instance::queue()
{
	return mQueues[mDevId];
}

} // namespace GE
