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

#include "Vertex.h"

namespace GE {

Vertex::Vertex()
	: u( 0.0f ), v( 0.0f ), w( 0.0f ), _align1( 0.0f )
	, color{ 0.0f }
	, nx( 0.0f ), ny( 0.0f ), nz( 0.0f ), _align2( 0.0f )
	, x( 0.0f ), y( 0.0f ), z( 0.0f ), weight( 0.0f )
{
}


} // namespace GE
