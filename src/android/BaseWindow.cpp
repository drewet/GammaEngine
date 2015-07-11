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

#include <string.h>
#include "android/BaseWindow.h"
#include "Window.h"
#include "Instance.h"
#include "Time.h"
#include "Input.h"
#include "gememory.h"

namespace GE {

Engine* BaseWindow::mEngine = nullptr;
int main( int, char** );

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
	, mDisplay( 0 )
{
	Window::Flags flags = static_cast<Window::Flags>( _flags );
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
}


static void BaseWindow::engine_handle_cmd( struct android_app* app, int32_t cmd )
{
}


static int32_t BaseWindow::engine_handle_input( struct android_app* app, AInputEvent* event )
{
	return 0;
}


void BaseWindow::PollEvents()
{
	int ident;
	int events;
	struct android_poll_source* source;
	u32 pause_start = geGetTick();
	bool time_shift = false;

	do {
		while ( ( ident = ALooper_pollAll( 0, nullptr, &events, (void**)&source) ) >= 0 ) {
			if (source != NULL) {
				source->process(_ge_android_context->state, source);
			}
			if (_ge_android_context->state->destroyRequested != 0) {
				engine_term_display(&engine);
				geDebugTerm();
				exit(0);
				return;
			}
		}
		if ( gotfocus == false ) {
			geSleep(100);
			time_shift = true;
		}
	} while ( gotfocus == false );

	if ( time_shift ) {
		tick_pause += geGetTick() - pause_start;
	}
}

} // namespace GE 
