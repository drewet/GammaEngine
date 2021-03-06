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

#include "Instance.h"
#include "OpenGLES20Object.h"
#include "OpenGLES20Instance.h"
#include "OpenGLES20Renderer.h"
#include "OpenGLES20Window.h"
#include "Object.h"
#include "Debug.h"
#include "File.h"
#include "Camera.h"

extern "C" GE::Renderer* CreateRenderer( GE::Instance* instance ) {
	return new OpenGLES20Renderer( instance );
}

static const char* vertex_shader_include =
#if ( !defined( GE_ANDROID ) && !defined( GE_IOS ) )
	"#version 130\n"
#endif
#if ( defined( GE_ANDROID ) || defined( GE_IOS ) )
	"#define in attribute\n"
	"#define out varying\n"
#endif
	"precision highp float;\n"
	"#define geTexture2D(x) ge_Texture0\n"
	"#define geTexture3D(x) ge_Texture0\n"
	"\n"
	"in vec4 ge_VertexTexcoord;\n"
	"in vec4 ge_VertexColor;\n"
	"in vec4 ge_VertexNormal;\n"
	"in vec4 ge_VertexPosition;\n"
	"\n"
	"uniform mat4 ge_ObjectMatrix;\n"
	"uniform mat4 ge_ProjectionMatrix;\n"
	"uniform mat4 ge_ViewMatrix;\n"
	"uniform sampler2D ge_Texture0;\n"
	"\n"
	"#define ge_Position gl_Position\n"
;

static const char* fragment_shader_include =
#if ( !defined( GE_ANDROID ) && !defined( GE_IOS ) )
	"#version 130\n"
#endif
	"precision highp float;\n"
	"uniform sampler2D ge_Texture0;\n"
#if ( defined( GE_ANDROID ) || defined( GE_IOS ) )
	"#define in varying\n"
	"#define texture texture2D\n"
#endif
#if ( defined( GE_ANDROID ) || defined( GE_IOS ) )
	"#define ge_FragColor gl_FragColor\n"
#else
	"out vec4 ge_FragColor;\n"
#endif
;

bool OpenGLES20Renderer::s2DActive = false;

OpenGLES20Renderer::OpenGLES20Renderer( Instance* instance )
	: mReady( false )
	, mInstance( instance ? instance : Instance::baseInstance() )
	, mMatrixObjects( 0 )
	, mRenderMode( GL_TRIANGLES )
	, mShader( 0 )
	, mVertexShader( 0 )
	, mFragmentShader( 0 )
	, mMatrixProjectionID( 0 )
	, mMatrixViewID( 0 )
	, mMatrixObjectID( 0 )
{

	mMatrixProjection = new Matrix();
	mMatrixProjection->Perspective( 60.0f, 16.0f / 9.0f, 0.01f, 1000.0f );
	mMatrixView = new Matrix();
	mMatrixView->Identity();
}


OpenGLES20Renderer::~OpenGLES20Renderer()
{
}


Matrix* OpenGLES20Renderer::projectionMatrix()
{
	return mMatrixProjection;
}


int OpenGLES20Renderer::LoadVertexShader( const void* data, size_t size )
{
	mReady = false;
	if ( mVertexShader ) {
		glDeleteShader( mVertexShader );
	}

	const char* array[2] = { vertex_shader_include, (char*)data };
	mVertexShader = glCreateShader( GL_VERTEX_SHADER );
	glShaderSource( mVertexShader, 2, array, NULL );
// 	glShaderSource( mVertexShader, 1, (const char**)&data, NULL );
	glCompileShader( mVertexShader );
	char log[4096] = "";
	int logsize = 4096;
	glGetShaderInfoLog( mVertexShader, logsize, &logsize, log );
	gDebug() << "vertex compile : " << log << "\n";

	return 0;
}


int OpenGLES20Renderer::LoadFragmentShader( const void* data, size_t size )
{
	mReady = false;
	if ( mFragmentShader ) {
		glDeleteShader( mFragmentShader );
	}

	fDebug( data, size );

	const char* array[2] = { fragment_shader_include, (char*)data };
	mFragmentShader = glCreateShader( GL_FRAGMENT_SHADER );
	glShaderSource( mFragmentShader, 2, array, NULL );
// 	glShaderSource( mFragmentShader, 1, (const char**)&data, NULL );
	glCompileShader( mFragmentShader );
	char log[4096] = "";
	int logsize = 4096;
	glGetShaderInfoLog( mFragmentShader, logsize, &logsize, log );
	gDebug() << "fragment compile : " << log << "\n";

	return 0;
}


int OpenGLES20Renderer::LoadVertexShader( const std::string& file )
{
	mReady = false;

	size_t sz = 0;
	uint8_t* data = loadShader( file, &sz );
	int ret = LoadVertexShader( data, sz );
	mInstance->Free( data );

	return ret;
}


int OpenGLES20Renderer::LoadFragmentShader( const std::string& file )
{
	mReady = false;

	size_t sz = 0;
	uint8_t* data = loadShader( file, &sz );
	int ret = LoadFragmentShader( data, sz );
	mInstance->Free( data );

	return ret;
}


void OpenGLES20Renderer::setRenderMode( int mode )
{
	mRenderMode = mode;
}


void OpenGLES20Renderer::createPipeline()
{
	if ( mShader ) {
		glDeleteProgram( mShader );
	}
	mShader = glCreateProgram();
	glAttachShader( mShader, mVertexShader );
	glAttachShader( mShader, mFragmentShader );

	glBindAttribLocation( mShader, 0, "ge_VertexTexcoord" );
	glBindAttribLocation( mShader, 1, "ge_VertexColor" );
	glBindAttribLocation( mShader, 2, "ge_VertexNormal" );
	glBindAttribLocation( mShader, 3, "ge_VertexPosition" );

	glLinkProgram( mShader );

	mMatrixProjectionID = glGetUniformLocation( mShader, "ge_ProjectionMatrix" );
	mMatrixViewID = glGetUniformLocation( mShader, "ge_ViewMatrix" );
	mMatrixObjectID = glGetUniformLocation( mShader, "ge_ObjectMatrix" );

	mFloatTimeID = glGetUniformLocation( mShader, "ge_Time" );

	mReady = true;
}


void OpenGLES20Renderer::AddObject( Object* obj )
{
// 	mObjects.emplace_back( (Object*)obj );

	bool opaque = true;
	Vertex* vertices = obj->vertices();
	for ( uint32_t i = 0; i < obj->verticesCount(); i++ ) {
		if ( vertices[i].color[3] < 1.0 ) {
			opaque = false;
			break;
		}
	}

	if ( opaque ) {
		mObjects.insert( mObjects.begin(), obj );
	} else {
		mObjects.emplace_back( (Object*)obj );
	}
}


void OpenGLES20Renderer::AddLight( Light* light )
{
	mLights.emplace_back( light );
}


void OpenGLES20Renderer::Compute()
{
	if ( !mReady ) {
		createPipeline();
	}

	std::vector< Vertex > vertices;
	std::vector< uint32_t > indices;

	if ( mMatrixObjects ) {
		mInstance->Free( mMatrixObjects );
	}
	mMatrixObjects = (float*)mInstance->Malloc( sizeof(float) * 16 * mObjects.size() );

	for ( size_t i = 0; i < mObjects.size(); i++ ) {
		mObjects[i]->matrix()->setDataPointer( &mMatrixObjects[ 16 * i ] );
		indices.insert( indices.end(), mObjects[i]->indices(), &mObjects[i]->indices()[mObjects[i]->indicesCount()] );
		vertices.insert( vertices.end(), mObjects[i]->vertices(), &mObjects[i]->vertices()[mObjects[i]->verticesCount()] );
	}

	glGenBuffers( 1, &mIBO );
	glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, mIBO );
	glBufferData( GL_ELEMENT_ARRAY_BUFFER, sizeof(uint32_t) * indices.size(), indices.data(), GL_STATIC_DRAW );
	((OpenGLES20Instance*)Instance::baseInstance())->AffectVRAM( sizeof(uint32_t) * indices.size() );

	glGenBuffers( 1, &mVBO );
	glBindBuffer( GL_ARRAY_BUFFER, mVBO );
	glBufferData( GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW );
	((OpenGLES20Instance*)Instance::baseInstance())->AffectVRAM( sizeof(Vertex) * vertices.size() );

	glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 );
	glBindBuffer( GL_ARRAY_BUFFER, 0 );
}


void OpenGLES20Renderer::Draw()
{
/*
	if ( !mReady ) {
		Compute();
	}
*/
	if ( s2DActive ) {
		glEnable( GL_DEPTH_TEST );
		s2DActive = false;
	}

	glUseProgram( mShader );
	glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, mIBO );
	glBindBuffer( GL_ARRAY_BUFFER, mVBO );

	glUniform1f( mFloatTimeID, Time::GetSeconds() );
	glUniformMatrix4fv( mMatrixProjectionID, 1, GL_FALSE, mMatrixProjection->data() );
	glUniformMatrix4fv( mMatrixViewID, 1, GL_FALSE, mMatrixView->data() );


	glEnableVertexAttribArray( 0 );
	glVertexAttribPointer( 0, 4, GL_FLOAT, GL_FALSE, sizeof( Vertex ), (void*)( 0 ) );
	glEnableVertexAttribArray( 1 );
	glVertexAttribPointer( 1, 4, GL_FLOAT, GL_FALSE, sizeof( Vertex ), (void*)( sizeof( float ) * 4 ) );
	glEnableVertexAttribArray( 2 );
	glVertexAttribPointer( 2, 4, GL_FLOAT, GL_FALSE, sizeof( Vertex ), (void*)( sizeof( float ) * 4 + sizeof( float ) * 4 ) );
	glEnableVertexAttribArray( 3 );
	glVertexAttribPointer( 3, 4, GL_FLOAT, GL_FALSE, sizeof( Vertex ), (void*)( sizeof( float ) * 4 + sizeof( float ) * 4 + sizeof( float ) * 4 ) );

	uint32_t iIndices = 0;
	for ( size_t i = 0; i < mObjects.size(); i++ ) {
		glUniformMatrix4fv( mMatrixObjectID, 1, GL_FALSE, mObjects[i]->matrix()->data() );
		glDrawElements( mRenderMode, mObjects[i]->indicesCount(), GL_UNSIGNED_INT, (void*)(uint64_t)iIndices );
		iIndices += mObjects[i]->indicesCount();
	}

	glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 );
	glBindBuffer( GL_ARRAY_BUFFER, 0 );
	glUseProgram( 0 );

}


void OpenGLES20Renderer::Look( Camera* cam )
{
	memcpy( mMatrixView->data(), cam->data(), sizeof( float ) * 16 );
	// TODO / TBD : upload matrix to shader here
}


uintptr_t OpenGLES20Renderer::attributeID( const std::string& name )
{
	if ( !mReady ) {
		createPipeline();
	}
	return glGetAttribLocation( mShader, name.c_str() );
}


uintptr_t OpenGLES20Renderer::uniformID( const std::string& name )
{
	if ( !mReady ) {
		createPipeline();
	}
	return glGetUniformLocation( mShader, name.c_str() );
}


void OpenGLES20Renderer::uniformUpload( const uintptr_t id, const float f )
{
	glUseProgram( mShader );
	glUniform1f( id, f );
}


void OpenGLES20Renderer::uniformUpload( const uintptr_t id, const Vector2f& v )
{
	glUseProgram( mShader );
	glUniform2f( id, v.x, v.y );
}


void OpenGLES20Renderer::uniformUpload( const uintptr_t id, const Vector3f& v )
{
	glUseProgram( mShader );
	glUniform3f( id, v.x, v.y, v.z );
}


void OpenGLES20Renderer::uniformUpload( const uintptr_t id, const Vector4f& v )
{
	glUseProgram( mShader );
	glUniform4f( id, v.x, v.y, v.z, v.w );
}


void OpenGLES20Renderer::uniformUpload( const uintptr_t id, const Matrix& v )
{
	glUseProgram( mShader );
	glUniformMatrix4fv( id, 1, GL_FALSE, v.constData() );
}


uint8_t* OpenGLES20Renderer::loadShader( const std::string& filename, size_t* sz )
{
	File* file = new File( filename, File::READ );

	size_t size = file->Seek( 0, File::END );
	file->Rewind();

	uint8_t* data = (uint8_t*)mInstance->Malloc( size + 1 );

	file->Read( data, size );
	data[size] = 0;
	delete file;

	if(sz){
		*sz = size;
	}
	return data;
}
