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

#ifndef VECTOR_H
#define VECTOR_H

namespace GE {

class Vector3f
{
public:
	Vector3f( float x = 0.0f, float y = 0.0f, float z = 0.0f );

	void normalize();

	Vector3f operator+( Vector3f& v );
	Vector3f operator-( Vector3f& v );
	Vector3f operator^( Vector3f& v );
	float operator*( Vector3f& v );

// Attributes defined as public for fast access
public:
	float x;
	float y;
	float z;
} __attribute__((packed));

} // namespace GE

#endif // VECTOR_H
