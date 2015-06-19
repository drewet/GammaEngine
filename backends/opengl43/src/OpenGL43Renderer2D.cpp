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

#include <fstream>
#include <vector>

#include "Window.h"
#include "Instance.h"
#include "OpenGL43Object.h"
#include "OpenGL43Instance.h"
#include "OpenGL43Renderer2D.h"
#include "OpenGL43Window.h"
#include "Object.h"
#include "Debug.h"
#include "File.h"
#include "Camera.h"
#include "Image.h"

#define GLSL(version, shader)  "#version " #version "\n" #shader

extern "C" GE::Renderer2D* CreateRenderer2D( GE::Instance* instance, uint32_t width, uint32_t height ) {
	return new OpenGL43Renderer2D( instance, width, height );
}
static const char vertices_shader_base[] = GLSL(420,
	layout(location = 0) in vec3 ge_VertexTexcoord;
	layout(location = 1) in vec4 ge_VertexColor;
	layout(location = 2) in vec3 ge_VertexNormal;
	layout(location = 3) in vec3 ge_VertexPosition;

	layout (binding=0, std140) uniform ge_Matrices_0
	{
		mat4 ge_ProjectionMatrix;
	};

	layout (binding=1, std140) uniform ge_Matrices_1
	{
		mat4 ge_ViewMatrix;
	};

	out vec4 ge_Color;
	out vec3 ge_TextureCoord;

	void main()
	{
		ge_Color = ge_VertexColor;
		ge_TextureCoord = ge_VertexTexcoord;
		gl_Position = ge_ProjectionMatrix * ge_ViewMatrix * vec4(ge_VertexPosition, 1.0);
	}
);

static const char fragment_shader_base[] = GLSL(420,
	uniform sampler2D ge_Texture0;
	in vec4 ge_Color;
	in vec3 ge_TextureCoord;

	void main()
	{
		gl_FragColor = ge_Color * texture2D( ge_Texture0, ge_TextureCoord.xy );
		gl_FragColor.a = 1.0;
	}
);


OpenGL43Renderer2D::OpenGL43Renderer2D( Instance* instance, uint32_t width, uint32_t height )
	: Renderer2D()
	, OpenGL43Renderer( instance )
	, m2DReady( false )
	, mWidth( width )
	, mHeight( height )
{
	mMatrixProjection->Orthogonal( 0.0, mWidth, mHeight, 0.0, -2049.0, 2049.0 );
	mMatrixView->Identity();

	OpenGL43Renderer::LoadVertexShader( vertices_shader_base, sizeof(vertices_shader_base) + 1 );
	OpenGL43Renderer::LoadFragmentShader( fragment_shader_base, sizeof(fragment_shader_base) + 1 );

	Compute();
}


OpenGL43Renderer2D::~OpenGL43Renderer2D()
{
}


int OpenGL43Renderer2D::LoadVertexShader( const void* data, size_t size )
{
	m2DReady = false;
	return OpenGL43Renderer::LoadVertexShader( data, size );
}


int OpenGL43Renderer2D::LoadVertexShader( const std::string& file )
{
	m2DReady = false;
	return OpenGL43Renderer::LoadVertexShader( file );
}


int OpenGL43Renderer2D::LoadFragmentShader( const void* data, size_t size )
{
	m2DReady = false;
	return OpenGL43Renderer::LoadFragmentShader( data, size );
}


int OpenGL43Renderer2D::LoadFragmentShader( const std::string& file )
{
	m2DReady = false;
	return OpenGL43Renderer::LoadFragmentShader( file );
}


void OpenGL43Renderer2D::Compute()
{
	createPipeline();

	glGenVertexArrays( 1, &mVAO );
	glBindVertexArray( mVAO );

	glUseProgram( mShader );

	const uint32_t binding_proj = 0;
	const uint32_t binding_view = 1;

	glGenBuffers( 1, &mMatrixProjectionID );
	glGenBuffers( 1, &mMatrixViewID );
	glGenBuffers( 1, &mVBO );

	glBindBuffer( GL_UNIFORM_BUFFER, mMatrixProjectionID );
	glBufferData( GL_UNIFORM_BUFFER, sizeof(float) * 16, mMatrixProjection->data(), GL_DYNAMIC_DRAW );
	glBindBufferBase( GL_UNIFORM_BUFFER, binding_proj, mMatrixProjectionID );
	glBindBuffer( GL_UNIFORM_BUFFER, 0 );
	((OpenGL43Instance*)Instance::baseInstance())->AffectVRAM( sizeof(float) * 16 );

	glBindBuffer( GL_UNIFORM_BUFFER, mMatrixViewID );
	glBufferData( GL_UNIFORM_BUFFER, sizeof(float) * 16, mMatrixView->data(), GL_DYNAMIC_DRAW );
	glBindBufferBase( GL_UNIFORM_BUFFER, binding_view, mMatrixViewID );
	glBindBuffer( GL_UNIFORM_BUFFER, 0 );
	((OpenGL43Instance*)Instance::baseInstance())->AffectVRAM( sizeof(float) * 16 );

	glBindBuffer( GL_ARRAY_BUFFER, mVBO );
	glBufferData( GL_ARRAY_BUFFER, 6 * sizeof(Vertex), nullptr, GL_STATIC_DRAW );
	((OpenGL43Instance*)Instance::baseInstance())->AffectVRAM( sizeof(Vertex) * 6 );

	glEnableVertexAttribArray( 0 );
	glVertexAttribPointer( 0, 4, GL_FLOAT, GL_FALSE, sizeof( Vertex ), (void*)( 0 ) );
	glEnableVertexAttribArray( 1 );
	glVertexAttribPointer( 1, 4, GL_FLOAT, GL_FALSE, sizeof( Vertex ), (void*)( sizeof( float ) * 4 ) );
	glEnableVertexAttribArray( 2 );
	glVertexAttribPointer( 2, 4, GL_FLOAT, GL_FALSE, sizeof( Vertex ), (void*)( sizeof( float ) * 4 + sizeof( float ) * 4 ) );
	glEnableVertexAttribArray( 3 );
	glVertexAttribPointer( 3, 4, GL_FLOAT, GL_FALSE, sizeof( Vertex ), (void*)( sizeof( float ) * 4 + sizeof( float ) * 4 + sizeof( float ) * 4 ) );

	glBindVertexArray( 0 );
	glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 );
	glBindBuffer( GL_ARRAY_BUFFER, 0 );
	m2DReady = true;
}


void OpenGL43Renderer2D::Draw( int x, int y, Image* image )
{
	if ( !m2DReady ) {
		Compute();
	}
	if ( mAssociatedWindow != nullptr && ( mAssociatedWindow->width() != mWidth || mAssociatedWindow->height() != mHeight ) ) {
		mWidth = mAssociatedWindow->width();
		mHeight = mAssociatedWindow->height();
		mMatrixProjection->Orthogonal( 0.0, mWidth, mHeight, 0.0, -2049.0, 2049.0 );
	}

	Vertex vertices[6];
	memset( vertices, 0, sizeof(vertices) );
	vertices[0].u = 0.0f;
	vertices[0].v = 0.0f;
	vertices[0].x = x;
	vertices[0].y = y;

	vertices[1].u = 1.0f;
	vertices[1].v = 1.0f;
	vertices[1].x = x + image->width();
	vertices[1].y = y + image->height();

	vertices[2].u = 1.0f;
	vertices[2].v = 0.0f;
	vertices[2].x = x + image->width();
	vertices[2].y = y;

	vertices[3].u = 0.0f;
	vertices[3].v = 0.0f;
	vertices[3].x = x;
	vertices[3].y = y;

	vertices[4].u = 0.0f;
	vertices[4].v = 1.0f;
	vertices[4].x = x;
	vertices[4].y = y + image->height();

	vertices[5].u = 1.0f;
	vertices[5].v = 1.0f;
	vertices[5].x = x + image->width();
	vertices[5].y = y + image->height();

	vertices[0].color[0] = vertices[1].color[0] = vertices[2].color[0] = vertices[3].color[0] = vertices[4].color[0] = vertices[5].color[0] = 1.0f;
	vertices[0].color[1] = vertices[1].color[1] = vertices[2].color[1] = vertices[3].color[1] = vertices[4].color[1] = vertices[5].color[1] = 1.0f;
	vertices[0].color[2] = vertices[1].color[2] = vertices[2].color[2] = vertices[3].color[2] = vertices[4].color[2] = vertices[5].color[2] = 1.0f;
	vertices[0].color[3] = vertices[1].color[3] = vertices[2].color[3] = vertices[3].color[3] = vertices[4].color[3] = vertices[5].color[3] = 1.0f;

	glBindBuffer( GL_ARRAY_BUFFER, mVBO );
	glBufferSubData( GL_ARRAY_BUFFER, 0, 6 * sizeof(Vertex), vertices );

	const uint32_t binding_proj = 0;
	const uint32_t binding_view = 1;

	glUseProgram( mShader );
	glBindVertexArray( mVAO );

	glActiveTexture( 0 );
	glBindTexture( GL_TEXTURE_2D, image->serverReference( mInstance ) );

	glBindBufferBase( GL_UNIFORM_BUFFER, binding_proj, mMatrixProjectionID );
	glBindBufferBase( GL_UNIFORM_BUFFER, binding_view, mMatrixViewID );

	glBindBuffer( GL_UNIFORM_BUFFER, mMatrixProjectionID );
	glBufferSubData( GL_UNIFORM_BUFFER, 0, sizeof(float) * 16, mMatrixProjection->data() );

	glBindBuffer( GL_UNIFORM_BUFFER, mMatrixViewID );
	glBufferSubData( GL_UNIFORM_BUFFER, 0, sizeof(float) * 16, mMatrixView->data() );


	glDrawArrays( GL_TRIANGLES, 0, 6 );


	glBindVertexArray( 0 );
	glUseProgram( 0 );
}
