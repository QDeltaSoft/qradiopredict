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

#ifndef SGGeodesy_H
#define SGGeodesy_H


class SGGeodesy {
public:
  // Hard numbers from the WGS84 standard.
  static const double EQURAD;
  static const double iFLATTENING;
  static const double SQUASH;
  static const double STRETCH;
  static const double POLRAD;

  /// Takes a cartesian coordinate data and returns the geodetic
  /// coordinates.
  static void SGCartToGeod(const SGVec3<double>& cart, SGGeod& geod);
  
  /// Takes a geodetic coordinate data and returns the cartesian
  /// coordinates.
  static void SGGeodToCart(const SGGeod& geod, SGVec3<double>& cart);
  
  /// Takes a geodetic coordinate data and returns the sea level radius.
  static double SGGeodToSeaLevelRadius(const SGGeod& geod);

  /// Takes a cartesian coordinate data and returns the geocentric
  /// coordinates.
  static void SGCartToGeoc(const SGVec3<double>& cart, SGGeoc& geoc);
  
  /// Takes a geocentric coordinate data and returns the cartesian
  /// coordinates.
  static void SGGeocToCart(const SGGeoc& geoc, SGVec3<double>& cart);

  // Geodetic course/distance computation
  static bool direct(const SGGeod& p1, double course1,
                     double distance, SGGeod& p2, double& course2);

  /// overloaded version of above, returns new value directly, throws
  /// an sg_exception on failure.
  static SGGeod direct(const SGGeod& p1, double course1,
                     double distance);

  static bool inverse(const SGGeod& p1, const SGGeod& p2, double& course1,
                      double& course2, double& distance);

  static double courseDeg(const SGGeod& from, const SGGeod& to);
  static double distanceM(const SGGeod& from, const SGGeod& to);
  static double distanceNm(const SGGeod& from, const SGGeod& to);
    
  // Geocentric course/distance computation
  static void advanceRadM(const SGGeoc& geoc, double course, double distance,
                          SGGeoc& result);
  static double courseRad(const SGGeoc& from, const SGGeoc& to);
  static double distanceRad(const SGGeoc& from, const SGGeoc& to);
  static double distanceM(const SGGeoc& from, const SGGeoc& to);
  
  /**
   * compute the intersection of two (true) radials (in degrees), or return false
   * if no intersection culd be computed.
   */
  static bool radialIntersection(const SGGeoc& a, double aRadial, 
    const SGGeoc& b, double bRadial, SGGeoc& result);
    
  static bool radialIntersection(const SGGeod& a, double aRadial, 
    const SGGeod& b, double bRadial, SGGeod& result);
};

#endif
