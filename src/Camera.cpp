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

#include <cmath>
#include "Camera.h"

namespace GE {

Camera::Camera()
	: Time()
	, mMatrix( Matrix() )
	, mPosition( Vector3f( 0.0f, 0.0f, 0.0f ) )
	, mLookPoint( Vector3f( 1.0f, 0.0f, 0.0f ) )
	, mUpVector( Vector3f( 0.0f, 0.0f, 1.0f ) )
	, mRotV( 0.0f )
	, mRotH( 0.0f )
{
}


Camera::~Camera()
{
}


void Camera::UpVector( const Vector3f& up )
{
	mUpVector = up;
}


void Camera::LookAt( const Vector3f& pos, const Vector3f& center )
{
	mPosition = pos;
	mLookPoint = center;
}


void Camera::WalkForward( float speed )
{
	float dt = Sync();
	speed *= dt;

	float x = std::cos( mRotH ) * speed;
	float y = std::sin( mRotH ) * speed;

	mPosition.x += x;
	mPosition.y += y;

	mLookPoint.x += x;
	mLookPoint.y += y;
}


void Camera::WalkBackward( float speed )
{
	float dt = Sync();
	speed *= dt;

	float x = std::cos( mRotH ) * speed;
	float y = std::sin( mRotH ) * speed;

	mPosition.x -= x;
	mPosition.y -= y;

	mLookPoint.x -= x;
	mLookPoint.y -= y;
}


void Camera::WalkLeft( float speed )
{
	float dt = Sync();
	speed *= dt;

	float x = std::cos( mRotH ) * speed;
	float y = std::sin( mRotH ) * speed;

	mPosition.x -= y;
	mPosition.y += x;

	mLookPoint.x -= y;
	mLookPoint.y += x;
}


void Camera::WalkRight( float speed )
{
	float dt = Sync();
	speed *= dt;

	float x = std::cos( mRotH ) * speed;
	float y = std::sin( mRotH ) * speed;

	mPosition.x += y;
	mPosition.y -= x;

	mLookPoint.x += y;
	mLookPoint.y -= x;
}


void Camera::UpdateLookPoint()
{
	float rtemp = 10000000.0f * std::cos( mRotV );
	mLookPoint.z = mPosition.z + 10000000.0f * std::sin( mRotV );
	mLookPoint.x = mPosition.x + rtemp * std::cos( mRotH );
	mLookPoint.y = mPosition.y + rtemp * std::sin( mRotH );
}


float* Camera::data()
{
	mMatrix.Identity();
	mMatrix.LookAt( mPosition, mLookPoint, mUpVector );
	return mMatrix.data();
}

} // namespace GE
