/*
 * The GammaEngine Library 2.0 is a multiplatform -based game engine
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

#include "OpenGL43Instance.h"
#include "OpenGL43Object.h"
#include "File.h"
#include "Image.h"
#include "Debug.h"

// #include <algorithm>


using namespace GE;

extern "C" GE::Object* CreateObject( Vertex* verts, uint32_t nVerts ) {
	return new OpenGL43Object( verts, nVerts );
}

extern "C" GE::Object* LoadObject( const std::string filename, Instance* instance ) {
	return new OpenGL43Object( filename, instance );
}


OpenGL43Object::OpenGL43Object( Vertex* verts, uint32_t nVerts )
	: Object( verts, nVerts )
{
}


OpenGL43Object::OpenGL43Object( const std::string filename, Instance* instance )
	: Object( filename, instance )
{
}


OpenGL43Object::~OpenGL43Object()
{
// 	Object::~Object();
}


const std::vector< std::pair< Image*, uint32_t > >& OpenGL43Object::textures()
{
	return mTextures;
}


void OpenGL43Object::setTexture( int unit, Image* texture )
{
	if ( (int)mTextures.size() <= unit ) {
		mTextures.resize( unit + 1, std::make_pair( nullptr, 0 ) );
	}

	uint32_t glTextureID = mTextures[unit].second;
	if ( !glTextureID ) {
		glGenTextures( 1, &glTextureID );
	}
	glBindTexture( GL_TEXTURE_2D, glTextureID );

	glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA8, texture->width(), texture->height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, texture->data() );
	glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );
	glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
	glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
	((OpenGL43Instance*)Instance::baseInstance())->AffectVRAM( texture->width() * texture->height() * sizeof( uint32_t ) );

	glBindTexture( GL_TEXTURE_2D, 0 );
	mTextures[unit] = std::make_pair( texture, glTextureID );
}


void OpenGL43Object::UpdateVertices( Instance* instance, Vertex* verts, uint32_t offset, uint32_t count )
{
}


void OpenGL43Object::UploadMatrix( Instance* instance )
{
}
