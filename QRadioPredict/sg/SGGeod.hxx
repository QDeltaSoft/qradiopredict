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

#ifndef SGGeod_H
#define SGGeod_H

#include "constants.h"


// #define SG_GEOD_NATIVE_DEGREE

/// Class representing a geodetic location
class SGGeod {
public:
  /// Default constructor, initializes the instance to lat = lon = elev = 0
  SGGeod(void);

  /// Factory from angular values in radians and elevation is 0
  static SGGeod fromRad(double lon, double lat);
  /// Factory from angular values in degrees and elevation is 0
  static SGGeod fromDeg(double lon, double lat);
  /// Factory from angular values in radians and elevation in ft
  static SGGeod fromRadFt(double lon, double lat, double elevation);
  /// Factory from angular values in degrees and elevation in ft
  static SGGeod fromDegFt(double lon, double lat, double elevation);
  /// Factory from angular values in radians and elevation in m
  static SGGeod fromRadM(double lon, double lat, double elevation);
  /// Factory from angular values in degrees and elevation in m
  static SGGeod fromDegM(double lon, double lat, double elevation);
  /// Factory from an other SGGeod and a different elevation in m
  static SGGeod fromGeodM(const SGGeod& geod, double elevation);
  /// Factory from an other SGGeod and a different elevation in ft
  static SGGeod fromGeodFt(const SGGeod& geod, double elevation);
  /// Factory to convert position from a cartesian position assumed to be
  /// in wgs84 measured in meters
  /// Note that this conversion is relatively expensive to compute
  static SGGeod fromCart(const SGVec3<double>& cart);
  /// Factory to convert position from a geocentric position
  /// Note that this conversion is relatively expensive to compute
  static SGGeod fromGeoc(const SGGeoc& geoc);

  /// Return the geodetic longitude in radians
  double getLongitudeRad(void) const;
  /// Set the geodetic longitude from the argument given in radians
  void setLongitudeRad(double lon);

  /// Return the geodetic longitude in degrees
  double getLongitudeDeg(void) const;
  /// Set the geodetic longitude from the argument given in degrees
  void setLongitudeDeg(double lon);

  /// Return the geodetic latitude in radians
  double getLatitudeRad(void) const;
  /// Set the geodetic latitude from the argument given in radians
  void setLatitudeRad(double lat);

  /// Return the geodetic latitude in degrees
  double getLatitudeDeg(void) const;
  /// Set the geodetic latitude from the argument given in degrees
  void setLatitudeDeg(double lat);

  /// Return the geodetic elevation in meters
  double getElevationM(void) const;
  /// Set the geodetic elevation from the argument given in meters
  void setElevationM(double elevation);

  /// Return the geodetic elevation in feet
  double getElevationFt(void) const;
  /// Set the geodetic elevation from the argument given in feet
  void setElevationFt(double elevation);

  /// Compare two geodetic positions for equality
  bool operator == ( const SGGeod & other ) const;

  /// check the Geod contains sane values (finite, inside appropriate
  /// ranges for lat/lon)
  bool isValid() const;
private:
  /// This one is private since construction is not unique if you do
  /// not know the units of the arguments. Use the factory methods for
  /// that purpose
  SGGeod(double lon, double lat, double elevation);

  //// FIXME: wrong comment!
  /// The actual data, angles in degrees, elevation in meters
  /// The rationale for storing the values in degrees is that most code places
  /// in flightgear/terragear use degrees as a nativ input and output value.
  /// The places where it makes sense to use radians is when we convert
  /// to other representations or compute rotation matrices. But both tasks
  /// are computionally intensive anyway and that additional 'toRadian'
  /// conversion does not hurt too much
  double _lon;
  double _lat;
  double _elevation;
};

inline
SGGeod::SGGeod(void) :
  _lon(0), _lat(0), _elevation(0)
{
}

inline
SGGeod::SGGeod(double lon, double lat, double elevation) :
  _lon(lon), _lat(lat), _elevation(elevation)
{
}

inline
SGGeod
SGGeod::fromRad(double lon, double lat)
{
#ifdef SG_GEOD_NATIVE_DEGREE
  return SGGeod(lon*SGD_RADIANS_TO_DEGREES, lat*SGD_RADIANS_TO_DEGREES, 0);
#else
  return SGGeod(lon, lat, 0);
#endif
}

inline
SGGeod
SGGeod::fromDeg(double lon, double lat)
{
#ifdef SG_GEOD_NATIVE_DEGREE
  return SGGeod(lon, lat, 0);
#else
  return SGGeod(lon*SGD_DEGREES_TO_RADIANS, lat*SGD_DEGREES_TO_RADIANS, 0);
#endif
}

inline
SGGeod
SGGeod::fromRadFt(double lon, double lat, double elevation)
{
#ifdef SG_GEOD_NATIVE_DEGREE
  return SGGeod(lon*SGD_RADIANS_TO_DEGREES, lat*SGD_RADIANS_TO_DEGREES,
                elevation*SG_FEET_TO_METER);
#else
  return SGGeod(lon, lat, elevation*SG_FEET_TO_METER);
#endif
}

inline
SGGeod
SGGeod::fromDegFt(double lon, double lat, double elevation)
{
#ifdef SG_GEOD_NATIVE_DEGREE
  return SGGeod(lon, lat, elevation*SG_FEET_TO_METER);
#else
  return SGGeod(lon*SGD_DEGREES_TO_RADIANS, lat*SGD_DEGREES_TO_RADIANS,
                elevation*SG_FEET_TO_METER);
#endif
}

inline
SGGeod
SGGeod::fromRadM(double lon, double lat, double elevation)
{
#ifdef SG_GEOD_NATIVE_DEGREE
  return SGGeod(lon*SGD_RADIANS_TO_DEGREES, lat*SGD_RADIANS_TO_DEGREES,
                elevation);
#else
  return SGGeod(lon, lat, elevation);
#endif
}

inline
SGGeod
SGGeod::fromDegM(double lon, double lat, double elevation)
{
#ifdef SG_GEOD_NATIVE_DEGREE
  return SGGeod(lon, lat, elevation);
#else
  return SGGeod(lon*SGD_DEGREES_TO_RADIANS, lat*SGD_DEGREES_TO_RADIANS,
                elevation);
#endif
}

inline
SGGeod
SGGeod::fromGeodM(const SGGeod& geod, double elevation)
{
  return SGGeod(geod._lon, geod._lat, elevation);
}

inline
SGGeod
SGGeod::fromGeodFt(const SGGeod& geod, double elevation)
{
  return SGGeod(geod._lon, geod._lat, elevation*SG_FEET_TO_METER);
}

inline
SGGeod
SGGeod::fromCart(const SGVec3<double>& cart)
{
  SGGeod geod;
  SGGeodesy::SGCartToGeod(cart, geod);
  return geod;
}

inline
SGGeod
SGGeod::fromGeoc(const SGGeoc& geoc)
{
  SGVec3<double> cart;
  SGGeodesy::SGGeocToCart(geoc, cart);
  SGGeod geod;
  SGGeodesy::SGCartToGeod(cart, geod);
  return geod;
}

inline
double
SGGeod::getLongitudeRad(void) const
{
#ifdef SG_GEOD_NATIVE_DEGREE
  return _lon*SGD_DEGREES_TO_RADIANS;
#else
  return _lon;
#endif
}

inline
void
SGGeod::setLongitudeRad(double lon)
{
#ifdef SG_GEOD_NATIVE_DEGREE
  _lon = lon*SGD_RADIANS_TO_DEGREES;
#else
  _lon = lon;
#endif
}

inline
double
SGGeod::getLongitudeDeg(void) const
{
#ifdef SG_GEOD_NATIVE_DEGREE
  return _lon;
#else
  return _lon*SGD_RADIANS_TO_DEGREES;
#endif
}

inline
void
SGGeod::setLongitudeDeg(double lon)
{
#ifdef SG_GEOD_NATIVE_DEGREE
  _lon = lon;
#else
  _lon = lon*SGD_DEGREES_TO_RADIANS;
#endif
}

inline
double
SGGeod::getLatitudeRad(void) const
{
#ifdef SG_GEOD_NATIVE_DEGREE
  return _lat*SGD_DEGREES_TO_RADIANS;
#else
  return _lat;
#endif
}

inline
void
SGGeod::setLatitudeRad(double lat)
{
#ifdef SG_GEOD_NATIVE_DEGREE
  _lat = lat*SGD_RADIANS_TO_DEGREES;
#else
  _lat = lat;
#endif
}

inline
double
SGGeod::getLatitudeDeg(void) const
{
#ifdef SG_GEOD_NATIVE_DEGREE
  return _lat;
#else
  return _lat*SGD_RADIANS_TO_DEGREES;
#endif
}

inline
void
SGGeod::setLatitudeDeg(double lat)
{
#ifdef SG_GEOD_NATIVE_DEGREE
  _lat = lat;
#else
  _lat = lat*SGD_DEGREES_TO_RADIANS;
#endif
}

inline
double
SGGeod::getElevationM(void) const
{
  return _elevation;
}

inline
void
SGGeod::setElevationM(double elevation)
{
  _elevation = elevation;
}

inline
double
SGGeod::getElevationFt(void) const
{
  return _elevation*SG_METER_TO_FEET;
}

inline
void
SGGeod::setElevationFt(double elevation)
{
  _elevation = elevation*SG_FEET_TO_METER;
}

inline
bool
SGGeod::operator == ( const SGGeod & other ) const
{
  return _lon == other._lon &&
         _lat == other._lat &&
         _elevation == other._elevation;
}

inline
bool
SGGeod::isValid() const
{
  if (SGMiscd::isNaN(_lon))
      return false;
  if (SGMiscd::isNaN(_lat))
      return false;
#ifdef SG_GEOD_NATIVE_DEGREE
  return (_lon >= -180.0) && (_lon <= 180.0) &&
  (_lat >= -90.0) && (_lat <= 90.0);
#else
  return (_lon >= -SGD_PI) && (_lon <= SGD_PI) &&
  (_lat >= -SGD_PI_2) && (_lat <= SGD_PI_2);
#endif
}

/// Output to an ostream
template<typename char_type, typename traits_type>
inline
std::basic_ostream<char_type, traits_type>&
operator<<(std::basic_ostream<char_type, traits_type>& s, const SGGeod& g)
{
  return s << "lon = " << g.getLongitudeDeg()
           << "deg, lat = " << g.getLatitudeDeg()
           << "deg, elev = " << g.getElevationM()
           << "m";
}

#endif
