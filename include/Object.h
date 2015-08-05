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

#ifndef OBJECT_H
#define OBJECT_H

#include <string>
#include <vector>
#include <list>
#include <map>

#include "Vertex.h"
#include "Matrix.h"
#include "MetaObject.h"

namespace GE {

class Instance;
class ObjectLoader;
class File;
class Image;

class Object : public MetaObject
{
public:
	Object( Vertex* verts = nullptr, uint32_t nVerts = 0, uint32_t* indices = nullptr, uint32_t nIndices = 0 );
	Object( const std::string filename, Instance* instance = nullptr );
	virtual ~Object();

	const std::string& name() const;
	uint32_t verticesCount() const;
	uint32_t indicesCount() const;
	Vertex* vertices() const;
	uint32_t* indices() const;
	Matrix* matrix() const;

	void setName( const std::string& name );

	virtual void setTexture( Instance* instance, int unit, Image* texture ) = 0;
	virtual void UpdateVertices( Instance* instance, Vertex* verts, uint32_t offset, uint32_t count ) = 0;
	virtual void UploadMatrix( Instance* instance ) = 0;

	void operator=( Object& other );
	void operator=( Object* other );

	static std::list< Object* > LoadObjects( const std::string filename, Instance* instance = nullptr );
	static ObjectLoader* AddObjectLoader( ObjectLoader* loader );

protected:
	static ObjectLoader* GetLoader( const std::string filename, File* file );
	std::string mName;
	Vertex* mVertices;
	uint32_t mVerticesCount;
	uint32_t* mIndices;
	uint32_t mIndicesCount;
	Matrix* mMatrix;

	static std::vector< ObjectLoader* > mObjectLoaders;
};


class ObjectLoader : public Object
{
public:
	typedef enum {
		UNKNOWN = -1,
		BINARY,
		TEXT
	} TYPE;
	ObjectLoader() : Object() { ; }
	virtual ~ObjectLoader() { ; }
	virtual TYPE fileType() = 0;
	virtual uint32_t magic() = 0;
	virtual std::vector< std::string > contentPatterns() = 0;
	virtual std::vector< std::string > extensions() = 0;
	virtual ObjectLoader* NewInstance() = 0;
	virtual void Load( Instance* instance, File* file ) = 0;
	virtual std::list< Object* > LoadObjects( Instance* instance, File* file ) = 0;

	virtual void setTexture( Instance* instance, int unit, Image* texture ){}
	virtual void UpdateVertices( Instance* instance, Vertex* verts, uint32_t offset, uint32_t count ){}
	virtual void UploadMatrix( Instance* instance ){}
};


} // namespace GE

#endif // OBJECT_H
