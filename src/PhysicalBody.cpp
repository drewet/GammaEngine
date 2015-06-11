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
#include <limits>
#include <stdio.h>
#include "Debug.h"
#include "PhysicalBody.h"

using namespace GE;

static int ___i = 0;

PhysicalBody::PhysicalBody( const Vector3f& pos, float m )
	: Time()
	, mMass( m )
	, mInertiaTensor( 0.0f )
	, mFriction( 0.0f )
	, mPosition( pos )
	, mCollideType( BOX )
	, mCollided( false )
{
	if(___i++ == 0){
// 		mMomentum.z = 0.1;
// 		mMomentum.y = mMass * 1.0f;
// 		mMomentum.y = 0.88888f;
	}else if(___i == 2){
		mMomentum.y = -mMass;
// 		mMomentum.y = 0.33333f;
	}
}


PhysicalBody::~PhysicalBody()
{
}


void PhysicalBody::setMass( float m )
{
	mMass = m;
}


void PhysicalBody::setBoundingBox( const Vector3f& min, const Vector3f& max )
{
	mBoundingBoxMin = min;
	mBoundingBoxMax = max;

	mInertiaTensor = mMass * std::abs( max.x - min.x ) * std::abs( max.y - min.y ) * std::abs( max.z - min.z ) / 6.0f;

	mBoundingBox[0] = Vector3f( min.x, min.y, min.z );
	mBoundingBox[1] = Vector3f( max.x, min.y, min.z );
	mBoundingBox[2] = Vector3f( min.x, max.y, min.z );
	mBoundingBox[3] = Vector3f( max.x, max.y, min.z );
	mBoundingBox[4] = Vector3f( min.x, min.y, max.z );
	mBoundingBox[5] = Vector3f( max.x, min.y, max.z );
	mBoundingBox[6] = Vector3f( min.x, max.y, max.z );
	mBoundingBox[7] = Vector3f( max.x, max.y, max.z );

	mBoundingDiagonal.x = std::max( std::abs( max.x ), std::abs( min.x ) );
	mBoundingDiagonal.y = std::max( std::abs( max.y ), std::abs( min.y ) );
	mBoundingDiagonal.z = std::max( std::abs( max.z ), std::abs( min.z ) );
	mBoundingDiagonal.w = 0.0f;
	mBoundingDiagonal.w = mBoundingDiagonal.length();
}


void PhysicalBody::setCollideType( PhysicalBody::CollideType type )
{
	mCollideType = type;
}


void PhysicalBody::setFriction( float f )
{
	mFriction = f;
}



Vector3f& PhysicalBody::velocity()
{
	return mVelocity;
}


Vector3f& PhysicalBody::position()
{
	return mPosition;
}


Matrix& PhysicalBody::matrix()
{
	mMatrix.Identity();

	mMatrix.Translate( mPosition.x, mPosition.y, mPosition.z );
	mMatrix *= mOrientation.matrix();

	return mMatrix;
}


Matrix PhysicalBody::rotationMatrix()
{
	return mOrientation.matrix();
}


Matrix PhysicalBody::inverseRotationMatrix()
{
	return mOrientation.inverseMatrix();
}


PhysicalBody::CollideType PhysicalBody::collisionType()
{
	return mCollideType;
}


Vector3f* PhysicalBody::collisionMesh( uint32_t* nVerts )
{
	//TODO
	if ( nVerts) {
		*nVerts = 8;
	}
	return mBoundingBox;
}


void PhysicalBody::ResetForces()
{
	mForce.x = 0.0f;
	mForce.y = 0.0f;
	mForce.z = 0.0f;
}

void PhysicalBody::ResetTorque()
{
	mTorque.x = 0.0f;
	mTorque.y = 0.0f;
	mTorque.z = 0.0f;
}



void PhysicalBody::ApplyForce( const Vector3f& f )
{
	mForce.x += f.x;
	mForce.y += f.y;
	mForce.z += f.z;
}


void PhysicalBody::ApplyTorque( const Vector3f& t )
{
	mTorque.x += t.x;
	mTorque.y += t.y;
	mTorque.z += t.z;
}


void PhysicalBody::ApplyGravity( PhysicalBody* other )
{
	const double G = 6.67234e-11;
	Vector3f dir = other->mPosition - mPosition;
	Vector3f force;
	
	double d = std::sqrt( dir.x * dir.x + dir.y * dir.y + dir.z * dir.z );
	if ( d <= 0.5 || d == std::numeric_limits<double>::infinity() ) {
		mMomentum.x = mMomentum.y = mMomentum.z = 0.0f;
		mVelocity.x = mVelocity.y = mVelocity.z = 0.0f;
		return;
	}
	double F = G * ( ( mMass * other->mMass ) / ( d * d ) );

	force = dir;
	force.normalize();
	force *= F;

	ApplyForce( force );
// 	other->ApplyForce( -force );
		aDebug( "mForce", mForce.x, mForce.y, mForce.z );
}


void PhysicalBody::Update()
{
// 	double dt = SlowSync( 1.0 / 5000.0 );
	double dt = Sync();
// 	if ( dt > 0.0 ) {
		RK4Integrate( dt );
		mCollided = false;
// 	}
}


PhysicalBody::RK4Deriv PhysicalBody::RK4Evaluate()
{
	RK4Deriv output;

	output.velocity = mVelocity;
	output.force = mForce;
	output.torque = mTorque;
	output.spin = mSpin;

	return output;
}


PhysicalBody::RK4Deriv PhysicalBody::RK4Evaluate( double dt, const RK4Deriv& d )
{
	RK4Deriv output;

	output.force = mForce;
	output.torque = mTorque;
	output.velocity = ( mMomentum + d.force * dt ) * ( 1.0f / mMass );
	Quaternion orientation = mOrientation + d.spin * dt;
	orientation.normalize();
	Vector3f angularVelocity = ( mAngularMomentum + d.torque * dt ) * ( 1.0f / mInertiaTensor );
	output.spin = 0.5f * Quaternion( 0.0f, angularVelocity.x, angularVelocity.y, angularVelocity.z ) * orientation;
	output.spin = 0.5f * Quaternion( angularVelocity.x, angularVelocity.y, angularVelocity.z, 0.0f ) * orientation;

	return output;
}


void PhysicalBody::RK4Integrate( double ddt )
{
	float dt = ddt;
	RK4Deriv a;// = RK4Evaluate();
	a.velocity = mVelocity;
	a.force = mForce;
	a.torque = mTorque;
	a.spin = mSpin;

	RK4Deriv b = RK4Evaluate( ddt * 0.5f, a );
	RK4Deriv c = RK4Evaluate( ddt * 0.5f, b );
	RK4Deriv d = RK4Evaluate( ddt, c );

// 	aDebug( "mMomentum", mMomentum.x, mMomentum.y, mMomentum.z );
	mPosition += 1.0f / 6.0f * dt * ( a.velocity + 2.0f * ( b.velocity + c.velocity ) + d.velocity );
// 	aDebug( "mPosition", mPosition.x, mPosition.y, mPosition.z );
// 	mMomentum += 1.0f / 6.0f * dt * ( a.force + 2.0f * ( b.force + c.force ) + d.force );
	mMomentum = ( mMomentum + dt * mForce ) - ( mMomentum * mFriction * dt );
	mOrientation += 1.0f / 6.0f * dt * ( a.spin + 2.0f * ( b.spin + c.spin ) + d.spin );
	mAngularMomentum += dt * mTorque;

	mVelocity = mMomentum * ( 1.0f / mMass );
	mAngularVelocity = mAngularMomentum * ( 1.0f / mInertiaTensor );
	mOrientation.normalize();
	mSpin = 0.5f * Quaternion( mAngularVelocity.x, mAngularVelocity.y, mAngularVelocity.z, 0.0f ) * mOrientation;
}


void PhysicalBody::HandleCollision( PhysicalBody* other, const Vector3f& pCollide, Vector3f normal )
{
	if ( mCollided ) {
		return;
	}

	mCollided = true;
	other->mCollided = true;

	aDebug( "mAngularMomentum_befor", mAngularMomentum.x, mAngularMomentum.y, mAngularMomentum.z );

// 	Vector3f normal = other->position() - position();
	normal.normalize();

	Vector3f collisionPoint = pCollide - position();
	Vector3f collisionPoint2 = pCollide - other->position();
	aDebug( "normal", normal.x, normal.y, normal.z );
	aDebug( "collisionPoint", collisionPoint.x, collisionPoint.y, collisionPoint.z );
	aDebug( "collisionPoint2", collisionPoint2.x, collisionPoint2.y, collisionPoint2.z );
// 	collisionPoint.normalize();
// 	collisionPoint2.normalize();

	mInertiaTensor = other->mInertiaTensor = 0.2107f;

	Vector3f angularVelChangea = normal ^ collisionPoint;
	aDebug( "angularVelChangea", angularVelChangea.x, angularVelChangea.y, angularVelChangea.z );
	angularVelChangea = angularVelChangea * ( 1.0f / mInertiaTensor );
	Vector3f vaLinDueToR = angularVelChangea ^ collisionPoint;
	float scalar = 1.0 / mMass + ( vaLinDueToR * normal );

	Vector3f angularVelChangeb = normal ^ collisionPoint2;
	angularVelChangeb = angularVelChangeb * ( 1.0f / other->mInertiaTensor );
	Vector3f vbLinDueToR = angularVelChangeb ^ collisionPoint2;
	scalar += 1.0 / other->mMass + ( vbLinDueToR * normal );

	double Jmod = (1+1) * ( mMomentum - other->mMomentum ).length() / scalar;
// 	double Jmod = (1+1) * ( ( mMomentum - other->mMomentum ) * normal ) / scalar;
	Vector3f J = normal * Jmod;
	mMomentum = mMomentum - J * ( 1.0f / mMass );
	other->mMomentum = other->mMomentum + J * ( 1.0f / other->mMass );
	mAngularMomentum = mAngularMomentum - angularVelChangea;
	other->mAngularMomentum = other->mAngularMomentum + angularVelChangeb;

	aDebug( "mAngularMomentum_after", mAngularMomentum.x, mAngularMomentum.y, mAngularMomentum.z );
// 	exit(0);

/*
	Vector3f momentum = mMomentum;
	Vector3f momentum2 = other->mMomentum;
	mMomentum = momentum * ( ( mMass - other->mMass ) / ( mMass + other->mMass ) ) + momentum2 * ( ( mMass * 2.0f ) / ( mMass + other->mMass ) );
	other->mMomentum = momentum * ( ( other->mMass * 2.0f ) / ( mMass + other->mMass ) ) + momentum2 * ( ( other->mMass - mMass ) / ( mMass + other->mMass ) );
*/
	mVelocity = Vector3f();
	other->mVelocity = Vector3f();

	mForce = Vector3f();
	other->mForce = Vector3f();
}


void PhysicalBody::Collide( PhysicalBody* other )
{
	if ( mCollided ) {
		return;
	}
	int i;
	int collide = 0;
	Vector3f pCollide;
	Vector3f pCollideOther;

	// Copy bodies and go a little step ahead in time to avoid overlapping
	PhysicalBody this_copy = *this;
	PhysicalBody other_copy = *other;
	this_copy.RK4Integrate( Sync() );
	other_copy.RK4Integrate( Sync() );
	Matrix& this_matrix = this_copy.matrix();
	Matrix& other_matrix = other_copy.matrix();
	Matrix this_rotationMatrix = this_copy.rotationMatrix();
	Matrix other_rotationMatrix = other_copy.rotationMatrix();

	if ( mCollideType == BOX ) {
		if ( other->mCollideType == BOX ) {


			Vector3f p1[8];
			Vector3f p2[8];
			for ( i = 0; i < 8; i++ ) {
				p1[i] = this_matrix * Vector4f( mBoundingBox[i], 1.0f );
				p2[i] = other_matrix * Vector4f( other->mBoundingBox[i], 1.0f );
			}
/*
			Vector3f axisX_0 = this_rotationMatrix * Vector4f( 1.0f, 0.0f, 0.0f, 1.0f );
			Vector3f axisY_0 = this_rotationMatrix * Vector4f( 0.0f, 1.0f, 0.0f, 1.0f );
			Vector3f axisZ_0 = this_rotationMatrix * Vector4f( 0.0f, 0.0f, 1.0f, 1.0f );
	
			Vector3f axisX_1 = other_rotationMatrix * Vector4f( 1.0f, 0.0f, 0.0f, 1.0f );
			Vector3f axisY_1 = other_rotationMatrix * Vector4f( 0.0f, 1.0f, 0.0f, 1.0f );
			Vector3f axisZ_1 = other_rotationMatrix * Vector4f( 0.0f, 0.0f, 1.0f, 1.0f );
*/
			Vector3f axisX_0 = this_matrix * Vector4f( 1.0f, 0.0f, 0.0f, 1.0f );
			Vector3f axisY_0 = this_matrix * Vector4f( 0.0f, 1.0f, 0.0f, 1.0f );
			Vector3f axisZ_0 = this_matrix * Vector4f( 0.0f, 0.0f, 1.0f, 1.0f );
	
			Vector3f axisX_1 = other_matrix * Vector4f( 1.0f, 0.0f, 0.0f, 1.0f );
			Vector3f axisY_1 = other_matrix * Vector4f( 0.0f, 1.0f, 0.0f, 1.0f );
			Vector3f axisZ_1 = other_matrix * Vector4f( 0.0f, 0.0f, 1.0f, 1.0f );


			collide += CollideSeparatingAxis( p1, p2, axisX_0, &pCollide.x );
			collide += CollideSeparatingAxis( p1, p2, axisY_0, &pCollide.y );
			collide += CollideSeparatingAxis( p1, p2, axisZ_0, &pCollide.z );

			collide += CollideSeparatingAxis( p1, p2, axisX_1 );
			collide += CollideSeparatingAxis( p1, p2, axisY_1 );
			collide += CollideSeparatingAxis( p1, p2, axisZ_1 );

			collide += CollideSeparatingAxis( p1, p2, axisX_0 ^ axisX_1 );
			collide += CollideSeparatingAxis( p1, p2, axisX_0 ^ axisY_1 );
			collide += CollideSeparatingAxis( p1, p2, axisX_0 ^ axisZ_1 );

			collide += CollideSeparatingAxis( p1, p2, axisY_0 ^ axisX_1 );
			collide += CollideSeparatingAxis( p1, p2, axisY_0 ^ axisY_1 );
			collide += CollideSeparatingAxis( p1, p2, axisY_0 ^ axisZ_1 );

			collide += CollideSeparatingAxis( p1, p2, axisZ_0 ^ axisX_1 );
			collide += CollideSeparatingAxis( p1, p2, axisZ_0 ^ axisY_1 );
			collide += CollideSeparatingAxis( p1, p2, axisZ_0 ^ axisZ_1 );
/*
			Vector3f normal = other_copy.position() - this_copy.position();
			pCollide.x = ( normal * axisX_0 ) * axisX_0.x + this_copy.position().x;
			pCollide.y = ( normal * axisY_0 ) * axisY_0.y + this_copy.position().y;
			pCollide.z = ( normal * axisZ_0 ) * axisZ_0.z + this_copy.position().z;
*/
		}
	}

	if ( collide >= 15 ) {
// 		pCollide += other_copy.mPosition;
		pCollideOther = pCollide - other_copy.mPosition;// + this_copy.mPosition;
		pCollide += this_copy.mPosition;
		pCollideOther += other_copy.mPosition;
		aDebug( "pCollide", pCollide.x, pCollide.y, pCollide.z );
		aDebug( "pCollideOther", pCollideOther.x, pCollideOther.y, pCollideOther.z );
		mCollided = true;
		other->mCollided = true;

/**
This function calulates the velocities after a 3D collision vaf, vbf, waf and wbf from information about the colliding bodies
@param double e coefficient of restitution which depends on the nature of the two colliding materials
@param double ma total mass of body a
@param double mb total mass of body b
@param matrix Ia inertia tensor for body a in absolute coordinates (if this is known in local body coordinates it must
                 be converted before this is called).
@param matrix Ib inertia tensor for body b in absolute coordinates (if this is known in local body coordinates it must
                 be converted before this is called).
@param vector ra position of collision point relative to centre of mass of body a in absolute coordinates (if this is
                 known in local body coordinates it must be converted before this is called).
@param vector rb position of collision point relative to centre of mass of body b in absolute coordinates (if this is
                 known in local body coordinates it must be converted before this is called).
@param vector n normal to collision point, the line along which the impulse acts.
@param vector vai initial velocity of centre of mass on object a
@param vector vbi initial velocity of centre of mass on object b
@param vector wai initial angular velocity of object a
@param vector wbi initial angular velocity of object b
@param vector vaf final velocity of centre of mass on object a
@param vector vbf final velocity of centre of mass on object a
@param vector waf final angular velocity of object a
@param vector wbf final angular velocity of object b
*/
/*
CollisionResponce(double e,double ma,double mb,matrix Ia,matrix Ib,vector ra,vector rb,vector n,
    vector vai, vector vbi, vector wai, vector wbi, vector vaf, vector vbf, vector waf, vector wbf) {
  matrix IaInverse = Ia.inverse();
  vector normal = n.normalise();
  vector angularVelChangea  = normal.copy(); // start calculating the change in abgular rotation of a
  angularVelChangea.cross(ra);
  IaInverse.transform(angularVelChangea);
  vector vaLinDueToR = angularVelChangea.copy().cross(ra);  // calculate the linear velocity of collision point on a due to rotation of a
  double scalar = 1/ma + vaLinDueToR.dot(normal);
  matrix IbInverse = Ib.inverse();
  vector angularVelChangeb = normal.copy(); // start calculating the change in abgular rotation of b
  angularVelChangeb.cross(rb);
  IbInverse.transform(angularVelChangeb);
  vector vbLinDueToR = angularVelChangeb.copy().cross(rb);  // calculate the linear velocity of collision point on b due to rotation of b
  scalar += 1/mb + vbLinDueToR.dot(normal);
  double Jmod = (e+1)*(vai-vbi).magnitude()/scalar;
  vector J = normal.mul(Jmod);
  vaf = vai - J.mul(1/ma);
  vbf = vbi - J.mul(1/mb);
  waf = wai - angularVelChangea;
  wbf = wbi - angularVelChangeb;
}
*/
		aDebug( "mAngularMomentum_befor", mAngularMomentum.x, mAngularMomentum.y, mAngularMomentum.z );

		Vector3f normal = other_copy.position() - this_copy.position();
		normal.normalize();

		pCollide.normalize();
		pCollideOther.normalize();

		Vector3f angularVelChangea = normal * mInertiaTensor;// * 10000000.0;
		angularVelChangea = angularVelChangea ^ pCollide;
		Vector3f vaLinDueToR = angularVelChangea ^ pCollide;
		float scalar = 1.0 / mMass + ( vaLinDueToR * normal );

		Vector3f angularVelChangeb = normal * mInertiaTensor;// * 10000000.0;
		angularVelChangeb = angularVelChangeb ^ pCollideOther;
		Vector3f vbLinDueToR = angularVelChangeb ^ pCollideOther;
		scalar += 1.0 / other->mMass + ( vbLinDueToR * normal );

		double Jmod = (1+1) * ( mMomentum - other->mMomentum ).length() / scalar;
		Vector3f J = normal * Jmod;
		mMomentum = mMomentum - J * ( 1.0f / mMass );
		other->mMomentum = other->mMomentum + J * ( 1.0f / other->mMass );
		mAngularMomentum = mAngularMomentum + angularVelChangea;
		other->mAngularMomentum = other->mAngularMomentum + angularVelChangeb;

		aDebug( "mAngularMomentum_after", mAngularMomentum.x, mAngularMomentum.y, mAngularMomentum.z );

		Vector3f momentum = mMomentum;
		Vector3f momentum2 = other->mMomentum;
// 		mMomentum = momentum * ( ( mMass - other->mMass ) / ( mMass + other->mMass ) ) + momentum2 * ( ( mMass * 2.0f ) / ( mMass + other->mMass ) );
// 		other->mMomentum = momentum * ( ( other->mMass * 2.0f ) / ( mMass + other->mMass ) ) + momentum2 * ( ( other->mMass - mMass ) / ( mMass + other->mMass ) );

		mVelocity = Vector3f();
		other->mVelocity = Vector3f();

		mForce = Vector3f();
		other->mForce = Vector3f();
	}
}


bool PhysicalBody::CollidePointToBox( const Vector3f& point, const Vector3f& min, const Vector3f& max )
{
	if ( point.x >= min.x && point.x <= max.x && point.y >= min.y && point.y <= max.y && point.z >= min.z && point.z <= max.z ) {
		return true;
	}
	return false;
}

bool PhysicalBody::CollideSeparatingAxis( Vector3f* aCorn, Vector3f* bCorn, Vector3f axis, float* res )
{
	// Handles the cross product = {0,0,0} case
	if( axis.x == 0.0f && axis.y == 0.0f && axis.z == 0.0f ) { 
	    return true;
	}

	float aMin = std::numeric_limits< float >::max();
	float aMax = std::numeric_limits< float >::min();
	float bMin = std::numeric_limits< float >::max();
	float bMax = std::numeric_limits< float >::min();

	// Define two intervals, a and b. Calculate their min and max values
	for( int i = 0; i < 8; i++ ) {
		float aDist = aCorn[i] * axis;
		aMin = ( aDist < aMin ) ? aDist : aMin;
		aMax = ( aDist > aMax ) ? aDist : aMax;
		float bDist =  bCorn[i] * axis;
		bMin = ( bDist < bMin ) ? bDist : bMin;
		bMax = ( bDist > bMax ) ? bDist : bMax;
	}

	// One-dimensional intersection test between a and b
	double longSpan = std::max( aMax, bMax ) - std::min( aMin, bMin );
	double sumSpan = aMax - aMin + bMax - bMin;
	bool ret = std::max( longSpan - sumSpan, 0.0 ) <= 0.001; // Change this to <= if you want the case were they are touching but not overlapping, to count as an intersection

	if ( res ) {
		*res = /*(aMax - aMin) / 2.0f +*/ (sumSpan - longSpan) / 2.0f;
		*res = (aMax - aMin) / 2.0f - (sumSpan - longSpan) / 2.0f;
	}

	if ( ret ) {
// 		gDebug() << "longSpan = " << longSpan << "   sumSpan = " << sumSpan << "\n";
// 		vDebug( "axis", axis.x, axis.y, axis.z ) << " collides\n";
	}

	return ret;
}
