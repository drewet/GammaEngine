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

#include <vector>
#include <unordered_map>
#include "Instance.h"
#include "SkyRenderer.h"
#include "Object.h"
#include "Light.h"
#include "Camera.h"
#include "MeshBuilder.h"
#include "Debug.h"

using namespace GE;

bool SkyRenderer::BuilderRemoveCB( MeshBuilder::Face* face, void* data )
{
	return face->p0().z >= 0.0f || face->p1().z >= 0.0f || face->p2().z >= 0.0f;
}


void SkyRenderer::BuilderModCB( MeshBuilder::Face* face, void* data )
{
	Vector3f p0 = face->p0();
	Vector3f p1 = face->p1();
	Vector3f p2 = face->p2();
	p0.z = std::max( p0.z, 0.0f );
	p1.z = std::max( p1.z, 0.0f );
	p2.z = std::max( p2.z, 0.0f );
	*face = MeshBuilder::Face( p0, p1, p2 );
}


SkyRenderer::SkyRenderer( Instance* instance, float domeRadius )
	: mRenderer( instance->CreateRenderer() )
	, mDome( nullptr )
	, mDomeRadius( domeRadius )
{
	mRenderer->projectionMatrix()->Perspective( 60.0f, 16.0f / 9.0f, 1.0f, domeRadius + 10.0f );

	MeshBuilder builder( MeshBuilder::Sphere, Vector3f( domeRadius, domeRadius, domeRadius ), 4 );
	builder.Translate( { 0.0f, 0.0f, -domeRadius + domeRadius / 18.0f } );
	builder.RemoveFaces( &BuilderRemoveCB );
	builder.Translate( { 0.0f, 0.0f, domeRadius - domeRadius / 18.0f } );
	builder.Tesselate( MeshBuilder::Normalize );
	builder.Tesselate( MeshBuilder::Normalize );
	builder.Tesselate( MeshBuilder::Normalize );
	builder.Translate( { 0.0f, 0.0f, -domeRadius + domeRadius / 18.0f } );
	builder.SinglePassFaces( &BuilderModCB );

	Vertex* verts = nullptr;
	uint32_t nVerts = 0;
	uint32_t* indices = nullptr;
	uint32_t nIndices = 0;
	builder.GenerateIndexedVertexArray( instance, &verts, &nVerts, &indices, &nIndices, true );
	mDome = instance->CreateObject( verts, nVerts, indices, nIndices );

	mRenderer->LoadVertexShader( "shaders/skydome.vert" );
	mRenderer->LoadFragmentShader( "shaders/skydome.frag" );
	mRenderer->AddObject( mDome );
	mRenderer->Compute();
}


SkyRenderer::~SkyRenderer()
{
}


void SkyRenderer::AddLight( Light* light )
{
	mLights.emplace_back( light );
}


void SkyRenderer::Render( Camera* camera )
{
	mRenderer->Look( camera );
	mRenderer->Draw();
}

