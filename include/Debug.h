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

#ifndef DEBUG_H
#define DEBUG_H

#include <string.h>

#include <iostream>
#include <sstream>
#include <string>
#include <typeinfo>


namespace GE {


class Debug
{
public:
	Debug();
	~Debug();


	template<typename T> Debug& operator<<( T t )
	{
		std::cout << t;
		if ( mWriteFile ) {
			// TODO
		}
		return *this;
	}

	static uint64_t GetTicks();

private:
	bool mWriteFile;
};

#ifndef __DBG_CLASS
static inline std::string className(const std::string& prettyFunction)
{
	size_t colons = prettyFunction.find("::");
	if ( prettyFunction.find( "GE::" ) ) {
		colons += 2 + prettyFunction.substr(colons + 2).find( "::" );
	}
	if (colons == std::string::npos)
		return "<none>";
	size_t begin = prettyFunction.substr(0,colons).rfind(" ") + 1;
	size_t end = colons - begin;
	return prettyFunction.substr(begin,end);
}

#define __CLASS_NAME__ className(__PRETTY_FUNCTION__)

#define gDebug() Debug() << __CLASS_NAME__ << "::" << __FUNCTION__ << "() "
// #define fDebug( x ) Debug() << __CLASS_NAME__ << "::" << __FUNCTION__ << "( " << x << " )\n"


static void fDebug_base( const char* end, bool f ) {
	std::cout << " " << end;
}

#include <cxxabi.h>

template<typename Arg1, typename... Args> static void fDebug_base( const char* end, bool first, const Arg1& arg1, const Args&... args ) {
	const char* type = abi::__cxa_demangle(typeid(arg1).name(), nullptr, nullptr, nullptr);
	char cap = 0;
	if ( strstr( type, "char" ) ) {
		if ( strstr( type, "*" ) || ( strstr( type, "[" ) && strstr( type, "]" ) ) ) {
			cap = '\"';
		} else {
			cap = '\'';
		}
	}

	if (!first ) {
		std::cout << ", ";
	}
// 	std::cout << "[" << type << "]";
	std::cout << cap;
	std::cout << arg1;
	std::cout << cap;
	fDebug_base( end, false, args... );
}

#define fDebug( args... ) std::cout << __CLASS_NAME__ << "::" << __FUNCTION__ << "( "; fDebug_base( ")\n", true, args )
#define fDebug0() std::cout << __CLASS_NAME__ << "::" << __FUNCTION__ << "()\n"

#define aDebug( name, args... ) std::cout << __CLASS_NAME__ << "::" << __FUNCTION__ << " " << name << " = { "; fDebug_base( "}", true, args )

#endif // __DBG_CLASS


} // namespace GE

#endif // DEBUG_H
