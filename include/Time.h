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

#ifndef TIME_H
#define TIME_H

#include <stdint.h>

namespace GE {

class Time {
public:
	Time();

	static void GlobalSync();
	static double Delta();
	static uint32_t GetTick();
	static float GetSeconds();
	static uint32_t WaitTick( uint32_t t, uint32_t last );
	static void Sleep( uint32_t t );

protected:
	double Sync();

private:
	static uint32_t sTime;
	static double sDt;
};

} // namespace GE

#endif // TIME_H
