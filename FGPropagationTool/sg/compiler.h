/**************************************************************************
 * compiler.h -- C++ Compiler Portability Macros
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 *
 * $Id$
 *
 **************************************************************************/

/** \file compiler.h
 * A set of defines to encapsulate compiler and platform differences.
 * Please refer to the source code for full documentation on this file.
 *
 * This file is useful to set compiler-specific options in every file - for
 * example, disabling warnings.
 *
 */

#ifndef _SG_COMPILER_H
#define _SG_COMPILER_H

/*
 * Helper macro SG_STRINGIZE:
 * Converts the parameter X to a string after macro replacement
 * on X has been performed.
 */
#define SG_STRINGIZE(X) SG_DO_STRINGIZE(X)
#define SG_DO_STRINGIZE(X) #X

#ifdef __GNUC__
#  if __GNUC__ < 3
#    error Time to upgrade. GNU compilers < 3.0 not supported
#  elif (__GNUC__ == 3) && (__GNUC_MINOR__ < 4)
#    warning GCC compilers prior to 3.4 are suspect  
#  endif

#  define GCC_VERSION (__GNUC__ * 10000 \
                     + __GNUC_MINOR__ * 100 \
                     + __GNUC_PATCHLEVEL__)
#  define SG_COMPILER_STR "GNU C++ version " SG_STRINGIZE(__GNUC__) "." SG_STRINGIZE(__GNUC_MINOR__)
#endif // __GNUC__

/* KAI C++ */
#if defined(__KCC)
#  define SG_COMPILER_STR "Kai C++ version " SG_STRINGIZE(__KCC_VERSION)
#endif // __KCC

//
// Microsoft compilers.
//
#ifdef _MSC_VER
#  define bcopy(from, to, n) memcpy(to, from, n)

#  if _MSC_VER >= 1200  // msvc++ 6.0 or greater
#    define isnan _isnan
#    define snprintf _snprintf
#    if _MSC_VER < 1500
#      define vsnprintf _vsnprintf
#    endif
#    define copysign _copysign
#    define strcasecmp stricmp

#    undef min
#    undef max

#    pragma warning(disable: 4786) // identifier was truncated to '255' characters
#    pragma warning(disable: 4244) // conversion from double to float
#    pragma warning(disable: 4305) //

#  else
#    error What version of MSVC++ is this?
#  endif

#    define SG_COMPILER_STR "Microsoft Visual C++ version " SG_STRINGIZE(_MSC_VER)

#endif // _MSC_VER

//
// Native SGI compilers
//

#if defined ( sgi ) && !defined( __GNUC__ )
# if (_COMPILER_VERSION < 740)
#  error Need MipsPro 7.4.0 or higher now
# endif

#define SG_HAVE_NATIVE_SGI_COMPILERS

#pragma set woff 1001,1012,1014,1116,1155,1172,1174
#pragma set woff 1401,1460,1551,1552,1681

#ifdef __cplusplus
# pragma set woff 1682,3303
# pragma set woff 3624
#endif

#  define SG_COMPILER_STR "SGI MipsPro compiler version " SG_STRINGIZE(_COMPILER_VERSION)

#endif // Native SGI compilers


#if defined (__sun)
#  define SG_UNIX
#  include <strings.h>
#  include <memory.h>
#  if defined ( __cplusplus )
     // typedef unsigned int size_t;
     extern "C" {
       extern void *memmove(void *, const void *, size_t);
     }
#  else
     extern void *memmove(void *, const void *, size_t);
#  endif // __cplusplus

#  if  !defined( __GNUC__ )
#   define SG_COMPILER_STR "Sun compiler version " SG_STRINGIZE(__SUNPRO_CC)
#  endif

#endif // sun

//
// Intel C++ Compiler
//
#if defined(__ICC) || defined (__ECC)
#  define SG_COMPILER_STR "Intel C++ version " SG_STRINGIZE(__ICC)
#endif // __ICC

//
// Platform dependent gl.h and glut.h definitions
//

#ifdef __APPLE__
#  define SG_MAC
#  define SG_UNIX
#  ifdef __GNUC__
#    if ( __GNUC__ > 3 ) || ( __GNUC__ == 3 && __GNUC_MINOR__ >= 3 )
inline int (isnan)(double r) { return !(r <= 0 || r >= 0); }
#    else
    // any C++ header file undefines isinf and isnan
    // so this should be included before <iostream>
    // the functions are STILL in libm (libSystem on mac os x)
extern "C" int (isnan)(double);
extern "C" int (isinf)(double);
#    endif
#  else
inline int (isnan)(double r) { return !(r <= 0 || r >= 0); }
#  endif
#endif

#if defined (__FreeBSD__)
#  define SG_UNIX
#include <sys/param.h>
#  if __FreeBSD_version < 500000
     extern "C" {
       inline int isnan(double r) { return !(r <= 0 || r >= 0); }
     }
#  endif
#endif

#if defined (__CYGWIN__)
#  define SG_WINDOWS
#  define SG_UNIX
#  include <ieeefp.h>		// isnan
#endif

// includes both MSVC and mingw compilers
#if defined(_WIN32) || defined(__WIN32__)
#  define SG_WINDOWS
#endif

#if defined(__linux__) || defined(_AIX) || defined ( sgi )
#  define SG_UNIX
#endif

#if defined( __GNUC__ )
#  define DEPRECATED __attribute__ ((deprecated))
#else
#  define DEPRECATED
#endif

//
// No user modifiable definitions beyond here.
//

#endif // _SG_COMPILER_H

