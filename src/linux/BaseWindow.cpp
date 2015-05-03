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

#include <X11/X.h>
#include <X11/Xlib.h>
#include <X11/Xatom.h>
#include <X11/extensions/xf86vmode.h>
#include <GL/glx.h>

#include "linux/BaseWindow.h"
#include "Window.h"
#include "Instance.h"
#include "Time.h"
#include "Input.h"
#include "gememory.h"

namespace GE {


BaseWindow::BaseWindow( Instance* instance, int devid, const std::string& title, int width, int height, uint32_t _flags )
	: mInstance( instance )
	, mDevId( devid )
	, mWidth( width )
	, mHeight( height )
	, mWindow( 0 )
	, mKeys{ false }
	, mDisplay( 0 )
{
	Window::Flags flags = static_cast<Window::Flags>( _flags );

	mDisplay = XOpenDisplay(0);
	mScreen = DefaultScreen( mDisplay );

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

	mColorMap = XCreateColormap( mDisplay, RootWindow( mDisplay, mScreen ), mVisualInfo->visual, AllocNone );
	mWindowAttributes = (XSetWindowAttributes*)geMalloc( sizeof(XSetWindowAttributes) );
	mWindowAttributes->colormap = mColorMap;
	mWindowAttributes->border_pixel = 0;
	mWindowAttributes->background_pixmap = None;

	XWindowAttributes attribs;
	XGetWindowAttributes( mDisplay, RootWindow( mDisplay, mScreen ), &attribs );
	if ( mWidth < 0 ) {
		mWidth = attribs.width;
	}
	if ( mHeight < 0 ) {
		mHeight = attribs.height;
	}

	XSizeHints* win_size_hints = XAllocSizeHints();
	win_size_hints->flags = PSize;
	if ( !( flags & Window::Resizable ) ) {
		win_size_hints->flags = PSize | PMinSize | PMaxSize;
		win_size_hints->min_width = mWidth;
		win_size_hints->min_height = mHeight;
		win_size_hints->max_width = mWidth;
		win_size_hints->max_height = mHeight;
		win_size_hints->base_width = mWidth;
		win_size_hints->base_height = mHeight;
	}

	mWindowAttributes->override_redirect = false;
	mWindowAttributes->event_mask = ExposureMask | PointerMotionMask | KeyPressMask | KeyReleaseMask | ButtonPressMask | ButtonReleaseMask | StructureNotifyMask;
	mWindow = XCreateWindow( mDisplay, RootWindow( mDisplay, mVisualInfo->screen ), 0, 0, mWidth, mHeight, 0, mVisualInfo->depth, InputOutput, mVisualInfo->visual, CWBorderPixel | CWBackPixmap | CWColormap | CWEventMask | CWOverrideRedirect, mWindowAttributes );

	XSetStandardProperties( mDisplay, mWindow, title.c_str(), title.c_str(), None, nullptr, 0, nullptr );
	XMapRaised( mDisplay, mWindow );

	if ( flags & Window::Fullscreen ) {
		Atom wm_fullscreen = XInternAtom( mDisplay, "_NET_WM_STATE_FULLSCREEN", true );
		XChangeProperty( mDisplay, mWindow, XInternAtom( mDisplay, "_NET_WM_STATE", true), XA_ATOM, 32, PropModeReplace, (unsigned char *)&wm_fullscreen, 1 );
	}


	Atom wmDelete = XInternAtom( mDisplay, "WM_DELETE_WINDOW", true );
	XSetWMProtocols( mDisplay, mWindow, &wmDelete, 1 );

	XSetWMNormalHints( mDisplay, mWindow, win_size_hints );
	XSetWMHints( mDisplay, mWindow, XAllocWMHints() );
	XSelectInput( mDisplay, mWindow, mWindowAttributes->event_mask );

/*
	XColor black;
	static char bm_no_data[] = { 0 };
	Pixmap bm_no = XCreateBitmapFromData( mDisplay, mWindow, bm_no_data, 8, 8 );
	invisible_cursor = XCreatePixmapCursor( mDisplay, bm_no, bm_no, &black, &black, 0, 0 );
	if (bm_no!=None)XFreePixmap( mDisplay, bm_no);
*/

	VK_CONNECTION_INFO connectionInfo;
	connectionInfo.dpy = mDisplay;
	connectionInfo.screen = mScreen;
	connectionInfo.window = mWindow;
	vkWsiX11AssociateConnection( instance->gpu( mDevId ), &connectionInfo );

// 	mEventThread = new std::thread( &BaseWindow::pEventThread, this );
}


BaseWindow::~BaseWindow()
{
}


void BaseWindow::SwapBuffersBase()
{
	pEventThread();
// 	glXSwapBuffers( mDisplay, mWindow );
}


void BaseWindow::pEventThread()
{
	uint32_t ticks = Time::GetTick();
	bool finished = false;
	int key = 0;
	XEvent event;

// 	while ( 1 )
	{
		while ( XPending( mDisplay ) ) {
			XNextEvent( mDisplay, &event );
// 			if(event.type)printf("event: %d\n", event.type);
			if ( ( event.type == ButtonPress || event.type == ButtonRelease ) && ( event.xbutton.button == Input::MWHEELUP || event.xbutton.button == Input::MWHEELDOWN ) ) {
				// TODO
			}
			switch ( event.type ) {
				case ClientMessage:
// 					if ( XGetAtomName( mDisplay, event.xclient.message_type) == "WM_PROTOCOLS" ) {
// 						finished = true;
// 					}
					if ( (Atom)event.xclient.data.l[0] == XInternAtom( mDisplay, "WM_DELETE_WINDOW", False ) ) {
						finished = true;
					}
					break;
				case ConfigureNotify:
					// TODO : inter-thread call "Resize"
					break;
				case KeymapNotify:
					XRefreshKeyboardMapping( &event.xmapping );
					break;
				case KeyPress:
					key = (int)XLookupKeysym(&event.xkey, 0);
					if ( key >= 'a' && key <= 'z' ) {
						key += ( 'A' - 'a' );
					}
					if ( key < 256 ) {
// 						printf( "'%c' pressed\n", key);
						mKeys[key] = true;
					} else {
						// TODO
						if ( mKeys[ Input::LALT ] && mKeys[ Input::F4 ] ) {
							finished = true;
						}
					}
					break;
				case KeyRelease:
					key = (int)XLookupKeysym(&event.xkey, 0);
					if ( key >= 'a' && key <= 'z' ) {
						key += ( 'A' - 'a' );
					}
					if ( key < 256 ) {
// 						printf( "'%c' released\n", key);
						mKeys[key] = false;
					} else {
						// TODO
					}
					break;
				case ButtonPress:
					
					if ( event.xbutton.button == 4 ) { // wheel up
						// TODO
					} else if ( event.xbutton.button == 5 ) { // wheel down
						// TODO
					} else {
						mKeys[ Input::LBUTTON + event.xbutton.button - 1 ] = true;
					}
					break;
				case ButtonRelease:
					if ( event.xbutton.button == 4 ) { // wheel up
						// TODO
					} else if ( event.xbutton.button == 5 ) { // wheel down
						// TODO
					} else {
						mKeys[ Input::LBUTTON + event.xbutton.button - 1 ] = false;
					}
					break;
				default:
					break;
			}
		}
		if ( finished ) {
			mClosing = true;
			finished = false;
			return;
		}
// 		ticks = Time::WaitTick( 1000 / 120, ticks );
	}
}


} // namespace GE 
