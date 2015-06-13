/*
 * <one line to give the library's name and an idea of what it does.>
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

#include "Instance.h"
#include "File.h"
#include "Debug.h"
#include <errno.h>
#include <string.h>


namespace GE {


File::File( std::string filename, MODE mode )
	: mType( FILE )
	, mMode( mode )
{
	fDebug( filename, mode );

	std::ios_base::openmode std_mode = std::ios_base::binary;

	if ( mode & READ ) {
		std_mode |= std::ios_base::in;
	}
	if ( mode & WRITE ) {
		std_mode |= std::ios_base::out;
	}

	mStream = new std::fstream( filename, std_mode );
	mPath = filename;
}


File::File( File* side, std::string filename, File::MODE mode )
	: mType( FILE )
	, mMode( mode )
{
	fDebug( side, filename, mode );

	if ( filename.length() <= 0 ) {
		((int*)0)[0] = 0;
	}

	std::string path = side->mPath.substr( 0, side->mPath.rfind( "/" ) + 1 ) + filename;

	std::ios_base::openmode std_mode = std::ios_base::binary;

	if ( mode & READ ) {
		std_mode |= std::ios_base::in;
	}
	if ( mode & WRITE ) {
		std_mode |= std::ios_base::out;
	}

	mStream = new std::fstream( path, std_mode );
	mPath = path;
}


File::File( void* data, size_t len, MODE mode, bool copy_buffer, Instance* instance )
	: mType( BUFFER )
	, mMode( mode )
	, mCopiedBuffer( copy_buffer )
	, mInstance( instance ? instance : Instance::baseInstance() )
{
	if ( copy_buffer ) {
		mBuffer = ( unsigned char* )mInstance->Malloc( len, false );
		memcpy( mBuffer, data, len );
	} else {
		mBuffer = ( unsigned char* )data;
	}
	mBufferSize = len;
	mOffset = 0;

	std::stringstream path;
	path << "gemem://" << std::hex << mBuffer << ":" << std::dec << mBufferSize;
	mPath = path.str();
}


File::~File()
{
	if ( mType == BUFFER ) {
		if ( mCopiedBuffer ) {
			mInstance->Free( mBuffer );
		}
	} else if ( mType == FILE ) {
		mStream->close();
		delete mStream;
	}
}


bool File::isOpened()
{
	if ( mType == BUFFER ) {
		return true;
	} else if ( mType == FILE ) {
		return mStream && mStream->is_open();
	}
	return false;
}


void File::Rewind()
{
	if ( mType == BUFFER ) {
		mOffset = 0;
	} else if ( mType == FILE ) {
		mStream->seekg( 0, std::ios_base::beg );
	}
}


size_t File::Seek( uint64_t ofs, DIR dir )
{
	if ( mType == BUFFER ) {
		if ( dir == BEGIN && ofs >= 0 && ofs < mBufferSize ) {
			mOffset = ofs;
		} else if ( dir == CURR && mOffset + ofs >= 0 && mOffset + ofs < mBufferSize ) {
			mOffset = mOffset + ofs;
		} else if ( dir == END && mBufferSize + ofs >= 0 && mBufferSize + ofs < mBufferSize ) {
			mOffset = mBufferSize + ofs;
		}
		return mOffset;
	}

	std::ios_base::seekdir std_dir = (std::ios_base::seekdir)-1;
	std_dir = ( dir == BEGIN ) ? std::ios_base::beg : std_dir;
	std_dir = ( dir == CURR ) ? std::ios_base::cur : std_dir;
	std_dir = ( dir == END ) ? std::ios_base::end : std_dir;
	if ( (int)std_dir != -1 ) {
		mStream->seekg( ofs, std_dir );
	}
	return mStream->tellg();
}


uint64_t File::Read( void* buf, uint64_t len )
{
	uint64_t ret = 0;

	if ( mType == BUFFER ) {
		if ( mOffset + len >= mBufferSize ) {
			len = mBufferSize - mOffset;
		}
		memcpy( buf, &mBuffer[mOffset], len );
		ret = len;
	} else if ( mType == FILE ) {
		mStream->read( (char*)buf, len );
		ret = mStream->gcount();
	}

	return ret;
}


bool File::ReadLine( std::string& line )
{
	if ( mType == BUFFER ) {
		// TODO
	} else if ( mType == FILE ) {
		std::getline( *mStream, line, '\n' );
		return !mStream->eof();
	}
	return false;
}


std::string File::ReadLine()
{
	std::string ret;
	ReadLine( ret );
	return ret;
}


} // namespace GE
