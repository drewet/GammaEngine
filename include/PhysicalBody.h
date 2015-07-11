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
#include "Quaternion.h"
#include "Matrix.h"
#include "Time.h"

namespace GE
{

/*
 * PhysicalBody :
 *   - using RK4 to calculate forces
 *   - mass is in kg
 *   - distances are in meters
 *   - velocity is in m/s
 */

class PhysicalBody : public Time
{
public:
	typedef enum {
		BOX = 0,
		SPHERE = 1,
		MESH = 10
	} CollideType;

	PhysicalBody( const Vector3f& pos = { 0.0f }, float mass = 1.0f );
	~PhysicalBody();

	void setMass( float m );
	void setBoundingBox( const Vector3f& min, const Vector3f& max );
	void setCollideType( CollideType type );
	void setFriction( float f );

	Vector3f& velocity();
	Vector3f& position();
	Matrix& matrix();
	Matrix rotationMatrix();
	Matrix inverseRotationMatrix();
	CollideType collisionType();
	Vector3f* collisionMesh( uint32_t* nVerts = nullptr );

	void ResetForces();
	void ResetTorque();
	void Update();

	void ApplyForce( const Vector3f& f );
	void ApplyGravity( PhysicalBody* other );

	void ApplyTorque( const Vector3f& t );

	void Collide( PhysicalBody* other );
	void HandleCollision( PhysicalBody* other, const Vector3f& pCollide, Vector3f normal );

protected:
	typedef struct RK4Deriv {
		Vector3f velocity;	// m/s
		Vector3f force;		// N
		Quaternion spin;
		Vector3f torque;
	} RK4Deriv;

	RK4Deriv RK4Evaluate();
	RK4Deriv RK4Evaluate( double dt, const RK4Deriv& d );
	void RK4Integrate( double dt );

	bool CollidePointToBox( const Vector3f& point, const Vector3f& min, const Vector3f& max );
	bool CollideSeparatingAxis( Vector3f* aCorn, Vector3f* bCorn, Vector3f axis, float* res = nullptr );

	float mMass;
	Vector3f mBoundingBox[8];
	Vector3f mBoundingBoxMin;
	Vector3f mBoundingBoxMax;
	float mInertiaTensor;
	float mFriction;

	Vector3f mPosition;
	Vector3f mForce;
	Vector3f mTorque;
	Vector3f mMomentum;
	Vector3f mVelocity;

	Quaternion mOrientation;
	Vector3f mAngularMomentum;
	Quaternion mSpin;
	Vector3f mAngularVelocity;

	Matrix mMatrix;

	CollideType mCollideType;
	Vector4f mBoundingDiagonal;

	bool mCollided;
};


}

#endif // GE_PHYSICALOBJECT_H
