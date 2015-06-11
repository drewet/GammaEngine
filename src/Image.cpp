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

#include "Image.h"
#include "Instance.h"
#include "File.h"
#include "Debug.h"

#include <algorithm>


namespace GE {

std::vector< ImageLoader* > Image::mImageLoaders = std::vector< ImageLoader* >();


Image::Image()
	: mWidth( 0 )
	, mHeight( 0 )
	, mData( nullptr )
{
}


Image::Image( const std::string filename, Instance* instance )
	: Image()
{
	if ( !instance ) {
		instance = Instance::baseInstance();
	}
	ImageLoader* loader = nullptr;
	File* file = new File( filename, File::READ );

	std::string extension = filename.substr( filename.rfind( "." ) + 1 );
	std::string first_line = file->ReadLine();
	std::transform( first_line.begin(), first_line.end(), first_line.begin(), ::tolower );
	file->Rewind();
	uint32_t file_magic = 0;
	file->Read( &file_magic, sizeof(file_magic) );
	file->Rewind();

	for ( size_t i = 0; i < mImageLoaders.size(); i++ ) {
		std::vector< std::string > patterns = mImageLoaders.at(i)->contentPatterns();
		for ( size_t j = 0; j < patterns.size(); j++ ) {
			std::string test_case = patterns[j];
			std::transform( test_case.begin(), test_case.end(), test_case.begin(), ::tolower );
			if ( first_line.find( test_case ) ) {
				loader = mImageLoaders.at(i);
			}
		}
	}

	if ( !loader ) {
		for ( size_t i = 0; i < mImageLoaders.size(); i++ ) {
			std::vector< std::string > extensions = mImageLoaders.at(i)->extensions();
			for ( size_t j = 0; j < extensions.size(); j++ ) {
				std::string test_case = extensions[j];
				std::transform( test_case.begin(), test_case.end(), test_case.begin(), ::tolower );
				if ( extension.find( test_case ) ) {
					loader = mImageLoaders.at(i);
					break;
				}
			}
		}
	}

	if ( loader ) {
		loader = loader->NewInstance();
		loader->Load( instance, file );
		*this = static_cast< Image >( *loader );
		delete loader;
		gDebug() << "Image loaded, size = " << mWidth << " x " << mHeight << "\n";
	}

	delete file;
}


Image::~Image()
{
}


uint32_t Image::width()
{
	return mWidth;
}


uint32_t Image::height()
{
	return mHeight;
}


uint32_t* Image::data()
{
	return mData;
}


ImageLoader* Image::AddImageLoader( ImageLoader* loader )
{
	mImageLoaders.insert( mImageLoaders.begin(), loader );
	return loader;
}

} // namespace GE
