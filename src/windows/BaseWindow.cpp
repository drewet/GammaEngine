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

#include "BaseWindow.h"
#include "Window.h"
#include "Instance.h"
#include "Time.h"
#include "Input.h"
#include "gememory.h"

#include <windows.h>
#undef CreateWindow

namespace GE {

static LRESULT CALLBACK sWindowProcedure( HWND window, UINT message, WPARAM wParam, LPARAM lParam );

BaseWindow::BaseWindow( Instance* instance, const std::string& title, int width, int height, uint32_t _flags )
	: mInstance( instance )
	, mWidth( width )
	, mHeight( height )
	, mHasResized( false )
	, mWindow( 0 )
	, mKeys{ false }
	, mInitializing( true )
{
	Window::Flags flags = static_cast<Window::Flags>( _flags );
	hInstance = (uint64_t)GetModuleHandle( nullptr );

	WNDCLASS winClass;
	RECT WindowRect;
	bool fullscreen = false;
	bool resizable = false;
	int nSamples = 1;
	if ( flags & Window::Fullscreen ) {
		fullscreen = true;
	}
	if ( flags & Window::Resizable ) {
		resizable = true;
	}

	int fsWidth = GetSystemMetrics( SM_CXSCREEN );
	int fsHeight = GetSystemMetrics( SM_CYSCREEN );

	if ( width < 0 ) {
		mWidth = fsWidth;
	}
	if ( height < 0 ) {
		mHeight = fsHeight;
	}
	WindowRect.left = 0;
	WindowRect.top = 0;
	WindowRect.right = mWidth;
	WindowRect.bottom = mHeight;

	memset( &winClass, 0x0, sizeof(winClass) );
	winClass.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
    winClass.lpfnWndProc = sWindowProcedure;
    winClass.cbClsExtra = 0;
    winClass.cbWndExtra = 0;
    winClass.hInstance = (HINSTANCE)hInstance;
	winClass.hIcon = LoadIcon( nullptr, IDI_APPLICATION );
	winClass.hCursor = LoadCursor( nullptr, IDC_ARROW );
	winClass.hbrBackground = (HBRUSH) GetStockObject( BLACK_BRUSH );
	winClass.lpszMenuName = nullptr;
    winClass.lpszClassName = "GammaEngine";
	RegisterClass( &winClass );

	int dwExStyle = 0;
	int dwStyle = 0;
/*
	if ( fullscreen ) {
		fsmode = true;
		DEVMODE dmScreenSettings;
		memset(&dmScreenSettings,0,sizeof(dmScreenSettings));
		dmScreenSettings.dmSize=sizeof(dmScreenSettings);
		dmScreenSettings.dmPelsWidth	= width;
		dmScreenSettings.dmPelsHeight	= height;
		dmScreenSettings.dmBitsPerPel	= 32;
		dmScreenSettings.dmFields=DM_BITSPERPEL|DM_PELSWIDTH|DM_PELSHEIGHT;
		ChangeDisplaySettings(&dmScreenSettings, CDS_FULLSCREEN);
	}
*/

	if ( fullscreen ) {
		dwExStyle = WS_EX_APPWINDOW;
		dwStyle = WS_POPUP;
	} else {
		dwExStyle = WS_EX_APPWINDOW | WS_EX_WINDOWEDGE;
		if ( resizable ) {
			dwStyle = ( WS_OVERLAPPEDWINDOW - WS_MAXIMIZEBOX - WS_THICKFRAME ) | WS_THICKFRAME | WS_MAXIMIZEBOX | WS_MINIMIZEBOX | WS_SYSMENU;
		} else {
			dwStyle = ( WS_OVERLAPPEDWINDOW - WS_MAXIMIZEBOX - WS_THICKFRAME ) | WS_BORDER | WS_MINIMIZEBOX | WS_SYSMENU;
		}
	}

	AdjustWindowRectEx( &WindowRect, dwStyle, FALSE, dwExStyle );

	mWindow = (uint64_t)CreateWindowEx( dwExStyle, "GammaEngine", title.c_str(), dwStyle|WS_CLIPSIBLINGS|WS_CLIPCHILDREN, CW_USEDEFAULT, CW_USEDEFAULT, WindowRect.right-WindowRect.left, WindowRect.bottom-WindowRect.top, NULL, NULL, (HINSTANCE)hInstance, NULL);
	
	static	PIXELFORMATDESCRIPTOR pfd;
	ZeroMemory(&pfd, sizeof (pfd));
	pfd.nSize = sizeof(pfd);
	pfd.nVersion = 2;
	pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER | PFD_STEREO;
	pfd.iPixelType = PFD_TYPE_RGBA;
	pfd.cColorBits = 24;
	pfd.cAlphaBits = 8;
	pfd.cDepthBits = 16;
	pfd.iLayerType = PFD_MAIN_PLANE;
	int fmt = ChoosePixelFormat( GetDC( (HWND)mWindow ), &pfd );
	SetPixelFormat( GetDC( (HWND)mWindow ), fmt, &pfd );
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


void BaseWindow::SwapBuffersBase()
{
	MSG message;
	while ( PeekMessage( &message, NULL, 0, 0, PM_REMOVE ) ) {
		TranslateMessage( &message );
		DispatchMessage( &message );
	}
}


static LRESULT CALLBACK sWindowProcedure( HWND window, UINT message, WPARAM wParam, LPARAM lParam )
{
/*
	if(message == WM_SETFOCUS){
		has_focus = true;
		ShowCursor(cursor_visible);
	}
	if(message == WM_KILLFOCUS){
		has_focus = false;
		ShowCursor(true);
	}
*/
/*
	if ( message == WM_DESTROY && !mInitializing ) {
		PostQuitMessage(0);
		exit(0);
	}
*/
/*
	if ( !mInitializing && message == WM_SIZE ) {
		if ( lParam != 0 ) {
				mWidth = (lParam & 0x0000FFFF);
				mHeight = ((lParam & 0xFFFF0000) >> 16);
		}
	}
*/
//	if ( !mInitializing && has_focus )
	{
		switch ( message ) {
			case WM_CREATE:
				break;

			case WM_COMMAND:
				break;
/*
			case WM_CHAR:
				last_key = wParam;
				break;
*/
	/*
			case WM_KEYDOWN:
				last_key = wParam;
				break;

			case WM_KEYUP:
				last_key = 0;
				break;
	*/
/*
			case WM_LBUTTONDOWN:
				keys_pressed[GEK_LBUTTON] = true;
				break;

			case WM_LBUTTONUP:
				keys_pressed[GEK_LBUTTON] = false;
				break;

			case WM_RBUTTONDOWN:
				keys_pressed[GEK_RBUTTON] = true;
				break;

			case WM_RBUTTONUP:
				keys_pressed[GEK_RBUTTON] = false;
				break;

			case WM_MBUTTONDOWN:
				keys_pressed[GEK_MBUTTON] = true;
				break;

			case WM_MBUTTONUP:
				keys_pressed[GEK_MBUTTON] = false;
				break;
			
			case WM_MOUSEWHEEL:
				if(((short)((wParam & 0xFFFF0000) >> 16)) > 0){
					keys_pressed[GEK_MWHEELUP] = true;
				}
				if(((short)((wParam & 0xFFFF0000) >> 16)) < 0){
					keys_pressed[GEK_MWHEELDOWN] = true;
				}
				break;
*/
			case WM_MOUSEMOVE:
				break;

			case 0x00FF: //WM_INPUT
				break;
		}
	}

	return DefWindowProc( window, message, wParam, lParam );
}

} // namespace GE 
