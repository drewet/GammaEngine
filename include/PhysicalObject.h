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

#ifndef GE_PHYSICALOBJECT_H
#define GE_PHYSICALOBJECT_H

#include "Vector.h"
#include "Matrix.h"
#include "Time.h"

namespace GE
{

/*
 * PhysicalObject :
 *   - using RK4 to calculate forces
 *   - mass is in kg
 *   - distances are in meters
 *   - velocity is in m/s
 */

class PhysicalObject : protected Time
{
public:
	PhysicalObject( float mass = 1.0f );
	~PhysicalObject();

	void setMass( float m );
	Vector3f& velocity();

	void ApplyForce( const Vector3f& f );
	void ApplyGravity( PhysicalObject* other );
	void Update();

protected:
	typedef struct RK4Deriv {
		Vector3f velocity;	// m/s
		Vector3f force;		// N
	} RK4Deriv;

	void RK4Recalculate();
	RK4Deriv RK4Evaluate();
	RK4Deriv RK4Evaluate( double dt, RK4Deriv d );
	void RK4Integrate( double dt );

	Vector3f mPosition;
	Vector3f mForce;
	Vector3f mMomentum;
	Vector3f mVelocity;
	float mMass;
};


}

#endif // GE_PHYSICALOBJECT_H
