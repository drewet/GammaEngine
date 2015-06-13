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

#include "Instance.h"
#include "Vector.h"
#include "Debug.h"
#include <cmath>

namespace GE {



template <typename T, int n> Vector<T,n>::Vector( T x, T y, T z, T w ) : x( x ), y( y ), z( z ), w( w ) {}

template <typename T, int n> void Vector<T,n>::normalize() {
	T add = 0;
	for ( int i = 0; i < n; i++ ) {
		add += ( &x )[i] * ( &x )[i];
	}
	T l = std::sqrt( add );
	if ( l > 0.00001f ) {
		T il = 1 / l;
		for ( int i = 0; i < n; i++ ) {
			( &x )[i] *= il;
		}
	}
}


template <typename T, int n> T Vector<T,n>::length() {
	T add = 0;
	for ( int i = 0; i < n; i++ ) {
		add += ( &x )[i] * ( &x )[i];
	}
	return std::sqrt( add );
}


template <typename T, int n> T Vector<T,n>::operator[]( int i ) const
{
	return ( &x )[i];
}


template <typename T, int n> Vector<T,n> Vector<T,n>::operator-() const
{
	Vector<T, n> ret;
	for ( int i = 0; i < n; i++ ) {
		( &ret.x )[i] = -( &x )[i];
	}
	return ret;
}


template <typename T, int n> void Vector<T,n>::operator+=( const Vector<T,n>& v ) {
	for ( int i = 0; i < n; i++ ) {
		( &x )[i] += ( &v.x )[i];
	}
}


template <typename T, int n> void Vector<T,n>::operator-=( const Vector<T,n>& v ) {
	for ( int i = 0; i < n; i++ ) {
		( &x )[i] -= ( &v.x )[i];
	}
}

template <typename T, int n> void Vector<T,n>::operator*=( T v ) {
	for ( int i = 0; i < n; i++ ) {
		( &x )[i] *= v;
	}
}


template <typename T, int n> Vector<T,n> Vector<T,n>::operator+( const Vector<T,n>& v ) const {
	Vector<T, n> ret;
	for ( int i = 0; i < n; i++ ) {
		( &ret.x )[i] = ( &x )[i] + ( &v.x )[i];
	}
	return ret;
}

template <typename T, int n> Vector<T,n> Vector<T,n>::operator-( const Vector<T,n>& v ) const {
	Vector<T, n> ret;
	for ( int i = 0; i < n; i++ ) {
		( &ret.x )[i] = ( &x )[i] - ( &v.x )[i];
	}
	return ret;
}

template <typename T, int n> Vector<T,n> Vector<T,n>::operator*( T im ) const {
	Vector<T, n> ret;
	for ( int i = 0; i < n; i++ ) {
		( &ret.x )[i] = ( &x )[i] * im;
	}
	return ret;
}

template <typename T, int n> T Vector<T,n>::operator*( const Vector<T,n>& v ) const {
	T ret = 0;
	for ( int i = 0; i < n; i++ ) {
		ret += ( &x )[i] * ( &v.x )[i];
	}
	return ret;
}

template <typename T, int n> Vector<T,n> Vector<T,n>::operator^( const Vector<T,n>& v ) const {
	Vector<T, n> ret;
	for ( int i = 0; i < n; i++ ) {
		T a = ( &x )[ ( i + 1 ) % n ];
		T b = ( &v.x )[ ( i + 2 ) % n ];
		T c = ( &x )[ ( i + 2 ) % n ];
		T d = ( &v.x )[ ( i + 1 ) % n ];
		( &ret.x )[i] = a * b - c * d;
	}
	return ret;
}

template <typename T, int n> Vector<T, n> operator*( T im, const Vector<T, n>& v ) {
	Vector<T, n> ret;
	for ( int i = 0; i < n; i++ ) {
		( &ret.x )[i] = ( &v.x )[i] * im;
	}
	return ret;
}


template <typename T, int n> bool operator==( const Vector<T, n>& v1, const Vector<T,n>& v2 ) {
	bool ret = true;
	for ( int i = 0; i < n; i++ ) {
		ret = ret && ( ( &v1.x )[i] == ( &v2.x )[i] );
	}
	return ret;
}

static void dummy_vectors()
{
	dummy_vectors();
	{
		Vector2i v;
		v += -v + ( v ^ v ) - v * ( 0 * v ) + v * 0;
		v.normalize();
		v *= 0 * v.length();
		v.x = v[0];
		v -= v * (v == v);
	}
	{
		Vector3i v;
		v += -v + ( v ^ v ) - v * ( 0 * v ) + v * 0;
		v.normalize();
		v *= 0 * v.length();
		v.x = v[0];
		v -= v * (v == v);
	}
	{
		Vector4i v;
		v += -v + ( v ^ v ) - v * ( 0 * v ) + v * 0;
		v.normalize();
		v *= 0 * v.length();
		v.x = v[0];
		v -= v * (v == v);
	}
	{
		Vector2f v;
		v += -v + ( v ^ v ) - v * ( 0.0f * v ) + v * 0.0f;
		v.normalize();
		v *= 0 * v.length();
		v.x = v[0];
		v -= v * (v == v);
	}
	{
		Vector3f v;
		v += -v + ( v ^ v ) - v * ( 0.0f * v ) + v * 0.0f;
		v.normalize();
		v *= 0 * v.length();
		v.x = v[0];
		v -= v * (v == v);
	}
	{
		Vector4f v;
		v += -v + ( v ^ v ) - v * ( 0.0f * v ) + v * 0.0f;
		v.normalize();
		v *= 0 * v.length();
		v.x = v[0];
		v -= v * (v == v);
	}
	{
		Vector2d v;
		v += -v + ( v ^ v ) - v * ( 0.0 * v ) + v * 0.0;
		v.normalize();
		v *= 0 * v.length();
		v.x = v[0];
		v -= v * (v == v);
	}
	{
		Vector3d v;
		v += -v + ( v ^ v ) - v * ( 0.0 * v ) + v * 0.0;
		v.normalize();
		v *= 0 * v.length();
		v.x = v[0];
		v -= v * (v == v);
	}
	{
		Vector4d v;
		v += -v + ( v ^ v ) - v * ( 0.0 * v ) + v * 0.0;
		v.normalize();
		v *= 0 * v.length();
		v.x = v[0];
		v -= v * (v == v);
	}
}

} // namespace GE
