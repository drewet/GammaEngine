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

#ifndef RENDERER_H
#define RENDERER_H

#include <vector>
#include "vulkan.h"

namespace GE {

class Instance;
class Object;

class Renderer
{
public:
	Renderer( Instance* instance = nullptr, int devid = 0 );
	~Renderer();

	int LoadVertexShader( const std::string file );
	int LoadFragmentShader( const std::string file );

	void AddObject( Object* obj );

	void Compute();
	void Draw();

private:
	uint8_t* loadShader( const std::string filename, size_t* sz );
	void createPipeline();

	bool mReady;
	Instance* mInstance;
	int mDevId;

	std::vector< Object* > mObjects;

	VK_CMD_BUFFER mCmdBuffer;
	VK_PIPELINE mPipeline;
	VK_MEMORY_REF mPipelineRef;
	VK_SHADER mVertexShader;
	VK_SHADER mFragmentShader;
};

} // namespace GE

#endif // RENDERER_H
