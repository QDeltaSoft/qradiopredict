// Copyright (C) 2006-2009  Mathias Froehlich - Mathias.Froehlich@web.de
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

#ifndef SGVec4_H
#define SGVec4_H

#include <iosfwd>

/// 4D Vector Class
template<typename T>
class SGVec4 {
public:
  typedef T value_type;

  /// Default constructor. Does not initialize at all.
  /// If you need them zero initialized, use SGVec4::zeros()
  SGVec4(void)
  {
    /// Initialize with nans in the debug build, that will guarantee to have
    /// a fast uninitialized default constructor in the release but shows up
    /// uninitialized values in the debug build very fast ...
#ifndef NDEBUG
    for (unsigned i = 0; i < 4; ++i)
      data()[i] = SGLimits<T>::quiet_NaN();
#endif
  }
  /// Constructor. Initialize by the given values
  SGVec4(T x, T y, T z, T w)
  { data()[0] = x; data()[1] = y; data()[2] = z; data()[3] = w; }
  /// Constructor. Initialize by the content of a plain array,
  /// make sure it has at least 3 elements
  explicit SGVec4(const T* d)
  { data()[0] = d[0]; data()[1] = d[1]; data()[2] = d[2]; data()[3] = d[3]; }
  template<typename S>
  explicit SGVec4(const SGVec4<S>& d)
  { data()[0] = d[0]; data()[1] = d[1]; data()[2] = d[2]; data()[3] = d[3]; }
  explicit SGVec4(const SGVec3<T>& v3, const T& v4 = 0)
  { data()[0] = v3[0]; data()[1] = v3[1]; data()[2] = v3[2]; data()[3] = v4; }

  /// Access by index, the index is unchecked
  const T& operator()(unsigned i) const
  { return data()[i]; }
  /// Access by index, the index is unchecked
  T& operator()(unsigned i)
  { return data()[i]; }

  /// Access raw data by index, the index is unchecked
  const T& operator[](unsigned i) const
  { return data()[i]; }
  /// Access raw data by index, the index is unchecked
  T& operator[](unsigned i)
  { return data()[i]; }

  /// Access the x component
  const T& x(void) const
  { return data()[0]; }
  /// Access the x component
  T& x(void)
  { return data()[0]; }
  /// Access the y component
  const T& y(void) const
  { return data()[1]; }
  /// Access the y component
  T& y(void)
  { return data()[1]; }
  /// Access the z component
  const T& z(void) const
  { return data()[2]; }
  /// Access the z component
  T& z(void)
  { return data()[2]; }
  /// Access the x component
  const T& w(void) const
  { return data()[3]; }
  /// Access the x component
  T& w(void)
  { return data()[3]; }

  /// Readonly raw storage interface
  const T (&data(void) const)[4]
  { return _data; }
  /// Readonly raw storage interface
  T (&data(void))[4]
  { return _data; }

  /// Inplace addition
  SGVec4& operator+=(const SGVec4& v)
  { data()[0]+=v(0);data()[1]+=v(1);data()[2]+=v(2);data()[3]+=v(3);return *this; }
  /// Inplace subtraction
  SGVec4& operator-=(const SGVec4& v)
  { data()[0]-=v(0);data()[1]-=v(1);data()[2]-=v(2);data()[3]-=v(3);return *this; }
  /// Inplace scalar multiplication
  template<typename S>
  SGVec4& operator*=(S s)
  { data()[0] *= s; data()[1] *= s; data()[2] *= s; data()[3] *= s; return *this; }
  /// Inplace scalar multiplication by 1/s
  template<typename S>
  SGVec4& operator/=(S s)
  { return operator*=(1/T(s)); }

  /// Return an all zero vector
  static SGVec4 zeros(void)
  { return SGVec4(0, 0, 0, 0); }
  /// Return unit vectors
  static SGVec4 e1(void)
  { return SGVec4(1, 0, 0, 0); }
  static SGVec4 e2(void)
  { return SGVec4(0, 1, 0, 0); }
  static SGVec4 e3(void)
  { return SGVec4(0, 0, 1, 0); }
  static SGVec4 e4(void)
  { return SGVec4(0, 0, 0, 1); }

private:
  T _data[4];
};

/// Unary +, do nothing ...
template<typename T>
inline
const SGVec4<T>&
operator+(const SGVec4<T>& v)
{ return v; }

/// Unary -, do nearly nothing
template<typename T>
inline
SGVec4<T>
operator-(const SGVec4<T>& v)
{ return SGVec4<T>(-v(0), -v(1), -v(2), -v(3)); }

/// Binary +
template<typename T>
inline
SGVec4<T>
operator+(const SGVec4<T>& v1, const SGVec4<T>& v2)
{ return SGVec4<T>(v1(0)+v2(0), v1(1)+v2(1), v1(2)+v2(2), v1(3)+v2(3)); }

/// Binary -
template<typename T>
inline
SGVec4<T>
operator-(const SGVec4<T>& v1, const SGVec4<T>& v2)
{ return SGVec4<T>(v1(0)-v2(0), v1(1)-v2(1), v1(2)-v2(2), v1(3)-v2(3)); }

/// Scalar multiplication
template<typename S, typename T>
inline
SGVec4<T>
operator*(S s, const SGVec4<T>& v)
{ return SGVec4<T>(s*v(0), s*v(1), s*v(2), s*v(3)); }

/// Scalar multiplication
template<typename S, typename T>
inline
SGVec4<T>
operator*(const SGVec4<T>& v, S s)
{ return SGVec4<T>(s*v(0), s*v(1), s*v(2), s*v(3)); }

/// multiplication as a multiplicator, that is assume that the first vector
/// represents a 4x4 diagonal matrix with the diagonal elements in the vector.
/// Then the result is the product of that matrix times the second vector.
template<typename T>
inline
SGVec4<T>
mult(const SGVec4<T>& v1, const SGVec4<T>& v2)
{ return SGVec4<T>(v1(0)*v2(0), v1(1)*v2(1), v1(2)*v2(2), v1(3)*v2(3)); }

/// component wise min
template<typename T>
inline
SGVec4<T>
min(const SGVec4<T>& v1, const SGVec4<T>& v2)
{
  return SGVec4<T>(SGMisc<T>::min(v1(0), v2(0)),
                   SGMisc<T>::min(v1(1), v2(1)),
                   SGMisc<T>::min(v1(2), v2(2)),
                   SGMisc<T>::min(v1(3), v2(3)));
}
template<typename S, typename T>
inline
SGVec4<T>
min(const SGVec4<T>& v, S s)
{
  return SGVec4<T>(SGMisc<T>::min(s, v(0)),
                   SGMisc<T>::min(s, v(1)),
                   SGMisc<T>::min(s, v(2)),
                   SGMisc<T>::min(s, v(3)));
}
template<typename S, typename T>
inline
SGVec4<T>
min(S s, const SGVec4<T>& v)
{
  return SGVec4<T>(SGMisc<T>::min(s, v(0)),
                   SGMisc<T>::min(s, v(1)),
                   SGMisc<T>::min(s, v(2)),
                   SGMisc<T>::min(s, v(3)));
}

/// component wise max
template<typename T>
inline
SGVec4<T>
max(const SGVec4<T>& v1, const SGVec4<T>& v2)
{
  return SGVec4<T>(SGMisc<T>::max(v1(0), v2(0)),
                   SGMisc<T>::max(v1(1), v2(1)),
                   SGMisc<T>::max(v1(2), v2(2)),
                   SGMisc<T>::max(v1(3), v2(3)));
}
template<typename S, typename T>
inline
SGVec4<T>
max(const SGVec4<T>& v, S s)
{
  return SGVec4<T>(SGMisc<T>::max(s, v(0)),
                   SGMisc<T>::max(s, v(1)),
                   SGMisc<T>::max(s, v(2)),
                   SGMisc<T>::max(s, v(3)));
}
template<typename S, typename T>
inline
SGVec4<T>
max(S s, const SGVec4<T>& v)
{
  return SGVec4<T>(SGMisc<T>::max(s, v(0)),
                   SGMisc<T>::max(s, v(1)),
                   SGMisc<T>::max(s, v(2)),
                   SGMisc<T>::max(s, v(3)));
}

/// Scalar dot product
template<typename T>
inline
T
dot(const SGVec4<T>& v1, const SGVec4<T>& v2)
{ return v1(0)*v2(0) + v1(1)*v2(1) + v1(2)*v2(2) + v1(3)*v2(3); }

/// The euclidean norm of the vector, that is what most people call length
template<typename T>
inline
T
norm(const SGVec4<T>& v)
{ return sqrt(dot(v, v)); }

/// The euclidean norm of the vector, that is what most people call length
template<typename T>
inline
T
length(const SGVec4<T>& v)
{ return sqrt(dot(v, v)); }

/// The 1-norm of the vector, this one is the fastest length function we
/// can implement on modern cpu's
template<typename T>
inline
T
norm1(const SGVec4<T>& v)
{ return fabs(v(0)) + fabs(v(1)) + fabs(v(2)) + fabs(v(3)); }

/// The inf-norm of the vector
template<typename T>
inline
T
normI(const SGVec4<T>& v)
{ return SGMisc<T>::max(fabs(v(0)), fabs(v(1)), fabs(v(2)), fabs(v(2))); }

/// The euclidean norm of the vector, that is what most people call length
template<typename T>
inline
SGVec4<T>
normalize(const SGVec4<T>& v)
{
  T normv = norm(v);
  if (normv <= SGLimits<T>::min())
    return SGVec4<T>::zeros();
  return (1/normv)*v;
}

/// Return true if exactly the same
template<typename T>
inline
bool
operator==(const SGVec4<T>& v1, const SGVec4<T>& v2)
{ return v1(0)==v2(0) && v1(1)==v2(1) && v1(2)==v2(2) && v1(3)==v2(3); }

/// Return true if not exactly the same
template<typename T>
inline
bool
operator!=(const SGVec4<T>& v1, const SGVec4<T>& v2)
{ return ! (v1 == v2); }

/// Return true if smaller, good for putting that into a std::map
template<typename T>
inline
bool
operator<(const SGVec4<T>& v1, const SGVec4<T>& v2)
{
  if (v1(0) < v2(0)) return true;
  else if (v2(0) < v1(0)) return false;
  else if (v1(1) < v2(1)) return true;
  else if (v2(1) < v1(1)) return false;
  else if (v1(2) < v2(2)) return true;
  else if (v2(2) < v1(2)) return false;
  else return (v1(3) < v2(3));
}

template<typename T>
inline
bool
operator<=(const SGVec4<T>& v1, const SGVec4<T>& v2)
{
  if (v1(0) < v2(0)) return true;
  else if (v2(0) < v1(0)) return false;
  else if (v1(1) < v2(1)) return true;
  else if (v2(1) < v1(1)) return false;
  else if (v1(2) < v2(2)) return true;
  else if (v2(2) < v1(2)) return false;
  else return (v1(3) <= v2(3));
}

template<typename T>
inline
bool
operator>(const SGVec4<T>& v1, const SGVec4<T>& v2)
{ return operator<(v2, v1); }

template<typename T>
inline
bool
operator>=(const SGVec4<T>& v1, const SGVec4<T>& v2)
{ return operator<=(v2, v1); }

/// Return true if equal to the relative tolerance tol
template<typename T>
inline
bool
equivalent(const SGVec4<T>& v1, const SGVec4<T>& v2, T rtol, T atol)
{ return norm1(v1 - v2) < rtol*(norm1(v1) + norm1(v2)) + atol; }

/// Return true if equal to the relative tolerance tol
template<typename T>
inline
bool
equivalent(const SGVec4<T>& v1, const SGVec4<T>& v2, T rtol)
{ return norm1(v1 - v2) < rtol*(norm1(v1) + norm1(v2)); }

/// Return true if about equal to roundoff of the underlying type
template<typename T>
inline
bool
equivalent(const SGVec4<T>& v1, const SGVec4<T>& v2)
{
  T tol = 100*SGLimits<T>::epsilon();
  return equivalent(v1, v2, tol, tol);
}

/// The euclidean distance of the two vectors
template<typename T>
inline
T
dist(const SGVec4<T>& v1, const SGVec4<T>& v2)
{ return norm(v1 - v2); }

/// The squared euclidean distance of the two vectors
template<typename T>
inline
T
distSqr(const SGVec4<T>& v1, const SGVec4<T>& v2)
{ SGVec4<T> tmp = v1 - v2; return dot(tmp, tmp); }

// calculate the projection of u along the direction of d.
template<typename T>
inline
SGVec4<T>
projection(const SGVec4<T>& u, const SGVec4<T>& d)
{
  T denom = dot(d, d);
  T ud = dot(u, d);
  if (SGLimits<T>::min() < denom) return u;
  else return d * (dot(u, d) / denom);
}

#ifndef NDEBUG
template<typename T>
inline
bool
isNaN(const SGVec4<T>& v)
{
  return SGMisc<T>::isNaN(v(0)) || SGMisc<T>::isNaN(v(1))
    || SGMisc<T>::isNaN(v(2)) || SGMisc<T>::isNaN(v(3));
}
#endif

/// Output to an ostream
template<typename char_type, typename traits_type, typename T>
inline
std::basic_ostream<char_type, traits_type>&
operator<<(std::basic_ostream<char_type, traits_type>& s, const SGVec4<T>& v)
{ return s << "[ " << v(0) << ", " << v(1) << ", " << v(2) << ", " << v(3) << " ]"; }

inline
SGVec4f
toVec4f(const SGVec4d& v)
{ return SGVec4f((float)v(0), (float)v(1), (float)v(2), (float)v(3)); }

inline
SGVec4d
toVec4d(const SGVec4f& v)
{ return SGVec4d(v(0), v(1), v(2), v(3)); }

#endif
