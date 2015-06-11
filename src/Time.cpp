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

#include <cmath>
#include <unistd.h>
#include <time.h>
#include "Time.h"

namespace GE {


int64_t Time::sTime = 0;
double Time::sDt = 0.0;


Time::Time()
	: mDt( 0.0 )
	, mSlowDt( 0.0 )
	, mParent( nullptr )
{
}


void Time::setTimeParent( Time* t )
{
	mParent = t;
}


void Time::setTime( int64_t t )
{
	int64_t dt = t - sTime;
	sDt = ( (double)dt ) / 1000.0;
	sTime = t;
}


void Time::GlobalSync()
{
	if ( sTime == 0 ) {
		sTime = GetTick();
	}

	int64_t dt = GetTick() - sTime;
	sDt = ( (double)dt ) / 1000.0;
	sTime = GetTick();
}


double Time::Delta()
{
	return sDt;
}


double Time::Sync()
{
	if ( mParent ) {
		return mParent->mDt;
	}

// 	uint32_t nTime = GetTick();

	if ( sTime == 0 ) {
		GlobalSync();
	}

	mDt = sDt;
	return mDt;

// 	uint32_t dt = GetTick() - sTime;
// 	return ( (float)dt ) / 1000.0f;
}


double Time::SlowSync( double min )
{
	mSlowDt += Sync();
	if ( std::abs( mSlowDt ) >= min ) {
		mDt = mSlowDt;
		mSlowDt = 0.0;
		return mDt;
	}
	return 0.0;
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


uint32_t Time::WaitTick( uint32_t t, uint32_t last )
{
	uint32_t ticks = GetTick();
	if ( ( ticks - last ) < t ) {
		Sleep( t - (ticks - last) - 1);
	}
	return GetTick();
}


void Time::Sleep( uint32_t t )
{
	usleep( t * 1000 );
}


} // namespace GE
