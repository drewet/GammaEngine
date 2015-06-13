/*
 * The GammaEngine Library 2.0 is a multiplatform Vulkan-based game engine
 * Copyright (C) 2015  Adrien Aubry <dridri85@gmail.com>
 *
 * This provkam is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This provkam is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this provkam.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#define BASEWINDOW_CPP

#include <GL/glx.h>
#include <X11/Xlib.h>

#include "OpenGL43Window.h"
#include "OpenGL43Instance.h"
#include "Debug.h"

extern "C" GE::Window* CreateWindow( GE::Instance* instance, const std::string& title, int width, int height, OpenGL43Window::Flags flags ) {
	return new OpenGL43Window( instance, title, width, height, flags );
}

OpenGL43Window::OpenGL43Window( Instance* instance, const std::string& title, int width, int height, Flags flags )
	: Window( instance, title, width, height, flags )
	, mClearColor( 0 )
{

#ifdef GE_WIN32
#else
	const int attributes[] = {
		GLX_RGBA,
		GLX_DOUBLEBUFFER,
		GLX_RED_SIZE, 8,
		GLX_GREEN_SIZE, 8,
		GLX_BLUE_SIZE, 8,
		GLX_ALPHA_SIZE, 8,
		GLX_DEPTH_SIZE, 16,
		None
	};
	mVisualInfo = glXChooseVisual( mDisplay, mScreen, (int*)attributes );
	mGLContext = glXCreateContext( mDisplay, (XVisualInfo*)mVisualInfo, 0, true );
	glXMakeCurrent( mDisplay, mWindow, static_cast<GLXContext>(mGLContext) );
#endif

	glViewport( 0, 0, mWidth, mHeight );
	glEnable( GL_CULL_FACE );
	glFrontFace( GL_CCW );
	glCullFace( GL_BACK );
	glEnable( GL_DEPTH_TEST );
	glEnable( GL_ALPHA_TEST );
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glPointSize( 4.0f );

	((OpenGL43Instance*)Instance::baseInstance())->AffectVRAM( sizeof(uint32_t) * mWidth * mHeight ); // Front
	((OpenGL43Instance*)Instance::baseInstance())->AffectVRAM( sizeof(uint32_t) * mWidth * mHeight ); // Back
	((OpenGL43Instance*)Instance::baseInstance())->AffectVRAM( sizeof(uint32_t) * mWidth * mHeight / 2 ); // Depth
}


OpenGL43Window::~OpenGL43Window()
{
}


uint64_t OpenGL43Window::colorImage()
{
	return 0;
}


void OpenGL43Window::Clear( uint32_t color )
{
	float clearColor[] = {
		(float)( ( color >>  0 ) & 0xFF ) / 255.0f,
		(float)( ( color >>  8 ) & 0xFF ) / 255.0f,
		(float)( ( color >> 16 ) & 0xFF ) / 255.0f,
		(float)( ( color >> 24 ) & 0xFF ) / 255.0f
	};
	glClearColor( clearColor[0], clearColor[1], clearColor[2], clearColor[3] );
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
}


void OpenGL43Window::BindTarget()
{
// 	glBindFramebuffer( GL_FRAMEBUFFER, 0 );
}


void OpenGL43Window::SwapBuffers()
{
	glXSwapBuffers( mDisplay, mWindow );
	SwapBuffersBase();

	if ( mHasResized ) {
		glViewport( 0, 0, mWidth, mHeight );
	}
}


uint64_t OpenGL43Window::CreateSharedContext()
{
	return (uint64_t)glXCreateContext( mDisplay, (XVisualInfo*)mVisualInfo, (GLXContext)mGLContext, true );
}


void OpenGL43Window::BindSharedContext( uint64_t ctx )
{
	int ret = glXMakeCurrent( mDisplay, mWindow, static_cast<GLXContext>((void*)ctx) );
// 	int ret = glXMakeCurrent( mDisplay, 0, static_cast<GLXContext>((void*)ctx) );
	gDebug() << "glXMakeCurrent ret = " << ret << "\n";
	gDebug() << "OGL : " << glGetString( GL_VERSION ) << "\n";
// 	exit(0);
// 	glXMakeCurrent( mDisplay, 0, static_cast<GLXContext>((void*)ctx) );
}


void OpenGL43Window::ReadKeys( bool* keys )
{
	memcpy( keys, mKeys, sizeof( mKeys ) );
}
