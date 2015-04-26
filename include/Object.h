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
#include <map>

#include "Vertex.h"
#include "vulkan.h"

namespace GE {

class Instance;
class ObjectLoader;
class File;

class Object
{
public:
	Object();
	Object( const std::string filename );
	~Object();

	uint32_t verticesCount();
	uint32_t indicesCount();
	VK_DESCRIPTOR_SET descriptorSet( Instance* instance, int devid );
	VK_MEMORY_REF indicesRef( Instance* instance, int devid );

	static ObjectLoader* AddObjectLoader( ObjectLoader* loader );

protected:
	Vertex* mVertices;
	uint32_t mVerticesCount;
	uint32_t* mIndices;
	uint32_t mIndicesCount;

	// descriptorSet, descriptorMemRef, vertexDataMemRef, indexMemRef associated to <Instance*, devid>
	std::map< std::pair< Instance*, int >, std::tuple< VK_DESCRIPTOR_SET, VK_MEMORY_REF, VK_MEMORY_REF, VK_MEMORY_REF > > mVkRefs;

	static std::vector< ObjectLoader* > mObjectLoaders;

	void AllocateGpu( Instance* instance, int devid );
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
	virtual void Load( File* file ) = 0;
};


} // namespace GE

#endif // OBJECT_H
