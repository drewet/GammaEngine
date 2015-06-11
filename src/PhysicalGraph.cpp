/*
 * <one line to give the library's name and an idea of what it does.>
 * Copyright (C) 2015  Adrien Aubry <email>
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

#include "PhysicalGraph.h"
#include "Instance.h"

using namespace GE;

PhysicalGraph::PhysicalGraph( Instance* instance )
	: Time()
	, mInstance( instance ? instance : Instance::baseInstance() )
	, mCollider( new PhysicalCollider( this ) )
{
}


PhysicalGraph::~PhysicalGraph()
{
}


Instance* PhysicalGraph::instance()
{
	return mInstance;
}


PhysicalCollider* PhysicalGraph::collider()
{
	return mCollider;
}


void PhysicalGraph::AddBody( PhysicalBody* body )
{
	body->setTimeParent( this );
	mBodies.emplace_back( body );
}


void PhysicalGraph::Update()
{
	double dt = SlowSync( 1.0 / 60.0 );
	if ( std::abs( dt ) <= 0.0 ) {
		return;
	}

	decltype(mBodies)::iterator it;
	decltype(mBodies)::iterator it2;

	for ( it = mBodies.begin(); it != mBodies.end(); ++it ) {
		it2 = it;
		while( ++it2 != mBodies.end() ) {
// 			(*it)->Collide( *it2 );
			mCollider->Collide( *it, *it2 );
		}
		(*it)->Update();
	}
}
