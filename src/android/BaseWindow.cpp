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

#define BASEWINDOW_CPP

#include <stdlib.h>
#include <string.h>
#include "android/BaseWindow.h"
#include "Window.h"
#include "Instance.h"
#include "Time.h"
#include "Input.h"
#include "Debug.h"

int main( int, char** );

namespace GE {

#define LOGI(...) ((void)__android_log_print(ANDROID_LOG_INFO, "GE", __VA_ARGS__))
#define LOGW(...) ((void)__android_log_print(ANDROID_LOG_WARN, "GE", __VA_ARGS__))

Engine* BaseWindow::mEngine = nullptr;
struct android_app* BaseWindow::mState = nullptr;

void android_main( struct android_app* state )
{
	LOGW("\n\n\n\n");
	LOGW("===================== GammaEngine for Android =====================\n");

	BaseWindow::AndroidInit( state );
}


void BaseWindow::AndroidInit( struct android_app* state )
{
	mState = state;
	mEngine = new Engine;
	memset( mEngine, 0, sizeof( Engine ) );
	state->userData = mEngine;
	state->onAppCmd = &BaseWindow::engine_handle_cmd;
	state->onInputEvent = &BaseWindow::engine_handle_input;
	mEngine->app = state;
	mEngine->aSurface = mEngine->app->window;
	mEngine->gotFocus = true;

	while ( !state->window ) {
		PollEvents();
	}

	main( 0, nullptr );
}


BaseWindow::BaseWindow( Instance* instance, const std::string& title, int width, int height, uint32_t _flags )
	: mInstance( instance )
	, mWidth( width )
	, mHeight( height )
	, mHasResized( false )
	, mWindow( 0 )
	, mKeys{ false }
{
	Window::Flags flags = static_cast<Window::Flags>( _flags );

	mEngine->aflags = AWINDOW_FLAG_KEEP_SCREEN_ON;
	if ( flags & Window::Fullscreen ) {
		mEngine->aflags |= AWINDOW_FLAG_FULLSCREEN;
		mEngine->aSurface = 0;
	}

	ANativeActivity_setWindowFlags( mState->activity, mEngine->aflags, 0 );

	while ( mEngine->hasSurface && !mEngine->aSurface ) {
		Time::Sleep( 10 );
	}

	const EGLint attribs[] = {
			EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT,
			EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
			EGL_BLUE_SIZE, 8,
			EGL_GREEN_SIZE, 8,
			EGL_RED_SIZE, 8,
			EGL_ALPHA_SIZE, 8,
			EGL_DEPTH_SIZE, 16,
			EGL_NONE
	};
	const EGLint context_attribs[] = {
		EGL_CONTEXT_CLIENT_VERSION, 2,
		EGL_NONE
	};

	EGLint w, h, dummy, format;
	EGLint numConfigs;
	EGLConfig config;
	EGLSurface surface;
	EGLContext context;

	EGLDisplay display = eglGetDisplay( EGL_DEFAULT_DISPLAY );
	eglInitialize( display, 0, 0 );
	eglChooseConfig( display, attribs, &config, 1, &numConfigs );
	eglGetConfigAttrib( display, config, EGL_NATIVE_VISUAL_ID, &format );

	int base_width = 0;
	int base_height = 0;
	if ( mEngine->app && mEngine->aSurface ) {
		base_width = ANativeWindow_getWidth( mEngine->aSurface );
		base_height = ANativeWindow_getHeight( mEngine->aSurface );
	}
	LOGI( "base size: %d x %d", base_width, base_height );

	mWidth = width < 0 ? base_width : width;
	mHeight = height < 0 ? base_height : height;
	LOGI( "new size: %d x %d", width, height );

	ANativeWindow_setBuffersGeometry( mEngine->aSurface, width, height, WINDOW_FORMAT_RGBA_8888/*format*/ );
	surface = eglCreateWindowSurface( display, config, mEngine->aSurface, nullptr );

	if ( mEngine->context == EGL_NO_CONTEXT ) {
		context = eglCreateContext( display, config, nullptr, context_attribs );
	} else {
		context = mEngine->context;
	}
	if ( eglMakeCurrent( display, surface, surface, context ) == EGL_FALSE ) {
		LOGW( "Unable to eglMakeCurrent" );
	}

	eglQuerySurface( display, surface, EGL_WIDTH, &w );
	eglQuerySurface( display, surface, EGL_HEIGHT, &h );

	mWidth = w;
	mHeight = h;
	LOGI("final size: %d x %d", w, h);

	mEngine->format = format;
	mEngine->config = config;
	mEngine->display = display;
	mEngine->context = context;
	mEngine->surface = surface;
	mEngine->width = w;
	mEngine->height = h;
	mEngine->mainWindowCreated = true;
}


BaseWindow::~BaseWindow()
{
}


uint32_t BaseWindow::width()
{
	return mWidth;
}


uint32_t BaseWindow::height()
{
	return mHeight;
}


Vector2i& BaseWindow::cursor()
{
	return mCursor;
}


Vector2i& BaseWindow::cursorWarp()
{
	return mCursorWarp;
}


float BaseWindow::fps() const
{
	return mFps;
}


void BaseWindow::SwapBuffersBase()
{
	mFpsImages++;
	if ( Time::GetTick() - mFpsTimer > 500 ) {
		mFps = mFpsImages * 1000.0f / ( Time::GetTick() - mFpsTimer );
		mFpsTimer = Time::GetTick();
		mFpsImages = 0;
	}

	mCursor.x = 0; // TODO
	mCursor.y = 0; // TODO
	mCursorWarp.x = 0;
	mCursorWarp.y = 0;

	PollEvents();
	while( mEngine->surface == EGL_NO_SURFACE ) {
		Time::Sleep(100);
		PollEvents();
	}

	eglSwapBuffers( mEngine->display, mEngine->surface );
}


void BaseWindow::engine_handle_cmd( struct android_app* app, int32_t cmd )
{
	int w, h;
	switch ( cmd ) {
		case APP_CMD_SAVE_STATE:
			break;
		case APP_CMD_INIT_WINDOW:
			if ( mEngine->app->window != nullptr && mEngine->mainWindowCreated ) {
				while ( mEngine->hasSurface && !mEngine->aSurface ) {
					Time::Sleep(10);
				}

				eglMakeCurrent( mEngine->display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT );
				ANativeActivity_setWindowFlags( mState->activity, mEngine->aflags, 0 );
				eglDestroySurface( mEngine->display, mEngine->surface );
				ANativeWindow_setBuffersGeometry( mEngine->aSurface, 0, 0, /*mEngine->format*/WINDOW_FORMAT_RGBA_8888 );
				mEngine->display = eglGetDisplay( EGL_DEFAULT_DISPLAY );
				mEngine->surface = eglCreateWindowSurface( mEngine->display, mEngine->config, mEngine->aSurface, nullptr );
				eglMakeCurrent( mEngine->display, mEngine->surface, mEngine->surface, mEngine->context );
				eglQuerySurface( mEngine->display, mEngine->surface, EGL_WIDTH, &w );
				eglQuerySurface( mEngine->display, mEngine->surface, EGL_HEIGHT, &h );
				mWidth = w;
				mHeight = h;
			}
			break;
		case APP_CMD_TERM_WINDOW:
			eglMakeCurrent( mEngine->display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT );
			if ( mEngine->surface != EGL_NO_SURFACE ) {
				eglDestroySurface(mEngine->display, mEngine->surface);
				mEngine->surface = EGL_NO_SURFACE;
			}

			if ( mEngine->hasSurface && mEngine->aSurface ) {
				ANativeWindow_release( mEngine->aSurface );
				mEngine->aSurface = 0;
			}

			break;
		case APP_CMD_GAINED_FOCUS:
// 			ge_ResumeAllThreads(); // TODO
			mEngine->gotFocus = true;
			break;
		case APP_CMD_LOST_FOCUS:
// 			ge_PauseAllThreads(); // TODO
			mEngine->gotFocus = false;
			break;
	}
}


int32_t BaseWindow::engine_handle_input( struct android_app* app, AInputEvent* event )
{
	return 0;
}


void BaseWindow::PollEvents()
{
	int ident;
	int events;
	struct android_poll_source* source;
	uint64_t pause_start = Time::GetTick();
	bool time_shift = false;

	do {
		while ( ( ident = ALooper_pollAll( 0, nullptr, &events, (void**)&source) ) >= 0 ) {
			if (source != nullptr) {
				source->process(mState, source);
			}
			if ( mState->destroyRequested != 0) {
// 				engine_term_display( mEngine );
				exit( 0 );
				return;
			}
		}
		if ( mEngine->gotFocus == false ) {
			Time::Sleep( 50 );
			time_shift = true;
		}
	} while ( mEngine->gotFocus == false );

	if ( time_shift ) {
// 		mTickPause += Time::GetTick() - pause_start;
// 		Time::IncreasePause( Time::GetTick() - pause_start );
	}
}


extern "C" JNIEXPORT void JNICALL Java_com_drich_libge_LibGE_setSurface( JNIEnv* env, jobject obj, jobject surface, jint ofsx, jint ofsy )
{
	fDebug( env, obj, surface, ofsx, ofsy );
	BaseWindow::androidEngine()->aJavaSurface = surface;
	BaseWindow::androidEngine()->ofsx = ofsx;
	BaseWindow::androidEngine()->ofsy = ofsy;
	BaseWindow::androidEngine()->aSurface = ANativeWindow_fromSurface( env, surface );
	gDebug() << " ===> " << BaseWindow::androidEngine()->aSurface;
}

extern "C" JNIEXPORT void JNICALL Java_com_drich_libge_LibGE_setHasSurface(JNIEnv* env, jobject obj, jint _hasSurface)
{
	while ( !BaseWindow::androidEngine() ) {
		Time::Sleep( 1 );
	}
	BaseWindow::androidEngine()->hasSurface = _hasSurface;
}

} // namespace GE 
