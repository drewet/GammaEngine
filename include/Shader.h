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

#ifndef SHADER_H
#define SHADER_H

#include "vulkan.h"

class Instance;

class Shader
{
public:
	Shader( Instance* instance, const std::string vs, const std::string fs, int devid = 0 );
	~Shader();

private:
	uint8_t* loadFile( const std::string filename, size_t* sz );

	VK_SHADER mVertexShader;
	VK_SHADER mFragmentShader;
};

#endif // SHADER_H
