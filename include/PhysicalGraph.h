/*
 * <one line to give the library's name and an idea of what it does.>
 * Copyright (C) 2015  Adrien Aubry <email>
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

#ifndef GE_PHYSICALGRAPH_H
#define GE_PHYSICALGRAPH_H

#include <list>
#include "PhysicalCollider.h"
#include "PhysicalBody.h"

namespace GE
{

class Instance;

class PhysicalGraph : protected Time
{
public:
	PhysicalGraph( Instance* instance = nullptr );
	~PhysicalGraph();

	PhysicalCollider* collider();

	void AddBody( PhysicalBody* body );

	void Update();

	Instance* instance();

protected:
	Instance* mInstance;
	PhysicalCollider* mCollider;
	std::list< PhysicalBody* > mBodies;
};

} // namespace GE

#endif // GE_PHYSICALGRAPH_H