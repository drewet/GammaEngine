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

#ifndef VERTEX_H
#define VERTEX_H

#include <stdint.h>
#include <vulkan.h>

namespace GE {

class Vertex
{
public:
	Vertex();

	static void UpdateDescriptorSet( VK_DESCRIPTOR_SET descriptorSet, VK_MEMORY_VIEW_ATTACH_INFO* memoryViewAttachInfo );

// Attributes defined as public for fast access
public:
	float u, v, w, _align1;
	float color[4];
	float nx, ny, nz, _align2;
	float x, y, z, weight;
} __attribute__((packed, aligned(16))); // Stay cool with memory

/*
typedef struct Vertex {
	float u, v, w, _align1;
	float color[4];
	float nx, ny, nz, _align2;
	float x, y, z, weight;
} Vertex;
*/
} // namespace GE

#endif // VERTEX_H
