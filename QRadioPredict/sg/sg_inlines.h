/** 
 * \file sg_inlines.h
 * Various inline template definitions.
 */

// Written by Norman Vine, started June 2000.
//
// Copyright (C) 2000  Norman Vine  - nhv@cape.com
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Library General Public
// License as published by the Free Software Foundation; either
// version 2 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Library General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
//
// $Id$


#ifndef _SG_INLINES_H
#define _SG_INLINES_H

// return the sign of a value
template <class T>
inline int SG_SIGN(const T x) {
    return x < T(0) ? -1 : 1;
}

// return the minimum of two values
template <class T>
inline T SG_MIN2(const T a, const T b) {
    return a < b ? a : b;
}

// return the minimum of three values
template <class T>
inline T SG_MIN3( const T a, const T b, const T c) {
    return (a < b ? SG_MIN2 (a, c) : SG_MIN2 (b, c));
}

// return the maximum of two values
template <class T>
inline T SG_MAX2(const T a, const T b) {
    return  a > b ? a : b;
}

// return the maximum of three values
template <class T>
inline T SG_MAX3 (const T a, const T b, const T c) {
    return (a > b ? SG_MAX2 (a, c) : SG_MAX2 (b, c));
}

// return the minimum and maximum of three values
template <class T>
inline void SG_MIN_MAX3 ( T &min, T &max, const T a, const T b, const T c) {
    if( a > b ) {
        if( a > c ) {
            max = a;
            min = SG_MIN2 (b, c);
        } else {
            max = c;
            min = SG_MIN2 (a, b);
        }
    } else {
        if( b > c ) {
            max = b;
            min = SG_MIN2 (a, c);
        } else {
            max = c;
            min = SG_MIN2 (a, b);
        }
    }
}

// swap two values
template <class T>
inline void SG_SWAP( T &a, T &b) {
    T c = a;  a = b;  b = c;
}

// clamp a value to lie between min and max
template <class T>
inline void SG_CLAMP_RANGE(T &x, const T min, const T max ) {
    if ( x < min ) { x = min; }
    if ( x > max ) { x = max; }
}

// normalize a value to lie between min and max
template <class T>
inline void SG_NORMALIZE_RANGE( T &val, const T min, const T max ) {
    T step = max - min;
    while( val >= max )  val -= step;
    while( val < min ) val += step;
}

// avoid an 'unused parameter' compiler warning.
#define SG_UNUSED(x) (void)x

// easy way to disable the copy constructor and assignment operator
// on an object
#define SG_DISABLE_COPY(Class) \
    Class(const Class &); \
    Class &operator=(const Class &);
   
#endif // _SG_INLINES_H
