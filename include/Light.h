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

#ifndef GE_LIGHT_H
#define GE_LIGHT_H

#include "Vector.h"

namespace GE
{

class Light
{
public:
	typedef enum Type {
		Point,
		Spot,
	} Type;

	Light( Type type, const Vector3f& position, const Vector3f& direction = Vector3f(), float angle = 70.0f );
	~Light();

	Type type();
	const Vector3f& position();
	const Vector3f& direction();
	float angle();

private:
	Type mType;
	Vector3f mPosition;
	Vector3f mDirection;
	float mAngle;
};

} // namespace GE

#endif // GE_LIGHT_H
