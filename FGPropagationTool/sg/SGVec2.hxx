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

#ifndef SGVec2_H
#define SGVec2_H

#include <iosfwd>


/// 2D Vector Class
template<typename T>
class SGVec2 {
public:
  typedef T value_type;

  /// Default constructor. Does not initialize at all.
  /// If you need them zero initialized, use SGVec2::zeros()
  SGVec2(void)
  {
    /// Initialize with nans in the debug build, that will guarantee to have
    /// a fast uninitialized default constructor in the release but shows up
    /// uninitialized values in the debug build very fast ...
#ifndef NDEBUG
    for (unsigned i = 0; i < 2; ++i)
      data()[i] = SGLimits<T>::quiet_NaN();
#endif
  }
  /// Constructor. Initialize by the given values
  SGVec2(T x, T y)
  { data()[0] = x; data()[1] = y; }
  /// Constructor. Initialize by the content of a plain array,
  /// make sure it has at least 2 elements
  explicit SGVec2(const T* d)
  { data()[0] = d[0]; data()[1] = d[1]; }
  template<typename S>
  explicit SGVec2(const SGVec2<S>& d)
  { data()[0] = d[0]; data()[1] = d[1]; }

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

  /// Access raw data
  const T (&data(void) const)[2]
  { return _data; }
  /// Access raw data
  T (&data(void))[2]
  { return _data; }

  /// Inplace addition
  SGVec2& operator+=(const SGVec2& v)
  { data()[0] += v(0); data()[1] += v(1); return *this; }
  /// Inplace subtraction
  SGVec2& operator-=(const SGVec2& v)
  { data()[0] -= v(0); data()[1] -= v(1); return *this; }
  /// Inplace scalar multiplication
  template<typename S>
  SGVec2& operator*=(S s)
  { data()[0] *= s; data()[1] *= s; return *this; }
  /// Inplace scalar multiplication by 1/s
  template<typename S>
  SGVec2& operator/=(S s)
  { return operator*=(1/T(s)); }

  /// Return an all zero vector
  static SGVec2 zeros(void)
  { return SGVec2(0, 0); }
  /// Return unit vectors
  static SGVec2 e1(void)
  { return SGVec2(1, 0); }
  static SGVec2 e2(void)
  { return SGVec2(0, 1); }

private:
  T _data[2];
};

/// Unary +, do nothing ...
template<typename T>
inline
const SGVec2<T>&
operator+(const SGVec2<T>& v)
{ return v; }

/// Unary -, do nearly nothing
template<typename T>
inline
SGVec2<T>
operator-(const SGVec2<T>& v)
{ return SGVec2<T>(-v(0), -v(1)); }

/// Binary +
template<typename T>
inline
SGVec2<T>
operator+(const SGVec2<T>& v1, const SGVec2<T>& v2)
{ return SGVec2<T>(v1(0)+v2(0), v1(1)+v2(1)); }

/// Binary -
template<typename T>
inline
SGVec2<T>
operator-(const SGVec2<T>& v1, const SGVec2<T>& v2)
{ return SGVec2<T>(v1(0)-v2(0), v1(1)-v2(1)); }

/// Scalar multiplication
template<typename S, typename T>
inline
SGVec2<T>
operator*(S s, const SGVec2<T>& v)
{ return SGVec2<T>(s*v(0), s*v(1)); }

/// Scalar multiplication
template<typename S, typename T>
inline
SGVec2<T>
operator*(const SGVec2<T>& v, S s)
{ return SGVec2<T>(s*v(0), s*v(1)); }

/// multiplication as a multiplicator, that is assume that the first vector
/// represents a 2x2 diagonal matrix with the diagonal elements in the vector.
/// Then the result is the product of that matrix times the second vector.
template<typename T>
inline
SGVec2<T>
mult(const SGVec2<T>& v1, const SGVec2<T>& v2)
{ return SGVec2<T>(v1(0)*v2(0), v1(1)*v2(1)); }

/// component wise min
template<typename T>
inline
SGVec2<T>
min(const SGVec2<T>& v1, const SGVec2<T>& v2)
{return SGVec2<T>(SGMisc<T>::min(v1(0), v2(0)), SGMisc<T>::min(v1(1), v2(1)));}
template<typename S, typename T>
inline
SGVec2<T>
min(const SGVec2<T>& v, S s)
{ return SGVec2<T>(SGMisc<T>::min(s, v(0)), SGMisc<T>::min(s, v(1))); }
template<typename S, typename T>
inline
SGVec2<T>
min(S s, const SGVec2<T>& v)
{ return SGVec2<T>(SGMisc<T>::min(s, v(0)), SGMisc<T>::min(s, v(1))); }

/// component wise max
template<typename T>
inline
SGVec2<T>
max(const SGVec2<T>& v1, const SGVec2<T>& v2)
{return SGVec2<T>(SGMisc<T>::max(v1(0), v2(0)), SGMisc<T>::max(v1(1), v2(1)));}
template<typename S, typename T>
inline
SGVec2<T>
max(const SGVec2<T>& v, S s)
{ return SGVec2<T>(SGMisc<T>::max(s, v(0)), SGMisc<T>::max(s, v(1))); }
template<typename S, typename T>
inline
SGVec2<T>
max(S s, const SGVec2<T>& v)
{ return SGVec2<T>(SGMisc<T>::max(s, v(0)), SGMisc<T>::max(s, v(1))); }

/// Scalar dot product
template<typename T>
inline
T
dot(const SGVec2<T>& v1, const SGVec2<T>& v2)
{ return v1(0)*v2(0) + v1(1)*v2(1); }

/// The euclidean norm of the vector, that is what most people call length
template<typename T>
inline
T
norm(const SGVec2<T>& v)
{ return sqrt(dot(v, v)); }

/// The euclidean norm of the vector, that is what most people call length
template<typename T>
inline
T
length(const SGVec2<T>& v)
{ return sqrt(dot(v, v)); }

/// The 1-norm of the vector, this one is the fastest length function we
/// can implement on modern cpu's
template<typename T>
inline
T
norm1(const SGVec2<T>& v)
{ return fabs(v(0)) + fabs(v(1)); }

/// The inf-norm of the vector
template<typename T>
inline
T
normI(const SGVec2<T>& v)
{ return SGMisc<T>::max(fabs(v(0)), fabs(v(1))); }

/// The euclidean norm of the vector, that is what most people call length
template<typename T>
inline
SGVec2<T>
normalize(const SGVec2<T>& v)
{
  T normv = norm(v);
  if (normv <= SGLimits<T>::min())
    return SGVec2<T>::zeros();
  return (1/normv)*v;
}

/// Return true if exactly the same
template<typename T>
inline
bool
operator==(const SGVec2<T>& v1, const SGVec2<T>& v2)
{ return v1(0) == v2(0) && v1(1) == v2(1); }

/// Return true if not exactly the same
template<typename T>
inline
bool
operator!=(const SGVec2<T>& v1, const SGVec2<T>& v2)
{ return ! (v1 == v2); }

/// Return true if smaller, good for putting that into a std::map
template<typename T>
inline
bool
operator<(const SGVec2<T>& v1, const SGVec2<T>& v2)
{
  if (v1(0) < v2(0)) return true;
  else if (v2(0) < v1(0)) return false;
  else return (v1(1) < v2(1));
}

template<typename T>
inline
bool
operator<=(const SGVec2<T>& v1, const SGVec2<T>& v2)
{
  if (v1(0) < v2(0)) return true;
  else if (v2(0) < v1(0)) return false;
  else return (v1(1) <= v2(1));
}

template<typename T>
inline
bool
operator>(const SGVec2<T>& v1, const SGVec2<T>& v2)
{ return operator<(v2, v1); }

template<typename T>
inline
bool
operator>=(const SGVec2<T>& v1, const SGVec2<T>& v2)
{ return operator<=(v2, v1); }

/// Return true if equal to the relative tolerance tol
template<typename T>
inline
bool
equivalent(const SGVec2<T>& v1, const SGVec2<T>& v2, T rtol, T atol)
{ return norm1(v1 - v2) < rtol*(norm1(v1) + norm1(v2)) + atol; }

/// Return true if equal to the relative tolerance tol
template<typename T>
inline
bool
equivalent(const SGVec2<T>& v1, const SGVec2<T>& v2, T rtol)
{ return norm1(v1 - v2) < rtol*(norm1(v1) + norm1(v2)); }

/// Return true if about equal to roundoff of the underlying type
template<typename T>
inline
bool
equivalent(const SGVec2<T>& v1, const SGVec2<T>& v2)
{
  T tol = 100*SGLimits<T>::epsilon();
  return equivalent(v1, v2, tol, tol);
}

/// The euclidean distance of the two vectors
template<typename T>
inline
T
dist(const SGVec2<T>& v1, const SGVec2<T>& v2)
{ return norm(v1 - v2); }

/// The squared euclidean distance of the two vectors
template<typename T>
inline
T
distSqr(const SGVec2<T>& v1, const SGVec2<T>& v2)
{ SGVec2<T> tmp = v1 - v2; return dot(tmp, tmp); }

// calculate the projection of u along the direction of d.
template<typename T>
inline
SGVec2<T>
projection(const SGVec2<T>& u, const SGVec2<T>& d)
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
isNaN(const SGVec2<T>& v)
{
  return SGMisc<T>::isNaN(v(0)) || SGMisc<T>::isNaN(v(1));
}
#endif

/// Output to an ostream
template<typename char_type, typename traits_type, typename T>
inline
std::basic_ostream<char_type, traits_type>&
operator<<(std::basic_ostream<char_type, traits_type>& s, const SGVec2<T>& v)
{ return s << "[ " << v(0) << ", " << v(1) << " ]"; }


inline
SGVec2f
toVec2f(const SGVec2d& v)
{ return SGVec2f((float)v(0), (float)v(1)); }

inline
SGVec2d
toVec2d(const SGVec2f& v)
{ return SGVec2d(v(0), v(1)); }


#endif
