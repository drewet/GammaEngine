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

#include <time.h>
#include "Time.h"

namespace GE {


Time::Time()
	: mTime( 0 )
{
}


float Time::Sync()
{
	uint32_t nTime = GetTick();

	if ( mTime == 0 ) {
		mTime = GetTick();
	}
	uint32_t dt = nTime - mTime;

	mTime = nTime;

	return ( (float)dt ) / 1000.0f;
}


uint32_t Time::GetTick()
{
	struct timespec now;
	clock_gettime( CLOCK_MONOTONIC, &now );
	return now.tv_sec * 1000 + now.tv_nsec / 1000000;
}


float Time::GetSeconds()
{
	struct timespec now;
	clock_gettime( CLOCK_MONOTONIC, &now );
	float ret = (float)now.tv_sec;
	uint32_t ms = now.tv_nsec / 1000000;
	ret += ( (float)ms ) / 1000.0;
	return ret;
}


} // namespace GE
