#ifndef _SG_GEODESY_HXX
#define _SG_GEODESY_HXX

#include "SGMath.hxx"

// Compatibility header.
// Please use the SGGeodesy and SGMath functions directly.

/**
 * Convert from geodetic coordinates to geocentric coordinates.
 * WARNING: this function is non-reversible.  Due to the fact that
 * "up" is a different direction for geocentric and geodetic frames,
 * you can not simply add your "alt" parameter to the "sl_radius"
 * result and get back (via sgGeodToGeoc()) to the coordinates you
 * started with.  The error under normal conditions will be of
 * centimeter order; whether that is important or not is application
 * dependent. Consider using sgGeodToCart() instead.
 *
 * @param lat_geod (in) Geodetic latitude, radians, + = North
 * @param alt (in) C.G. altitude above mean sea level (meters)
 * @param sl_radius (out) SEA LEVEL radius to earth center (meters)
 * @param lat_geoc (out) Geocentric latitude, radians, + = North
 */
inline void sgGeodToGeoc(double lat_geod, double alt,
                         double *sl_radius, double *lat_geoc)
{
  SGVec3<double> cart;
  SGGeod geod = SGGeod::fromRadM(0, lat_geod, alt);
  SGGeodesy::SGGeodToCart(geod, cart);
  SGGeoc geoc;
  SGGeodesy::SGCartToGeoc(cart, geoc);
  *lat_geoc = geoc.getLatitudeRad();
  *sl_radius = SGGeodesy::SGGeodToSeaLevelRadius(geod);
}


/**
 * Convert a cartesian point to a geodetic lat/lon/altitude.
 *
 * @param xyz (in) Pointer to cartesian point.
 * @param lat (out) Latitude, in radians
 * @param lon (out) Longitude, in radians
 * @param alt (out) Altitude, in meters above the WGS84 ellipsoid
 */
inline void sgCartToGeod(const double* xyz, double* lat, double* lon, double* alt)
{
  SGGeod geod;
  SGGeodesy::SGCartToGeod(SGVec3<double>(xyz), geod);
  *lat = geod.getLatitudeRad();
  *lon = geod.getLongitudeRad();
  *alt = geod.getElevationM();
}

/**
 * Convert a geodetic lat/lon/altitude to a cartesian point.
 *
 * @param lat (in) Latitude, in radians
 * @param lon (in) Longitude, in radians
 * @param alt (in) Altitude, in meters above the WGS84 ellipsoid
 * @param xyz (out) Pointer to cartesian point.
 */
inline void sgGeodToCart(double lat, double lon, double alt, double* xyz)
{
  SGVec3<double> cart;
  SGGeodesy::SGGeodToCart(SGGeod::fromRadM(lon, lat, alt), cart);
  xyz[0] = cart(0);
  xyz[1] = cart(1);
  xyz[2] = cart(2);
}

/**
 * Given a starting position and an offset radial and distance,
 * calculate an ending positon on a wgs84 ellipsoid.
 * @param alt (in) meters (unused)
 * @param lat1 (in) degrees
 * @param lon1 (in) degrees
 * @param az1 (in) degrees
 * @param s (in) distance in meters
 * @param lat2 (out) degrees
 * @param lon2 (out) degrees
 * @param az2 (out) return course in degrees
 */
inline int geo_direct_wgs_84 ( double lat1, double lon1, double az1, 
                               double s, double *lat2, double *lon2,
                               double *az2 )
{
  SGGeod p2;
  if (!SGGeodesy::direct(SGGeod::fromDeg(lon1, lat1), az1, s, p2, *az2))
    return 1;
  *lat2 = p2.getLatitudeDeg();
  *lon2 = p2.getLongitudeDeg();
  return 0;
}
inline int geo_direct_wgs_84 ( double alt, double lat1,
                        double lon1, double az1, 
			double s, double *lat2, double *lon2,
                        double *az2 )
{ return geo_direct_wgs_84(lat1, lon1, az1, s, lat2, lon2, az2); }

/**
 * Given a starting position and an offset radial and distance,
 * calculate an ending positon on a wgs84 ellipsoid.
 * @param p1 (in) geodetic position
 * @param az1 (in) degrees
 * @param s (in) distance in meters
 * @param p2 (out) geodetic position
 * @param az2 (out) return course in degrees
 */
inline int geo_direct_wgs_84(const SGGeod& p1, double az1,
                             double s, SGGeod& p2, double *az2 )
{
  return !SGGeodesy::direct(p1, az1, s, p2, *az2);
}

/**
 * Given an altitude and two sets of (lat, lon) calculate great circle
 * distance between them as well as the starting and ending azimuths.
 * @param alt (in) meters (unused)
 * @param lat1 (in) degrees
 * @param lon1 (in) degrees
 * @param lat2 (in) degrees
 * @param lon2 (in) degrees
 * @param az1 (out) start heading degrees
 * @param az2 (out) end heading degrees
 * @param s (out) distance meters
 */
inline int geo_inverse_wgs_84( double lat1, double lon1, double lat2,
                               double lon2, double *az1, double *az2,
                               double *s )
{
  return !SGGeodesy::inverse(SGGeod::fromDeg(lon1, lat1),
                             SGGeod::fromDeg(lon2, lat2), *az1, *az2, *s);
}
inline int geo_inverse_wgs_84( double alt, double lat1,
                               double lon1, double lat2,
                               double lon2, double *az1, double *az2,
                               double *s )
{ return geo_inverse_wgs_84(lat1, lon1, lat2, lon2, az1, az2, s); }


/**
 * Given an altitude and two sets of (lat, lon) calculate great circle
 * distance between them as well as the starting and ending azimuths.
 * @param p1 (in) first position
 * @param p2 (in) fsecond position
 * @param az1 (out) start heading degrees
 * @param az2 (out) end heading degrees
 * @param s (out) distance meters
 */
inline int geo_inverse_wgs_84(const SGGeod& p1, const SGGeod& p2,
                              double *az1, double *az2, double *s )
{
  return !SGGeodesy::inverse(p1, p2, *az1, *az2, *s);
}

#endif // _SG_GEODESY_HXX
