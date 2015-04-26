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

#ifndef FILE_H
#define FILE_H


#include <sys/types.h>
#include <string>
#include <fstream>


namespace GE {


class File
{
public:
	typedef enum {
		READ = 0x1,
		WRITE = 0x2
	} MODE;
	typedef enum {
		BEGIN,
		CURR,
		END
	} DIR;

	File( std::string filename, MODE mode );
	File( File* side, std::string filename, MODE mode );
	File( void* data, size_t len, MODE mode, bool copy_buffer = false );
	~File();

	void Rewind();
	uint64_t Seek( uint64_t ofs, DIR dir );
	uint64_t Read( void* buf, size_t len );
	bool ReadLine( std::string& line );
	std::string ReadLine();


private:
	typedef enum {
		FILE,
		BUFFER
	} TYPE;

	TYPE mType;
	MODE mMode;
	std::string mPath;
	std::fstream* mStream;
	unsigned char* mBuffer;
	uint64_t mBufferSize;
	uint64_t mOffset;
	bool mCopiedBuffer;
};


} // namespace GE

#endif // FILE_H
