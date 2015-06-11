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

#include "Light.h"

using namespace GE;

Light::Light( Type type, const Vector3f& position, const Vector3f& direction, float angle )
	: mType( type )
	, mPosition( position )
	, mDirection( direction )
	, mAngle( angle )
{
}


Light::~Light()
{
}


Light::Type Light::type()
{
	return mType;
}


const Vector3f& Light::position()
{
	return mPosition;
}


const Vector3f& Light::direction()
{
	return mDirection;
}


float Light::angle()
{
	return mAngle;
}
