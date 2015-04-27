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

#include "Matrix.h"
#include <cmath>

#define PI_OVER_360 0.0087266f
#define PI_OVER_180 0.0174532f
#define PI_OVER_90 0.0349065f

namespace GE {

Matrix::Matrix()
{
	Identity();
}


float* Matrix::data()
{
	return m;
}


void Matrix::Identity()
{
	m[0] = 1.0f;
	m[1] = 0.0f;
	m[2] = 0.0f;
	m[3] = 0.0f;
	m[4] = 0.0f;
	m[5] = 1.0f;
	m[6] = 0.0f;
	m[7] = 0.0f;
	m[8] = 0.0f;
	m[9] = 0.0f;
	m[10] = 1.0f;
	m[11] = 0.0f;
	m[12] = 0.0f;
	m[13] = 0.0f;
	m[14] = 0.0f;
	m[15] = 1.0f;
}


void Matrix::Perspective( float fov, float aspect, float zNear, float zFar )
{
	float h = 1.0f / std::tan( fov * PI_OVER_360 );
	float neg_depth = zNear - zFar;

	m[0] = h / aspect;
	m[1] = 0.0f;
	m[2] = 0.0f;
	m[3] = 0.0f;
	m[4] = 0.0f;
	m[5] = h;
	m[6] = 0.0f;
	m[7] = 0.0f;
	m[8] = 0.0f;
	m[9] = 0.0f;
	m[10] = ( zFar + zNear ) / neg_depth;
	m[11] = -1.0f;
	m[12] = 0.0f;
	m[13] = 0.0f;
	m[14] = 2.0f * ( zNear * zFar ) / neg_depth;
	m[15] = 0.0f;
}


void Matrix::LookAt( Vector3f& eye, Vector3f& center, Vector3f& up )
{
	Matrix t;
	Vector3f f;
	Vector3f u;
	Vector3f s;

	f = center - eye;
	f.normalize();

	s = f ^ up;
	s.normalize();

	u = s ^ f;

	t.m[0] = s.x;
	t.m[4] = s.y;
	t.m[8] = s.z;
	t.m[12]= 0.0;

	t.m[1] = u.x;
	t.m[5] = u.y;
	t.m[9] = u.z;
	t.m[13]= 0.0;

	t.m[2] = -f.x;
	t.m[6] = -f.y;
	t.m[10]= -f.z;
	t.m[14]= 0.0;

	t.m[3] = 0.0;
	t.m[7] = 0.0;
	t.m[11]= 0.0;
	t.m[15]= 1.0;

	*this *= t;
	Translate( -eye.x, -eye.y, -eye.z );
}


void Matrix::Translate( float x, float y, float z )
{
	Matrix t;
	t.Identity();

	t.m[12] = x;
	t.m[13] = y;
	t.m[14] = z;

	*this *= t;
}


void Matrix::Scale( float x, float y, float z )
{
	Matrix t;
	t.Identity();

	t.m[0] = x;
	t.m[5] = y;
	t.m[10] = z;

	*this *= t;
}


void Matrix::RotateX( float a )
{
	Matrix t;
	t.Identity();

	float c = std::cos( a );
	float s = std::sin( a );
	t.m[1*4+1] = c;
	t.m[1*4+2] = s;
	t.m[2*4+1] = -s;
	t.m[2*4+2] = c;

	*this *= t;
}


void Matrix::RotateY( float a )
{
	Matrix t;
	t.Identity();

	float c = std::cos( a );
	float s = std::sin( a );
	t.m[0*4+0] = c;
	t.m[0*4+2] = -s;
	t.m[2*4+0] = s;
	t.m[2*4+2] = c;

	*this *= t;
}


void Matrix::RotateZ( float a )
{
	Matrix t;
	t.Identity();

	float c = std::cos( a );
	float s = std::sin( a );
	t.m[0*4+0] = c;
	t.m[0*4+1] = s;
	t.m[1*4+0] = -s;
	t.m[1*4+1] = c;

	*this *= t;
}


Matrix Matrix::operator*( Matrix& other )
{
	Matrix ret;
	int i=0, j=0, k=0;

	for ( i = 0; i < 16; i++ ) {
		ret.m[i] = m[j] * other.m[k] + m[j+4] * other.m[k+1] + m[j+8] * other.m[k+2] + m[j+12] * other.m[k+3];
		j = ( j + 1 ) % 4;
		k += 4 * ( j == 4 );
	}

	return ret;
}

void Matrix::operator*=( Matrix& other )
{
	*this = *this * other;
}

} // namespace GE
