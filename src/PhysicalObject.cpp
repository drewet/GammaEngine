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
#include "PhysicalObject.h"

using namespace GE;


PhysicalObject::PhysicalObject( float m )
	: Time()
	, mPosition( Vector3f() )
	, mForce( Vector3f() )
	, mMomentum( Vector3f() )
	, mVelocity( Vector3f() )
	, mMass( m )
{
}


PhysicalObject::~PhysicalObject()
{
}


void PhysicalObject::setMass( float m )
{
	mMass = m;
}


Vector3f& PhysicalObject::velocity()
{
	return mVelocity;
}


void PhysicalObject::ApplyForce( const Vector3f& f )
{
	mForce.x += f.x;
	mForce.y += f.y;
	mForce.z += f.z;
}


void PhysicalObject::ApplyGravity( PhysicalObject* other )
{
	const double G = 6.67234E-11;
	Vector3f dir = other->mPosition - mPosition;
	Vector3f force;

	double d = std::sqrt( dir.x * dir.x + dir.y * dir.y + dir.z * dir.z );
	if ( d <= 1.0 ) {
		return;
	}
	double F = G * ( ( mMass * other->mMass ) / ( d * d ) );

	force.normalize();
	force *= F;

	ApplyForce( force );
	other->ApplyForce( -force );
}


void PhysicalObject::Update()
{
	float dt = Sync();
	RK4Integrate( dt );
}


void PhysicalObject::RK4Recalculate()
{
	mVelocity = mMomentum * ( 1.0f / mMass );
}


PhysicalObject::RK4Deriv PhysicalObject::RK4Evaluate()
{
	RK4Deriv output;

	output.velocity = mVelocity;
	output.force = mForce;

	return output;
}


PhysicalObject::RK4Deriv PhysicalObject::RK4Evaluate( double dt, RK4Deriv d )
{
	RK4Deriv output;
	PhysicalObject state = *this;

	mPosition += d.velocity * dt;

	state.mPosition += d.velocity * dt;
	state.mMomentum += d.force * dt;
	state.RK4Recalculate();

	output.force = state.mForce;
	output.velocity = state.mVelocity;

	return output;
}


void PhysicalObject::RK4Integrate( double dt )
{
	RK4Deriv a = RK4Evaluate();
	RK4Deriv b = RK4Evaluate( dt * 0.5f, a );
	RK4Deriv c = RK4Evaluate( dt * 0.5f, b );
	RK4Deriv d = RK4Evaluate( dt, c );

	mPosition += 1.0f / 6.0f * dt * ( a.velocity + 2.0f * ( b.velocity + c.velocity ) + d.velocity );
	mMomentum += 1.0f / 6.0f * dt * ( a.force + 2.0f * ( b.force + c.force ) + d.force );

	RK4Recalculate();
}
