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

#ifndef BASEWINDOW_H
#define BASEWINDOW_H

#include <string>
#include <thread>
#include "Vector.h"

namespace GE {

class Instance;

#ifdef BASEWINDOW_CPP

	#include <android/sensor.h>
	#include <android/log.h>
	#include <android/native_activity.h>
	#include <android/native_window_jni.h>
	#include <android/window.h>
	#include <jni.h>
	#include <android_native_app_glue.h>

	typedef struct Engine {
		JNIEnv* env;
		jobject aJavaSurface;
		struct android_app* app;
		ANativeWindow* aSurface;
		int ofsx;
		int ofsy;
		int fullscreen;

		ASensorManager* sensorManager;
		const ASensor* accelerometerSensor;
		ASensorEventQueue* sensorEventQueue;

		EGLDisplay display;
		EGLSurface surface;
		EGLContext context;
		EGLConfig config;
		EGLint format;
		int32_t width;
		int32_t height;
	} Engine;

	typedef struct ATouch {
		int used;
		int id;
		int action;
		float x, y;
		float force;
	} ATouch;

#else // BASEWINDOW_CPP

	#define Engine void
	struct android_app;
	#define AInputEvent void

#endif // BASEWINDOW_CPP

class BaseWindow
{
public:
	BaseWindow( Instance* instance, const std::string& title, int width, int height, uint32_t flags );
	~BaseWindow();

	uint32_t width();
	uint32_t height();
	Vector2i& cursor();
	Vector2i& cursorWarp();

	void SwapBuffersBase();
	float fps() const;

	static void AndroidInit( struct android_app* state );

protected:
	Instance* mInstance;
	uint32_t mWidth;
	uint32_t mHeight;
	bool mHasResized;
	uint64_t mWindow;
	std::thread* mEventThread;
	bool mKeys[512];
	Vector2i mCursor;
	Vector2i mCursorWarp;

	float mFps;
	int mFpsImages;
	uint64_t mFpsTimer;

protected://private:
	bool mClosing;

private:
	static void PollEvents();
	static void engine_handle_cmd( struct android_app* app, int32_t cmd );
	static int32_t engine_handle_input( struct android_app* app, AInputEvent* event );
	static struct android_app* mState;
	static Engine* mEngine;
};


} // namespace GE

#endif // BASEWINDOW_H
 