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

#include <algorithm>

#include "Instance.h"
#include "Object.h"
#include "ObjectLoaderObj.h"
#include "File.h"
#include "Debug.h"


namespace GE {

std::vector< ObjectLoader* > Object::mObjectLoaders = std::vector< ObjectLoader* >();
static bool ObjectLoaderFirstCall = true;

Object::Object( Vertex* verts, uint32_t nVerts, uint32_t* indices, uint32_t nIndices )
	: mName( "" )
	, mVertices( verts )
	, mVerticesCount( nVerts )
	, mIndices( indices )
	, mIndicesCount( nIndices )
	, mMatrix( new Matrix() )
{
}


Object::Object( const std::string filename, Instance* instance )
	: mName( "" )
	, mVertices( nullptr )
	, mVerticesCount( 0 )
	, mIndices( nullptr )
	, mIndicesCount( 0 )
	, mMatrix( new Matrix() )
{
	if ( !instance ) {
		instance = Instance::baseInstance();
	}
	File* file = new File( filename, File::READ );
	ObjectLoader* loader = GetLoader( filename, file );

	if ( loader ) {
		loader = loader->NewInstance();
		loader->Load( instance, file );

		mVertices = loader->mVertices;
		mIndices = loader->mIndices;
		mVerticesCount = loader->mVerticesCount;
		mIndicesCount = loader->mIndicesCount;

// 		delete loader;
		free( loader );
	}

	delete file;
}


std::list< Object* > Object::LoadObjects( const std::string filename, Instance* instance )
{
	if ( !instance ) {
		instance = Instance::baseInstance();
	}
	File* file = new File( filename, File::READ );
	ObjectLoader* loader = GetLoader( filename, file );
	std::list< Object* > ret;

	if ( loader ) {
		loader = loader->NewInstance();
		ret = loader->LoadObjects( instance, file );
// 		delete loader;
		free( loader );
	}

	delete file;
	return ret;
}


ObjectLoader* Object::GetLoader( const std::string filename, File* file )
{
	ObjectLoader* loader = nullptr;

	if ( ObjectLoaderFirstCall ) {
		AddObjectLoader( new ObjectLoaderObj() );
		ObjectLoaderFirstCall = false;
	}

	std::string extension = filename.substr( filename.rfind( "." ) + 1 );
	std::string first_line = file->ReadLine();
	std::transform( first_line.begin(), first_line.end(), first_line.begin(), ::tolower );
	file->Rewind();
	uint32_t file_magic = 0;
	file->Read( &file_magic, sizeof(file_magic) );
	file->Rewind();

	for ( size_t i = 0; i < mObjectLoaders.size(); i++ ) {
		if ( mObjectLoaders.at(i)->fileType() == ObjectLoader::BINARY ) {
			if ( mObjectLoaders.at(i)->magic() == file_magic ) {
				loader = mObjectLoaders.at(i);
				break;
			}
		} else {
			std::vector< std::string > patterns = mObjectLoaders.at(i)->contentPatterns();
			for ( size_t j = 0; j < patterns.size(); j++ ) {
				std::string test_case = patterns[j];
				std::transform( test_case.begin(), test_case.end(), test_case.begin(), ::tolower );
				if ( first_line.find( test_case ) ) {
					loader = mObjectLoaders.at(i);
				}
			}
		}
	}

	if ( !loader ) {
		for ( size_t i = 0; i < mObjectLoaders.size(); i++ ) {
			std::vector< std::string > extensions = mObjectLoaders.at(i)->extensions();
			for ( size_t j = 0; j < extensions.size(); j++ ) {
				std::string test_case = extensions[j];
				std::transform( test_case.begin(), test_case.end(), test_case.begin(), ::tolower );
				printf(" [%s].find(%s)\n", extension.c_str(), test_case.c_str());
				if ( extension.find( test_case ) == 0 ) {
					loader = mObjectLoaders.at(i);
					break;
				}
			}
		}
	}

	printf("  loader : %p\n", loader); fflush(stdout);
	return loader;
}


void Object::operator=( Object& other )
{
	mVertices = other.mVertices;
	mVerticesCount = other.mVerticesCount;
	mIndices = other.mIndices;
	mIndicesCount = other.mIndicesCount;
	memcpy( mMatrix->data(), other.mMatrix->data(), sizeof(float) * 16 );
}


void Object::operator=( Object* other )
{
	mVertices = other->mVertices;
	mVerticesCount = other->mVerticesCount;
	mIndices = other->mIndices;
	mIndicesCount = other->mIndicesCount;
	memcpy( mMatrix->data(), other->mMatrix->data(), sizeof(float) * 16 );
}


Object::~Object()
{
	delete mMatrix;
}


const std::string& Object::name() const
{
	return mName;
}


uint32_t Object::verticesCount() const
{
	return mVerticesCount;
}


uint32_t Object::indicesCount() const
{
	return mIndicesCount;
}


Vertex* Object::vertices() const
{
	return mVertices;
}


uint32_t* Object::indices() const
{
	return mIndices;
}


Matrix* Object::matrix() const
{
	return mMatrix;
}


Vector3f Object::position() const
{
	return Vector3f( mMatrix->m[3], mMatrix->m[7], mMatrix->m[11] );
}


void Object::setName( const std::string& name )
{
	mName = name;
}


ObjectLoader* Object::AddObjectLoader( ObjectLoader* loader )
{
	mObjectLoaders.insert( mObjectLoaders.begin(), loader );
	return loader;
}


} // namespace GE
