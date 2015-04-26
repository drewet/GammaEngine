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

#include "ObjectLoaderObj.h"
#include "File.h"
#include "Vector.h"
#include "Debug.h"
#include "gememory.h"

#include <unordered_map>

#define RGBA(r,g,b,a) (((uint8_t)(a) << 24)|((uint8_t)(b) << 16)|((uint8_t)(g) << 8)|(uint8_t)(r))
#define RGBAf(r,g,b,a) RGBA(((r)*255.0f),((g)*255.0f),((b)*255.0f),((a)*255.0f))

namespace GE {


ObjectLoaderObj* ObjectLoaderObj::mBaseInstance = ( ObjectLoaderObj* )Object::AddObjectLoader( new ObjectLoaderObj() );


ObjectLoaderObj::ObjectLoaderObj()
	: ObjectLoader()
{
}


ObjectLoaderObj::TYPE ObjectLoaderObj::fileType()
{
	return TEXT;
}


uint32_t ObjectLoaderObj::magic()
{
	return 0;
}


std::vector< std::string > ObjectLoaderObj::contentPatterns()
{
	std::vector< std::string > ret;
	ret.emplace_back( "obj" );
	ret.emplace_back( "wavefront" );
	return ret;
}


std::vector< std::string > ObjectLoaderObj::extensions()
{
	std::vector< std::string > ret;
	ret.emplace_back( "obj" );
	return ret;
}


ObjectLoader* ObjectLoaderObj::NewInstance()
{
	return new ObjectLoaderObj();
}


void ObjectLoaderObj::Load( File* file )
{
	fDebug( file );

	std::string line;
	std::stringstream tokenizer;
	std::stringstream tokenizer2;
	std::string type;
	std::string str;
	std::string str2;
	float f = 0.0f;

	std::vector< Vector3f > verts;
	std::vector< Vector3f > tex;
	std::vector< Vector3f > norms;
	Material* mat = nullptr;

	uint32_t iVert = 0;
	uint32_t iTex = 0;
	uint32_t iNorm = 0;
	uint32_t iFace = 0;

	verts.resize( 1024 );
	tex.resize( 1024 );
	norms.resize( 1024 );

	Vertex* buff = ( Vertex* )geMalloc( sizeof( Vertex ) * 1024 );
	uint32_t iBuff = 0;
	uint32_t maxBuff = 1024;
	std::unordered_map< std::string, uint32_t > elements;

	uint32_t* indices = ( uint32_t* )geMalloc( sizeof( uint32_t ) * 128 * 3 );
	uint32_t iIndices = 0;
	uint32_t maxIndices = 128 * 3;

	while ( file->ReadLine( line ) )
	{
		line[line.length() - 1] = 0;
		tokenizer.clear();
		tokenizer.str( line );

		tokenizer >> type;

		if ( type == "mtllib" ) {
			tokenizer >> str;
			LoadMaterials( file, str );
		}

		if ( type == "usemtl" ) {
			tokenizer >> str;
			mat = mMaterials[ str ];
// 			gDebug() << "mat = " << mat << "\n";
		}

		if ( type == "v" ) {
			if ( iVert + 1 >= verts.size() ) {
				verts.resize( verts.size() + 1024 );
			}
			tokenizer >> f; verts[iVert].x = f;
			tokenizer >> f; verts[iVert].y = f;
			tokenizer >> f; verts[iVert].z = f;
// 			aDebug( "verts[" << iVert << "]", verts[iVert].x, verts[iVert].y, verts[iVert].z ) << "\n";
			iVert++;
		}

		if ( type == "vn" ) {
			if ( iNorm + 1 >= norms.size() ) {
				norms.resize( norms.size() + 1024 );
			}
			tokenizer >> f; norms[iNorm].x = f;
			tokenizer >> f; norms[iNorm].y = f;
			tokenizer >> f; norms[iNorm].z = f;
// 			aDebug( "norms[" << iNorm << "]", norms[iNorm].x, norms[iNorm].y, norms[iNorm].z ) << "\n";
			iNorm++;
		}

		if ( type == "vt" ) {
			if ( iTex + 1 >= tex.size() ) {
				tex.resize( tex.size() + 1024 );
			}
			tokenizer >> f; tex[iTex].x = f;
			tokenizer >> f; tex[iTex].y = f;
			tokenizer >> f; tex[iTex].z = f;
// 			aDebug( "tex[" << iTex << "]", tex[iTex].x, tex[iTex].y, tex[iTex].z ) << "\n";
			iTex++;
		}

		if ( type == "f" )
		{
			uint32_t face_indices[4] = { 0 }; // Indice in GE's object
			uint32_t n = 0;
			while ( !tokenizer.eof() )
			{
				uint32_t idx = 0;
				tokenizer >> str;
				if ( str == "" || str.length() <= 1 ) {
					break;
				}
// 				gDebug() << "f[" << iFace << "][" << n << "] = '" << str << "'\n";
				if ( elements.find( str ) != elements.end() )
				{
					idx = elements[ str ];
				}
				else
				{
					idx = iBuff;
					elements.insert( std::pair< std::string, uint32_t >( str, idx ) );

					int point_indexes[3] = { 0 }; // Indexes in OBJ vertices tables
					uint32_t p = 0;
					tokenizer2.clear();
					tokenizer2.str( str );
					while(std::getline( tokenizer2, str2, '/') ) {
						point_indexes[p] = std::stoi( str2 );
						p++;
					}
					if ( p == 3 ) {
						buff[idx].x = verts[point_indexes[0]].x;
						buff[idx].y = verts[point_indexes[0]].y;
						buff[idx].z = verts[point_indexes[0]].z;
						buff[idx].u = tex[point_indexes[1]].x;
						buff[idx].v = tex[point_indexes[1]].y;
						buff[idx].w = tex[point_indexes[1]].z;
						buff[idx].nx = norms[point_indexes[2]].x;
						buff[idx].ny = norms[point_indexes[2]].y;
						buff[idx].nz = norms[point_indexes[2]].z;
						buff[idx].color = mat->diffuse;
// 						aDebug( "buff[" << idx << "].xyz = ", buff[idx].x, buff[idx].y, buff[idx].z ) << "\n";
// 						aDebug( "buff[" << idx << "].uvw = ", buff[idx].u, buff[idx].v, buff[idx].w ) << "\n";
// 						aDebug( "buff[" << idx << "].nnn = ", buff[idx].nx, buff[idx].ny, buff[idx].nz ) << "\n";
					}

					if ( iBuff + 1 >= maxBuff ) {
						buff = ( Vertex* )geRealloc( buff, maxBuff + 1024 );
						maxBuff += 1024;
					}
					iBuff++;
				}
				face_indices[n] = idx;
				n++;
			}
			if ( n == 3 ) { // Triangle
				memcpy( &indices[iIndices], face_indices, sizeof( uint32_t ) * 3 );
				if ( iIndices + 3 >= maxIndices ) {
					indices = ( uint32_t* )geRealloc( indices, maxIndices + 128 * 3 );
					maxIndices += 128 * 3;
				}
				iIndices += 3;
			} else if ( n == 4 ) { // Quad
				// TODO
				iIndices += 4;
			}
			iFace++;
		}
	}

	for ( std::map< std::string, Material* >::iterator it = mMaterials.begin(); it != mMaterials.end(); ++it ) {
		delete (*it).second;
	}

	mVerticesCount = iBuff;
	mVertices = ( Vertex* )geMalloc( sizeof( Vertex ) * iBuff, false );
	memcpy( mVertices, buff, sizeof( Vertex ) * iBuff );
	geFree( buff );

	mIndicesCount = iIndices;
	mIndices = ( uint32_t* )geMalloc( sizeof( uint32_t ) * iIndices, false );
	memcpy( mIndices, indices, sizeof( uint32_t ) * iIndices );
	geFree( indices );
}


void ObjectLoaderObj::LoadMaterials( File* base_file, std::string filename )
{
	File* file = new File( base_file, filename, File::READ );

	std::string line;
	std::stringstream tokenizer;
	std::string type;
	std::string str;
	float alpha = 0.0f;
	float f1 = 0.0f;
	float f2 = 0.0f;
	float f3 = 0.0f;
	Material* mat = nullptr;

	while ( file->ReadLine( line ) )
	{
		line[line.length() - 1] = 0;
		tokenizer.clear();
		tokenizer.str( line );

		tokenizer >> type;

		if ( type == "newmtl" ) {
			tokenizer >> str;
			mat = new Material;
			mMaterials.insert( std::pair< std::string, Material* >( str, mat ) );
		}

		if ( type == "Tr" ) {
			tokenizer >> alpha;
		}

		if ( type == "Ka" ) {
			tokenizer >> f1;
			tokenizer >> f2;
			tokenizer >> f3;
			mat->ambient = RGBAf( f1, f2, f3, alpha );
		}

		if ( type == "Kd" ) {
			tokenizer >> f1;
			tokenizer >> f2;
			tokenizer >> f3;
			mat->diffuse = RGBAf( f1, f2, f3, alpha );
		}

		if ( type == "Ks" ) {
			tokenizer >> f1;
			tokenizer >> f2;
			tokenizer >> f3;
			mat->specular = RGBAf( f1, f2, f3, alpha );
		}
	}

	delete file;
}

} // namespace GE
