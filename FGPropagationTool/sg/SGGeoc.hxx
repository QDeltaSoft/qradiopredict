// Copyright (C) 2006  Mathias Froehlich - Mathias.Froehlich@web.de
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

#ifndef SGGeoc_H
#define SGGeoc_H

#include "constants.h"

 
// #define SG_GEOC_NATIVE_DEGREE

/// Class representing a geocentric location
class SGGeoc {
public:
  /// Default constructor, initializes the instance to lat = lon = lat = 0
  SGGeoc(void);

  /// Factory from angular values in radians and radius in ft
  static SGGeoc fromRadFt(double lon, double lat, double radius);
  /// Factory from angular values in degrees and radius in ft
  static SGGeoc fromDegFt(double lon, double lat, double radius);
  /// Factory from angular values in radians and radius in m
  static SGGeoc fromRadM(double lon, double lat, double radius);
  /// Factory from angular values in degrees and radius in m
  static SGGeoc fromDegM(double lon, double lat, double radius);
  /// Factory to convert position from a cartesian position assumed to be
  /// in wgs84 measured in meters
  /// Note that this conversion is relatively expensive to compute
  static SGGeoc fromCart(const SGVec3<double>& cart);
  /// Factory to convert position from a geodetic position
  /// Note that this conversion is relatively expensive to compute
  static SGGeoc fromGeod(const SGGeod& geod);

  /// Return the geocentric longitude in radians
  double getLongitudeRad(void) const;
  /// Set the geocentric longitude from the argument given in radians
  void setLongitudeRad(double lon);

  /// Return the geocentric longitude in degrees
  double getLongitudeDeg(void) const;
  /// Set the geocentric longitude from the argument given in degrees
  void setLongitudeDeg(double lon);

  /// Return the geocentric latitude in radians
  double getLatitudeRad(void) const;
  /// Set the geocentric latitude from the argument given in radians
  void setLatitudeRad(double lat);

  /// Return the geocentric latitude in degrees
  double getLatitudeDeg(void) const;
  /// Set the geocentric latitude from the argument given in degrees
  void setLatitudeDeg(double lat);

  /// Return the geocentric radius in meters
  double getRadiusM(void) const;
  /// Set the geocentric radius from the argument given in meters
  void setRadiusM(double radius);

  /// Return the geocentric radius in feet
  double getRadiusFt(void) const;
  /// Set the geocentric radius from the argument given in feet
  void setRadiusFt(double radius);

  SGGeoc advanceRadM(double course, double distance) const;
  static double courseRad(const SGGeoc& from, const SGGeoc& to);
  static double courseDeg(const SGGeoc& from, const SGGeoc& to);
  static double distanceM(const SGGeoc& from, const SGGeoc& to);

  // Compare two geocentric positions for equality
  bool operator == ( const SGGeoc & other ) const;
private:
  /// This one is private since construction is not unique if you do
  /// not know the units of the arguments, use the factory methods for
  /// that purpose
  SGGeoc(double lon, double lat, double radius);

  /// The actual data, angles in degree, radius in meters
  /// The rationale for storing the values in degrees is that most code places
  /// in flightgear/terragear use degrees as a nativ input and output value.
  /// The places where it makes sense to use radians is when we convert
  /// to other representations or compute rotation matrices. But both tasks
  /// are computionally intensive anyway and that additional 'toRadian'
  /// conversion does not hurt too much
  double _lon;
  double _lat;
  double _radius;
};

inline
SGGeoc::SGGeoc(void) :
  _lon(0), _lat(0), _radius(0)
{
}

inline
SGGeoc::SGGeoc(double lon, double lat, double radius) :
  _lon(lon), _lat(lat), _radius(radius)
{
}

inline
SGGeoc
SGGeoc::fromRadFt(double lon, double lat, double radius)
{
#ifdef SG_GEOC_NATIVE_DEGREE
  return SGGeoc(lon*SGD_RADIANS_TO_DEGREES, lat*SGD_RADIANS_TO_DEGREES,
                radius*SG_FEET_TO_METER);
#else
  return SGGeoc(lon, lat, radius*SG_FEET_TO_METER);
#endif
}

inline
SGGeoc
SGGeoc::fromDegFt(double lon, double lat, double radius)
{
#ifdef SG_GEOC_NATIVE_DEGREE
  return SGGeoc(lon, lat, radius*SG_FEET_TO_METER);
#else
  return SGGeoc(lon*SGD_DEGREES_TO_RADIANS, lat*SGD_DEGREES_TO_RADIANS,
                radius*SG_FEET_TO_METER);
#endif
}

inline
SGGeoc
SGGeoc::fromRadM(double lon, double lat, double radius)
{
#ifdef SG_GEOC_NATIVE_DEGREE
  return SGGeoc(lon*SGD_RADIANS_TO_DEGREES, lat*SGD_RADIANS_TO_DEGREES,
                radius);
#else
  return SGGeoc(lon, lat, radius);
#endif
}

inline
SGGeoc
SGGeoc::fromDegM(double lon, double lat, double radius)
{
#ifdef SG_GEOC_NATIVE_DEGREE
  return SGGeoc(lon, lat, radius);
#else
  return SGGeoc(lon*SGD_DEGREES_TO_RADIANS, lat*SGD_DEGREES_TO_RADIANS,
                radius);
#endif
}

inline
SGGeoc
SGGeoc::fromCart(const SGVec3<double>& cart)
{
  SGGeoc geoc;
  SGGeodesy::SGCartToGeoc(cart, geoc);
  return geoc;
}

inline
SGGeoc
SGGeoc::fromGeod(const SGGeod& geod)
{
  SGVec3<double> cart;
  SGGeodesy::SGGeodToCart(geod, cart);
  SGGeoc geoc;
  SGGeodesy::SGCartToGeoc(cart, geoc);
  return geoc;
}

inline
double
SGGeoc::getLongitudeRad(void) const
{
#ifdef SG_GEOC_NATIVE_DEGREE
  return _lon*SGD_DEGREES_TO_RADIANS;
#else
  return _lon;
#endif
}

inline
void
SGGeoc::setLongitudeRad(double lon)
{
#ifdef SG_GEOC_NATIVE_DEGREE
  _lon = lon*SGD_RADIANS_TO_DEGREES;
#else
  _lon = lon;
#endif
}

inline
double
SGGeoc::getLongitudeDeg(void) const
{
#ifdef SG_GEOC_NATIVE_DEGREE
  return _lon;
#else
  return _lon*SGD_RADIANS_TO_DEGREES;
#endif
}

inline
void
SGGeoc::setLongitudeDeg(double lon)
{
#ifdef SG_GEOC_NATIVE_DEGREE
  _lon = lon;
#else
  _lon = lon*SGD_DEGREES_TO_RADIANS;
#endif
}

inline
double
SGGeoc::getLatitudeRad(void) const
{
#ifdef SG_GEOC_NATIVE_DEGREE
  return _lat*SGD_DEGREES_TO_RADIANS;
#else
  return _lat;
#endif
}

inline
void
SGGeoc::setLatitudeRad(double lat)
{
#ifdef SG_GEOC_NATIVE_DEGREE
  _lat = lat*SGD_RADIANS_TO_DEGREES;
#else
  _lat = lat;
#endif
}

inline
double
SGGeoc::getLatitudeDeg(void) const
{
#ifdef SG_GEOC_NATIVE_DEGREE
  return _lat;
#else
  return _lat*SGD_RADIANS_TO_DEGREES;
#endif
}

inline
void
SGGeoc::setLatitudeDeg(double lat)
{
#ifdef SG_GEOC_NATIVE_DEGREE
  _lat = lat;
#else
  _lat = lat*SGD_DEGREES_TO_RADIANS;
#endif
}

inline
double
SGGeoc::getRadiusM(void) const
{
  return _radius;
}

inline
void
SGGeoc::setRadiusM(double radius)
{
  _radius = radius;
}

inline
double
SGGeoc::getRadiusFt(void) const
{
  return _radius*SG_METER_TO_FEET;
}

inline
void
SGGeoc::setRadiusFt(double radius)
{
  _radius = radius*SG_FEET_TO_METER;
}

inline
SGGeoc
SGGeoc::advanceRadM(double course, double distance) const
{
  SGGeoc result;
  SGGeodesy::advanceRadM(*this, course, distance, result);
  return result;
}

inline
double
SGGeoc::courseRad(const SGGeoc& from, const SGGeoc& to)
{
  return SGGeodesy::courseRad(from, to);
}

inline
double
SGGeoc::courseDeg(const SGGeoc& from, const SGGeoc& to)
{
  return SGMiscd::rad2deg(courseRad(from, to));
}

inline
double
SGGeoc::distanceM(const SGGeoc& from, const SGGeoc& to)
{
  return SGGeodesy::distanceM(from, to);
}

inline
bool
SGGeoc::operator == ( const SGGeoc & other ) const
{
  return _lon == other._lon &&
         _lat == other._lat &&
         _radius == other._radius;
}

/// Output to an ostream
template<typename char_type, typename traits_type>
inline
std::basic_ostream<char_type, traits_type>&
operator<<(std::basic_ostream<char_type, traits_type>& s, const SGGeoc& g)
{
  return s << "lon = " << g.getLongitudeDeg()
           << ", lat = " << g.getLatitudeDeg()
           << ", radius = " << g.getRadiusM();
}

#endif
