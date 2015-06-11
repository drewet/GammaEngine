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

#ifndef IMAGE_H
#define IMAGE_H

#include <string>
#include <vector>
#include <map>

#include <vulkan.h>

namespace GE {

class Instance;
class ImageLoader;
class File;

class Image
{
public:
	Image();
	Image( const std::string filename, Instance* instance = nullptr );
	~Image();

	uint32_t width();
	uint32_t height();
	uint32_t* data();

	static ImageLoader* AddImageLoader( ImageLoader* loader );

protected:
	uint32_t mWidth;
	uint32_t mHeight;
	uint32_t* mData;

	std::map< std::pair< Instance*, int >, VK_MEMORY_REF > mVkRefs;

	static std::vector< ImageLoader* > mImageLoaders;
};


class ImageLoader : public Image
{
public:
	ImageLoader() : Image() { ; }
	virtual ~ImageLoader() { ; }
	virtual std::vector< std::string > contentPatterns() = 0;
	virtual std::vector< std::string > extensions() = 0;
	virtual ImageLoader* NewInstance() = 0;
	virtual void Load( Instance* instance, File* file, uint32_t pref_w = 0, uint32_t pref_h = 0 ) = 0;
};


} // namespace GE

#endif // IMAGE_H
