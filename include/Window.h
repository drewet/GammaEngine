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

#ifndef WINDOW_H
#define WINDOW_H

#include <string>

#include "linux/BaseWindow.h"

namespace GE {

class Instance;

class Window : public BaseWindow
{
public:
	typedef enum {
		Nil = 0,
		Resizable,
		Fullscreen,
	} Flags;
	Window( Instance* instance, int devid, const std::string& title, int width, int height, Flags flags = Nil );
	~Window();

	void Clear( uint32_t color = 0xFF000000 );
	void BindTarget();
	void SwapBuffers();

	VK_IMAGE colorImage();

private:
	void InitPresentableImage();

	VK_IMAGE mColorImage;
	VK_MEMORY_REF mColorImageMemRef;
	VK_IMAGE_SUBRESOURCE_RANGE mImageColorRange;
	VK_COLOR_TARGET_VIEW mColorTargetView;

	VK_IMAGE mDepthImage;
	VK_MEMORY_REF mDepthImageMemRef;
	VK_IMAGE_SUBRESOURCE_RANGE mImageDepthRange;
	VK_DEPTH_STENCIL_VIEW mDepthTargetView;

	VK_CMD_BUFFER mBindCmdBuffer;
	VK_CMD_BUFFER mClearCmdBuffer;

	uint32_t mClearColor;
};


} // namespace GE

#endif // WINDOW_H
 