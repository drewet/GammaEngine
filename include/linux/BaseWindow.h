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
#include <vulkan.h>

#ifndef BASEWINDOW_CPP
#ifndef Display
// typedef struct _XDisplay Display;
#define Display void
#endif
#ifndef XSetWindowAttributes
// typedef struct XSetWindowAttributes XSetWindowAttributes;
#define XSetWindowAttributes void
#endif
#ifndef XVisualInfo
// typedef struct XVisualInfo XVisualInfo;
#define XVisualInfo void
#endif
#endif

namespace GE {

class Instance;

class BaseWindow
{
public:
	BaseWindow( Instance* instance, int devid, const std::string& title, int width, int height, uint32_t flags );
	~BaseWindow();

	void SwapBuffersBase();

protected:
	Instance* mInstance;
	int mDevId;
	int mWidth;
	int mHeight;
	uint64_t mWindow;

private:
	Display* mDisplay;
	int mScreen;
	XVisualInfo* mVisualInfo;
	uint64_t mColorMap;
	XSetWindowAttributes* mWindowAttributes;
};


} // namespace GE

#endif // BASEWINDOW_H
 