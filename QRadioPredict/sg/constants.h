// constants.h -- various constant definitions
//
// Written by Curtis Olson, started February 2000.
// Last change by Eric van den Berg, Feb 2013
//
// Copyright (C) 2000  Curtis L. Olson - http://www.flightgear.org/~curt/
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

/** \file constants.h
 * Various constant definitions.
 */


#ifndef _SG_CONSTANTS_H
#define _SG_CONSTANTS_H


#include "compiler.h"

#include <cmath>


// Make sure PI is defined in its various forms

#ifndef SGD_PI // remove me once FlightGear no longer uses PLIB

#ifdef M_PI
const double SGD_PI = M_PI;
const float SG_PI = M_PI;
#else
const float SG_PI = 3.1415926535f;
const double SGD_PI = 3.1415926535;
#endif

#endif // of PLIB-SG guard

/** 2 * PI */
const double SGD_2PI = SGD_PI * 2.0;

/** PI / 2 */
#ifdef M_PI_2
#  define  SGD_PI_2  M_PI_2
#else
#  define  SGD_PI_2  1.57079632679489661923
#endif

/** PI / 4 */
const double SGD_PI_4 = 0.78539816339744830961;

#ifndef SGD_DEGREES_TO_RADIANS // // remove me once FlightGear no longer uses PLIB

const double SGD_DEGREES_TO_RADIANS = SGD_PI / 180.0;
const double SGD_RADIANS_TO_DEGREES = 180.0 / SGD_PI;

const float SG_DEGREES_TO_RADIANS = SG_PI / 180.0f;
const float SG_RADIANS_TO_DEGREES = 180.0f / SG_PI;

#endif // of PLIB-SG guard

/** \def SG_E "e" */
#ifdef M_E
#  define SG_E     M_E
#else
#  define SG_E     2.7182818284590452354
#endif

/** pi/180/60/60, or about 100 feet at earths' equator */
#define SG_ONE_SECOND 4.848136811E-6


/** Radius of Earth in kilometers at the equator.  Another source had
 *  6378.165 but this is probably close enough */
#define SG_EARTH_RAD 6378.155

// Maximum terrain elevation from sea level
#define SG_MAX_ELEVATION_M 9000.0

// Earth parameters for WGS 84, taken from LaRCsim/ls_constants.h

/** Value of earth radius from LaRCsim (ft) */
#define SG_EQUATORIAL_RADIUS_FT  20925650.

/** Value of earth radius from LaRCsim (meter) */
#define SG_EQUATORIAL_RADIUS_M   6378138.12

/** Radius squared (ft) */
#define SG_EQ_RAD_SQUARE_FT 437882827922500.

/** Radius squared (meter) */
#define SG_EQ_RAD_SQUARE_M   40680645877797.1344


// Physical Constants, SI

/**mean gravity on earth */
#define SG_g0_m_p_s2          9.80665   // m/s2

/**standard pressure at SL */
#define SG_p0_Pa              101325.0 // Pa

/**standard density at SL */
#define SG_rho0_kg_p_m3       1.225 // kg/m3

/**standard temperature at SL */
#define SG_T0_K               288.15   // K (=15degC)

/**specific gas constant of air*/
#define SG_R_m2_p_s2_p_K      287.05   // m2/s2/K

/**specific heat constant at constant pressure*/
#define SG_cp_m2_p_s2_p_K 1004.68      // m2/s2/K   

/**ratio of specific heats of air*/
#define SG_gamma              1.4         // =cp/cv (cp = 1004.68 m2/s2 K , cv = 717.63 m2/s2 K)

/**constant beta used to calculate dynamic viscosity */
#define SG_beta_kg_p_sm_sqrK  1.458e-06   // kg/s/m/SQRT(K) 

/** Sutherland constant */
#define SG_S_K                110.4       // K


// Conversions

/** Arc seconds to radians.  (arcsec*pi)/(3600*180) = rad */
#define SG_ARCSEC_TO_RAD    4.84813681109535993589e-06 

/** Radians to arc seconds.  (rad*3600*180)/pi = arcsec */
#define SG_RAD_TO_ARCSEC    206264.806247096355156

/** Feet to Meters */
#define SG_FEET_TO_METER    0.3048

/** Meters to Feet */
#define SG_METER_TO_FEET    3.28083989501312335958  

/** Meters to Nautical Miles.  1 nm = 6076.11549 feet */
#define SG_METER_TO_NM      0.0005399568034557235

/** Nautical Miles to Meters */
#define SG_NM_TO_METER      1852.0000

/** Meters to Statute Miles. */
#define SG_METER_TO_SM      0.0006213699494949496

/** Statute Miles to Meters. */
#define SG_SM_TO_METER      1609.3412196

/** Radians to Nautical Miles.  1 nm = 1/60 of a degree */
#define SG_NM_TO_RAD        0.00029088820866572159

/** Nautical Miles to Radians */
#define SG_RAD_TO_NM        3437.7467707849392526

/** meters per second to Knots */
#define SG_MPS_TO_KT        1.9438444924406046432

/** Knots to meters per second */
#define SG_KT_TO_MPS        0.5144444444444444444

/** Feet per second to Knots */
#define SG_FPS_TO_KT        0.5924838012958962841

/** Knots to Feet per second */
#define SG_KT_TO_FPS        1.6878098571011956874

/** meters per second to Miles per hour */
#define SG_MPS_TO_MPH       2.2369362920544020312

/** meetrs per hour to Miles per second */
#define SG_MPH_TO_MPS       0.44704

/** Meters per second to Kilometers per hour */
#define SG_MPS_TO_KMH       3.6

/** Kilometers per hour to meters per second */
#define SG_KMH_TO_MPS       0.2777777777777777778

/** Pascal to Inch Mercury */
#define SG_PA_TO_INHG       0.0002952998330101010

/** Inch Mercury to Pascal */
#define SG_INHG_TO_PA       3386.388640341

/** slug/ft3 to kg/m3 */
#define SG_SLUGFT3_TO_KGPM3   515.379


/** For divide by zero avoidance, this will be close enough to zero */
#define SG_EPSILON 0.0000001

/** Highest binobj format version we know how to read/write.  This starts at
 *  0 and can go up to 65535 */
#define SG_BINOBJ_VERSION 6

/** for backwards compatibility */
#define SG_SCENERY_FILE_FORMAT "0.4"


#endif // _SG_CONSTANTS_H
