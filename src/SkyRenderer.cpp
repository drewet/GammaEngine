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

#include "Instance.h"
#include "SkyRenderer.h"
#include "Object.h"
#include "Light.h"
#include "Camera.h"
#include "MeshBuilder.h"
#include "Debug.h"

using namespace GE;

SkyRenderer::SkyRenderer( Instance* instance, float domeRadius )
	: mRenderer( instance->CreateRenderer() )
	, mDome( nullptr )
	, mDomeRadius( domeRadius )
{
	mRenderer->projectionMatrix()->Perspective( 60.0f, 16.0f / 9.0f, 1.0f, 200000.0f );

	MeshBuilder builder( MeshBuilder::Sphere, Vector3f( domeRadius, domeRadius, domeRadius ), 4 );
	Vertex* verts = nullptr;
	uint32_t nVerts = 0;
	uint32_t* indices = nullptr;
	uint32_t nIndices = 0;
	builder.GenerateIndexedVertexArray( instance, &verts, &nVerts, &indices, &nIndices, true );
	gDebug() << "mDome = instance->CreateObject( " << verts << ", " << nVerts << ", " << indices << ", " << nIndices << " )\n";
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

