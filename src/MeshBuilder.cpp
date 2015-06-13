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

#include "MeshBuilder.h"
#include "Instance.h"

using namespace GE;

MeshBuilder::MeshBuilder( BaseType basetype, const Vector3f& size, int tesslevel )
{
	if ( basetype == Sphere ) {
		const Vector3f icosahedron[12] = {
			{  1.618033, 1.0, 0.0 }, // 0
			{ -1.618033, 1.0, 0.0 }, // 1
			{  1.618033,-1.0, 0.0 }, // 2
			{ -1.618033,-1.0, 0.0 }, // 3
			{  1.0, 0.0, 1.618033 }, // 4
			{  1.0, 0.0,-1.618033 }, // 5
			{ -1.0, 0.0, 1.618033 }, // 6
			{ -1.0, 0.0,-1.618033 }, // 7
			{  0.0, 1.618033, 1.0 }, // 8
			{  0.0,-1.618033, 1.0 }, // 9
			{  0.0, 1.618033,-1.0 }, // 10
			{  0.0,-1.618033,-1.0 }  // 11
		};
		mFaces.emplace_back( Face( icosahedron[0], icosahedron[8], icosahedron[4] ) );
		mFaces.emplace_back( Face( icosahedron[1], icosahedron[10], icosahedron[7] ) );
		mFaces.emplace_back( Face( icosahedron[2], icosahedron[9], icosahedron[11] ) );
		mFaces.emplace_back( Face( icosahedron[7], icosahedron[3], icosahedron[1] ) );    
		mFaces.emplace_back( Face( icosahedron[0], icosahedron[5], icosahedron[10] ) );
		mFaces.emplace_back( Face( icosahedron[3], icosahedron[9], icosahedron[6] ) );
		mFaces.emplace_back( Face( icosahedron[3], icosahedron[11], icosahedron[9] ) );
		mFaces.emplace_back( Face( icosahedron[8], icosahedron[6], icosahedron[4] ) );    
		mFaces.emplace_back( Face( icosahedron[2], icosahedron[4], icosahedron[9] ) );
		mFaces.emplace_back( Face( icosahedron[3], icosahedron[7], icosahedron[11] ) );
		mFaces.emplace_back( Face( icosahedron[4], icosahedron[2], icosahedron[0] ) );
		mFaces.emplace_back( Face( icosahedron[9], icosahedron[4], icosahedron[6] ) );    
		mFaces.emplace_back( Face( icosahedron[2], icosahedron[11], icosahedron[5] ) );
		mFaces.emplace_back( Face( icosahedron[0], icosahedron[10], icosahedron[8] ) );
		mFaces.emplace_back( Face( icosahedron[5], icosahedron[0], icosahedron[2] ) );
		mFaces.emplace_back( Face( icosahedron[10], icosahedron[5], icosahedron[7] ) );    
		mFaces.emplace_back( Face( icosahedron[1], icosahedron[6], icosahedron[8] ) );
		mFaces.emplace_back( Face( icosahedron[1], icosahedron[8], icosahedron[10] ) );
		mFaces.emplace_back( Face( icosahedron[6], icosahedron[1], icosahedron[3] ) );
		mFaces.emplace_back( Face( icosahedron[11], icosahedron[7], icosahedron[5] ) );
		for ( int i = 0; i < tesslevel; i++ ) {
			Tesselate( Normalize );
		}
	}
}

MeshBuilder::~MeshBuilder()
{
}


void MeshBuilder::Tesselate( MeshBuilder::TesselationMethod method )
{
	std::vector< Face > newFaces;

	/*    0
	     /\
	  01/__\02
	   /\  /\
	  /__\/__\
	 1   12   2
	*/

	for ( size_t i = 0; i < mFaces.size(); i++ ) {
		Vector3f p0 = mFaces[i].p0();
		Vector3f p1 = mFaces[i].p1();
		Vector3f p2 = mFaces[i].p2();
		Vector3f p01 = ( p0 + p1 ) * 0.5f;
		Vector3f p12 = ( p1 + p2 ) * 0.5f;
		Vector3f p02 = ( p0 + p2 ) * 0.5f;
		newFaces.emplace_back( Face( p0, p01, p02 ) );
		newFaces.emplace_back( Face( p01, p1, p12 ) );
		newFaces.emplace_back( Face( p01, p12, p02 ) );
		newFaces.emplace_back( Face( p02, p12, p2 ) );
	}

	mFaces = newFaces;
}


void MeshBuilder::GenerateVertexArray( Instance* instance, Vertex** verts, uint32_t* nVerts )
{
	Vertex* ret = (Vertex*)instance->Malloc( sizeof(Vertex) * mFaces.size() * 3 );

	for ( size_t i = 0; i < mFaces.size(); i++ ) {
		ret[i * 3 + 0] = Vertex( mFaces[i].p0() );
		ret[i * 3 + 1] = Vertex( mFaces[i].p1() );
		ret[i * 3 + 2] = Vertex( mFaces[i].p2() );
	}

	*verts = ret;
	*nVerts = mFaces.size() * 3;
}
