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

#include "Shader.h"
#include "gememory.h"
#include <Instance.h>


Shader::Shader( Instance* instance, const std::string vs, const std::string fs, int devid )
{
	VK_SHADER_CREATE_INFO vsInfo = { 0 };
	vsInfo.pCode = loadFile( vs, &vsInfo.codeSize );
	vkCreateShader( instance->device( devid ), &vsInfo, &mVertexShader );

	VK_SHADER_CREATE_INFO fsInfo = { 0 };
	fsInfo.pCode = loadFile( fs, &fsInfo.codeSize );
	vkCreateShader( instance->device( devid ), &fsInfo, &mFragmentShader );
}
Shader::~Shader()
{

}


uint8_t* Shader::loadFile( const std::string filename, size_t* sz )
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