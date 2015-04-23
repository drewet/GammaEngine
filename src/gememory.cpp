#include <malloc.h>
#include <string.h>
#include <stdint.h>


#ifndef ALIGN
	#define ALIGN(x, align) (((x)+((align)-1))&~((align)-1))
#endif


uintptr_t _ge_AllocMemBlock( int size )
{
	return (uintptr_t)malloc( size );
}


void* geMemalign( int size, int align, bool clear_mem )
{
	int block_sz = sizeof(uintptr_t) * 2;
	uintptr_t addr = _ge_AllocMemBlock( size + ( align > block_sz ? align : block_sz ) + align );
	uintptr_t* var = (uintptr_t*)( ALIGN( addr + ( align > block_sz ? align : block_sz ), align ) - block_sz );
	var[0] = addr;
	var[1] = size;
	memset( (void*)(uintptr_t)&var[2], 0x0, size );
	// TODO : Update RAM counter
	return (void*)(uintptr_t)&var[2];
}


void* geMalloc( int size, bool clear_mem )
{
	if ( size <= 0 ) {
		return NULL;
	}
	return geMemalign( size, 16, clear_mem );
}


void geFree( void* data )
{
	if ( data != NULL && data != (void*)0xDEADBEEF && data != (void*)0xBAADF00D ) {
		uintptr_t* var = (uintptr_t*)data;
		uintptr_t addr = var[-2];
		uintptr_t size = var[-1];
		free((void*)addr);
		// TODO : Update RAM counter
	}
}


void* geRealloc( void* last, int size, bool clear_mem )
{
	if ( size <= 0 ) {
		geFree( last );
		return NULL;
	}
	if ( last == NULL || last == (void*)0xDEADBEEF || last == (void*)0xBAADF00D ) {
		return geMalloc( size, clear_mem );
	}
	uintptr_t last_size = ((uintptr_t*)last)[-1];
	
	uintptr_t addr = _ge_AllocMemBlock( size + sizeof(uintptr_t) * 2 + 16 );
	uintptr_t* var = (uintptr_t*)(ALIGN(addr + sizeof(uintptr_t) * 2, 16 ) - sizeof(uintptr_t) * 2 );
	var[0] = addr;
	var[1] = size;
	void* new_ptr = (void*)&var[2];
	if ( clear_mem ) {
		memset( (void*)(uintptr_t)&var[2], 0x0, size );
	}

	int sz_copy = last_size < size ? last_size : size;
	memcpy(new_ptr, last, sz_copy);

	var = (uintptr_t*)last;
	free((void*)var[-2]);

	// TODO : Update RAM counter

	return new_ptr;
}
