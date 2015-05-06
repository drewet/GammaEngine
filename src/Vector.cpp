/*
 * <one line to give the library's name and an idea of what it does.>
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

#include "Vector.h"
#include <cmath>

namespace GE {




Vector2i::Vector2i( int x, int y )
	: x( x )
	, y( y )
{
}


void Vector2i::normalize()
{
	float l = std::sqrt( x * x + y * y );
	if ( l > 0.00001f ) {
		float il = 1.0f / l;
		x *= il;
		y *= il;
	}
}


Vector2i Vector2i::operator+( const Vector2i& v )
{
	Vector2i ret;

	ret.x = x + v.x;
	ret.y = y + v.y;

	return ret;
}


Vector2i Vector2i::operator-( const Vector2i& v )
{
	Vector2i ret;

	ret.x = x - v.x;
	ret.y = y - v.y;

	return ret;
}


int Vector2i::operator*( const Vector2i& v )
{
	return x * v.x + y * v.y;
}





Vector2f::Vector2f( float x, float y )
	: x( x )
	, y( y )
{
}


void Vector2f::normalize()
{
	float l = std::sqrt( x * x + y * y );
	if ( l > 0.00001f ) {
		float il = 1.0f / l;
		x *= il;
		y *= il;
	}
}


Vector2f Vector2f::operator+( const Vector2f& v )
{
	Vector2f ret;

	ret.x = x + v.x;
	ret.y = y + v.y;

	return ret;
}


Vector2f Vector2f::operator-( const Vector2f& v )
{
	Vector2f ret;

	ret.x = x - v.x;
	ret.y = y - v.y;

	return ret;
}


float Vector2f::operator*( const Vector2f& v )
{
	return x * v.x + y * v.y;
}


void Vector2f::operator*=( float v )
{
	x *= v;
	y *= v;
}





Vector3f::Vector3f( float x, float y, float z )
	: x( x )
	, y( y )
	, z( z )
{
}


void Vector3f::normalize()
{
	float l = std::sqrt( x * x + y * y + z * z );
	if ( l > 0.00001f ) {
		float il = 1.0f / l;
		x *= il;
		y *= il;
		z *= il;
	}
}


Vector3f Vector3f::operator-()
{
	Vector3f ret;

	ret.x = -x;
	ret.y = -y;
	ret.z = -z;

	return ret;
}


Vector3f Vector3f::operator+( const Vector3f& v )
{
	Vector3f ret;

	ret.x = x + v.x;
	ret.y = y + v.y;
	ret.z = z + v.z;

	return ret;
}


Vector3f Vector3f::operator-( const Vector3f& v )
{
	Vector3f ret;

	ret.x = x - v.x;
	ret.y = y - v.y;
	ret.z = z - v.z;

	return ret;
}


Vector3f Vector3f::operator^( const Vector3f& v )
{
	Vector3f ret;

	ret.x = y * v.z - z * v.y;
	ret.y = z * v.x - x * v.z;
	ret.z = x * v.y - y * v.x;

	return ret;
}


float Vector3f::operator*( const Vector3f& v )
{
	return x * v.x + y * v.y + z * v.z;
}


void Vector3f::operator+=( const Vector3f& v )
{
	x += v.x;
	y += v.y;
	z += v.z;
}


void Vector3f::operator*=( float v )
{
	x *= v;
	y *= v;
	z *= v;
}


Vector3f operator*( float f, const Vector3f& v )
{
	Vector3f ret = v;

	ret.x *= f;
	ret.y *= f;
	ret.z *= f;

	return ret;
}

} // namespace GE
