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
#include "ImageLoaderPng.h"
#include "ImageLoaderJpg.h"
#include "Instance.h"
#include "File.h"
#include "Debug.h"

#include <algorithm>


namespace GE {

std::vector< ImageLoader* > Image::mImageLoaders = std::vector< ImageLoader* >();
static bool ImageLoaderFirstCall = true;

Image::Image()
	: mAllocInstance( nullptr )
	, mWidth( 0 )
	, mHeight( 0 )
	, mData( nullptr )
{
}


Image::Image( File* file, const std::string& extension, Instance* instance )
	: Image()
{
	if ( !instance ) {
		instance = Instance::baseInstance();
	}
	mAllocInstance = instance;
	Load( file, extension, instance );
}


Image::Image( const std::string filename, Instance* instance )
	: Image()
{
	if ( !instance ) {
		instance = Instance::baseInstance();
	}
	File* file = new File( filename, File::READ );
	std::string extension = filename.substr( filename.rfind( "." ) + 1 );

	Load( file, extension, instance );

	delete file;
}


Image::Image( uint32_t width, uint32_t height, uint32_t backcolor, Instance* instance )
	: mAllocInstance( instance ? instance : Instance::baseInstance() )
	, mWidth( width )
	, mHeight( height )
	, mData( nullptr )
{
	mData = (uint32_t*)mAllocInstance->Malloc( sizeof(uint32_t) * mWidth * mHeight );
	for ( uint32_t i = 0; i < mWidth * mHeight; i++ ) {
		mData[ i ] = backcolor;
	}
}


Image::~Image()
{
	((int*)0)[0] = 0;
	Instance* instance = mAllocInstance ? mAllocInstance : Instance::baseInstance();
	decltype(mServerRefs)::iterator it;

	if ( mData ) {
		instance->Free( mData );
	}
	for ( it = mServerRefs.begin(); it != mServerRefs.end(); ++it ) {
		(*it).first->UnreferenceImage( (*it).second );
	}
}


void Image::Load( File* file, const std::string& extension, Instance* instance )
{
	ImageLoader* loader = nullptr;

	if ( ImageLoaderFirstCall ) {
		AddImageLoader( new ImageLoaderPng() );
		AddImageLoader( new ImageLoaderJpg() );
		ImageLoaderFirstCall = false;
	}

// 	std::string first_line = file->ReadLine();
	char first_line[32];
	file->Rewind();
	file->Read( first_line, sizeof(first_line) );
// 	std::transform( first_line.begin(), first_line.end(), first_line.begin(), ::tolower );
	file->Rewind();

	for ( size_t i = 0; i < mImageLoaders.size(); i++ ) {
		std::vector< std::string > patterns = mImageLoaders.at(i)->contentPatterns();
		for ( size_t j = 0; j < patterns.size(); j++ ) {
			std::string test_case = patterns[j];
// 			std::transform( test_case.begin(), test_case.end(), test_case.begin(), ::tolower );
// 			if ( first_line.find( test_case ) ) {
			for ( size_t k = 0; k < sizeof(first_line)-test_case.length(); k++ ) {
				if ( !memcmp( first_line + k, test_case.c_str(), test_case.length() ) ) {
					loader = mImageLoaders.at(i);
					break;
				}
			}
		}
	}

	if ( !loader && extension.length() > 0 ) {
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
// 		*this = static_cast< Image >( *loader );
		*this = *((Image*)loader);
// 		delete loader;
		free( loader );
// 		gDebug() << "Image loaded, size = " << mWidth << " x " << mHeight << "\n";
	}
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


uint64_t Image::serverReference( Instance* instance )
{
	if ( mServerRefs.find( instance ) != mServerRefs.end() ) {
		return mServerRefs[ instance ];
	}
	uint64_t ref = instance->ReferenceImage( this );
	mServerRefs.insert( std::make_pair( instance, ref ) );
	return ref;
}


ImageLoader* Image::AddImageLoader( ImageLoader* loader )
{
	mImageLoaders.insert( mImageLoaders.begin(), loader );
	return loader;
}

} // namespace GE
