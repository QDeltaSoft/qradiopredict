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

#ifndef SGMatrix_H
#define SGMatrix_H

/// Expression templates for poor programmers ... :)
template<typename T>
struct TransNegRef;

/// 3D Matrix Class
template<typename T>
class SGMatrix {
public:
  enum { nCols = 4, nRows = 4, nEnts = 16 };
  typedef T value_type;

  /// Default constructor. Does not initialize at all.
  /// If you need them zero initialized, use SGMatrix::zeros()
  SGMatrix(void)
  {
    /// Initialize with nans in the debug build, that will guarantee to have
    /// a fast uninitialized default constructor in the release but shows up
    /// uninitialized values in the debug build very fast ...
#ifndef NDEBUG
    for (unsigned i = 0; i < nEnts; ++i)
      _data.flat[i] = SGLimits<T>::quiet_NaN();
#endif
  }
  /// Constructor. Initialize by the content of a plain array,
  /// make sure it has at least 16 elements
  explicit SGMatrix(const T* data)
  { for (unsigned i = 0; i < nEnts; ++i) _data.flat[i] = data[i]; }

  /// Constructor, build up a SGMatrix from given elements
  SGMatrix(T m00, T m01, T m02, T m03,
           T m10, T m11, T m12, T m13,
           T m20, T m21, T m22, T m23,
           T m30, T m31, T m32, T m33)
  {
    _data.flat[0] = m00; _data.flat[1] = m10;
    _data.flat[2] = m20; _data.flat[3] = m30;
    _data.flat[4] = m01; _data.flat[5] = m11;
    _data.flat[6] = m21; _data.flat[7] = m31;
    _data.flat[8] = m02; _data.flat[9] = m12;
    _data.flat[10] = m22; _data.flat[11] = m32;
    _data.flat[12] = m03; _data.flat[13] = m13;
    _data.flat[14] = m23; _data.flat[15] = m33;
  }

  /// Constructor, build up a SGMatrix from a translation
  template<typename S>
  SGMatrix(const SGVec3<S>& trans)
  { set(trans); }

  /// Constructor, build up a SGMatrix from a rotation and a translation
  template<typename S>
  SGMatrix(const SGQuat<S>& quat)
  { set(quat); }

  /// Copy constructor for a transposed negated matrix
  SGMatrix(const TransNegRef<T>& tm)
  { set(tm); }

  /// Set from a tranlation
  template<typename S>
  void set(const SGVec3<S>& trans)
  {
    _data.flat[0] = 1; _data.flat[4] = 0;
    _data.flat[8] = 0; _data.flat[12] = T(trans(0));
    _data.flat[1] = 0; _data.flat[5] = 1;
    _data.flat[9] = 0; _data.flat[13] = T(trans(1));
    _data.flat[2] = 0; _data.flat[6] = 0;
    _data.flat[10] = 1; _data.flat[14] = T(trans(2));
    _data.flat[3] = 0; _data.flat[7] = 0;
    _data.flat[11] = 0; _data.flat[15] = 1;
  }

  /// Set from a scale/rotation and tranlation
  template<typename S>
  void set(const SGQuat<S>& quat)
  {
    T w = quat.w(); T x = quat.x(); T y = quat.y(); T z = quat.z();
    T xx = x*x; T yy = y*y; T zz = z*z;
    T wx = w*x; T wy = w*y; T wz = w*z;
    T xy = x*y; T xz = x*z; T yz = y*z;
    _data.flat[0] = 1-2*(yy+zz); _data.flat[1] = 2*(xy-wz);
    _data.flat[2] = 2*(xz+wy); _data.flat[3] = 0;
    _data.flat[4] = 2*(xy+wz); _data.flat[5] = 1-2*(xx+zz);
    _data.flat[6] = 2*(yz-wx); _data.flat[7] = 0;
    _data.flat[8] = 2*(xz-wy); _data.flat[9] = 2*(yz+wx);
    _data.flat[10] = 1-2*(xx+yy); _data.flat[11] = 0;
    _data.flat[12] = 0; _data.flat[13] = 0;
    _data.flat[14] = 0; _data.flat[15] = 1;
  }

  /// set from a transposed negated matrix
  void set(const TransNegRef<T>& tm)
  {
    const SGMatrix& m = tm.m;
    _data.flat[0] = m(0,0);
    _data.flat[1] = m(0,1);
    _data.flat[2] = m(0,2);
    _data.flat[3] = m(3,0);

    _data.flat[4] = m(1,0);
    _data.flat[5] = m(1,1);
    _data.flat[6] = m(1,2);
    _data.flat[7] = m(3,1);

    _data.flat[8] = m(2,0);
    _data.flat[9] = m(2,1);
    _data.flat[10] = m(2,2);
    _data.flat[11] = m(3,2);

    // Well, this one is ugly here, as that xform method on the current
    // object needs the above data to be already set ...
    SGVec3<T> t = xformVec(SGVec3<T>(m(0,3), m(1,3), m(2,3)));
    _data.flat[12] = -t(0);
    _data.flat[13] = -t(1);
    _data.flat[14] = -t(2);
    _data.flat[15] = m(3,3);
  }

  /// Access by index, the index is unchecked
  const T& operator()(unsigned i, unsigned j) const
  { return _data.flat[i + 4*j]; }
  /// Access by index, the index is unchecked
  T& operator()(unsigned i, unsigned j)
  { return _data.flat[i + 4*j]; }

  /// Access raw data by index, the index is unchecked
  const T& operator[](unsigned i) const
  { return _data.flat[i]; }
  /// Access by index, the index is unchecked
  T& operator[](unsigned i)
  { return _data.flat[i]; }

  /// Get the data pointer
  const T* data(void) const
  { return _data.flat; }
  /// Get the data pointer
  T* data(void)
  { return _data.flat; }

  /// Readonly interface function to ssg's sgMat4/sgdMat4
  const T (&sg(void) const)[4][4]
  { return _data.carray; }
  /// Interface function to ssg's sgMat4/sgdMat4
  T (&sg(void))[4][4]
  { return _data.carray; }

  /// Inplace addition
  SGMatrix& operator+=(const SGMatrix& m)
  { for (unsigned i = 0; i < nEnts; ++i) _data.flat[i] += m._data.flat[i]; return *this; }
  /// Inplace subtraction
  SGMatrix& operator-=(const SGMatrix& m)
  { for (unsigned i = 0; i < nEnts; ++i) _data.flat[i] -= m._data.flat[i]; return *this; }
  /// Inplace scalar multiplication
  template<typename S>
  SGMatrix& operator*=(S s)
  { for (unsigned i = 0; i < nEnts; ++i) _data.flat[i] *= s; return *this; }
  /// Inplace scalar multiplication by 1/s
  template<typename S>
  SGMatrix& operator/=(S s)
  { return operator*=(1/T(s)); }
  /// Inplace matrix multiplication, post multiply
  SGMatrix& operator*=(const SGMatrix<T>& m2);

  template<typename S>
  SGMatrix& preMultTranslate(const SGVec3<S>& t)
  {
    for (unsigned i = 0; i < 3; ++i) {
      T tmp = T(t(i));
      if (tmp == 0)
        continue;
      (*this)(i,0) += tmp*(*this)(3,0);
      (*this)(i,1) += tmp*(*this)(3,1);
      (*this)(i,2) += tmp*(*this)(3,2);
      (*this)(i,3) += tmp*(*this)(3,3);
    }
    return *this;
  }
  template<typename S>
  SGMatrix& postMultTranslate(const SGVec3<S>& t)
  {
    SGVec4<T> col3((*this)(0,3), (*this)(1,3), (*this)(2,3), (*this)(3,3));
    for (unsigned i = 0; i < SGMatrix<T>::nCols-1; ++i) {
      SGVec4<T> tmp((*this)(0,i), (*this)(1,i), (*this)(2,i), (*this)(3,i));
      col3 += T(t(i))*tmp;
    }
    (*this)(0,3) = col3(0); (*this)(1,3) = col3(1);
    (*this)(2,3) = col3(2); (*this)(3,3) = col3(3);
    return *this;
  }

  SGMatrix& preMultRotate(const SGQuat<T>& r)
  {
    for (unsigned i = 0; i < SGMatrix<T>::nCols; ++i) {
      SGVec3<T> col((*this)(0,i), (*this)(1,i), (*this)(2,i));
      col = r.transform(col);
      (*this)(0,i) = col(0); (*this)(1,i) = col(1); (*this)(2,i) = col(2);
    }
    return *this;
  }
  SGMatrix& postMultRotate(const SGQuat<T>& r)
  {
    for (unsigned i = 0; i < SGMatrix<T>::nCols; ++i) {
      SGVec3<T> col((*this)(i,0), (*this)(i,1), (*this)(i,2));
      col = r.backTransform(col);
      (*this)(i,0) = col(0); (*this)(i,1) = col(1); (*this)(i,2) = col(2);
    }
    return *this;
  }

  SGVec3<T> xformPt(const SGVec3<T>& pt) const
  {
    SGVec3<T> tpt;
    tpt(0) = (*this)(0,3);
    tpt(1) = (*this)(1,3);
    tpt(2) = (*this)(2,3);
    for (unsigned i = 0; i < SGMatrix<T>::nCols-1; ++i) {
      T tmp = pt(i);
      tpt(0) += tmp*(*this)(0,i);
      tpt(1) += tmp*(*this)(1,i);
      tpt(2) += tmp*(*this)(2,i);
    }
    return tpt;
  }
  SGVec3<T> xformVec(const SGVec3<T>& v) const
  {
    SGVec3<T> tv;
    T tmp = v(0);
    tv(0) = tmp*(*this)(0,0);
    tv(1) = tmp*(*this)(1,0);
    tv(2) = tmp*(*this)(2,0);
    for (unsigned i = 1; i < SGMatrix<T>::nCols-1; ++i) {
      T tmp = v(i);
      tv(0) += tmp*(*this)(0,i);
      tv(1) += tmp*(*this)(1,i);
      tv(2) += tmp*(*this)(2,i);
    }
    return tv;
  }

  /// Return an all zero matrix
  static SGMatrix zeros(void)
  { return SGMatrix(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0); }

  /// Return a unit matrix
  static SGMatrix unit(void)
  { return SGMatrix(1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1); }

private:
  /// Required to make that alias safe.
  union Data {
    T flat[16];
    T carray[4][4];
  };

  /// The actual data, the matrix is stored in column major order,
  /// that matches the storage format of OpenGL
  Data _data;
};

/// Class to distinguish between a matrix and the matrix with a transposed
/// rotational part and a negated translational part
template<typename T>
struct TransNegRef {
  TransNegRef(const SGMatrix<T>& _m) : m(_m) {}
  const SGMatrix<T>& m;
};

/// Unary +, do nothing ...
template<typename T>
inline
const SGMatrix<T>&
operator+(const SGMatrix<T>& m)
{ return m; }

/// Unary -, do nearly nothing
template<typename T>
inline
SGMatrix<T>
operator-(const SGMatrix<T>& m)
{
  SGMatrix<T> ret;
  for (unsigned i = 0; i < SGMatrix<T>::nEnts; ++i)
    ret[i] = -m[i];
  return ret;
}

/// Binary +
template<typename T>
inline
SGMatrix<T>
operator+(const SGMatrix<T>& m1, const SGMatrix<T>& m2)
{
  SGMatrix<T> ret;
  for (unsigned i = 0; i < SGMatrix<T>::nEnts; ++i)
    ret[i] = m1[i] + m2[i];
  return ret;
}

/// Binary -
template<typename T>
inline
SGMatrix<T>
operator-(const SGMatrix<T>& m1, const SGMatrix<T>& m2)
{
  SGMatrix<T> ret;
  for (unsigned i = 0; i < SGMatrix<T>::nEnts; ++i)
    ret[i] = m1[i] - m2[i];
  return ret;
}

/// Scalar multiplication
template<typename S, typename T>
inline
SGMatrix<T>
operator*(S s, const SGMatrix<T>& m)
{
  SGMatrix<T> ret;
  for (unsigned i = 0; i < SGMatrix<T>::nEnts; ++i)
    ret[i] = s*m[i];
  return ret;
}

/// Scalar multiplication
template<typename S, typename T>
inline
SGMatrix<T>
operator*(const SGMatrix<T>& m, S s)
{
  SGMatrix<T> ret;
  for (unsigned i = 0; i < SGMatrix<T>::nEnts; ++i)
    ret[i] = s*m[i];
  return ret;
}

/// Vector multiplication
template<typename T>
inline
SGVec4<T>
operator*(const SGMatrix<T>& m, const SGVec4<T>& v)
{
  SGVec4<T> mv;
  T tmp = v(0);
  mv(0) = tmp*m(0,0);
  mv(1) = tmp*m(1,0);
  mv(2) = tmp*m(2,0);
  mv(3) = tmp*m(3,0);
  for (unsigned i = 1; i < SGMatrix<T>::nCols; ++i) {
    T tmp = v(i);
    mv(0) += tmp*m(0,i);
    mv(1) += tmp*m(1,i);
    mv(2) += tmp*m(2,i);
    mv(3) += tmp*m(3,i);
  }
  return mv;
}

/// Vector multiplication
template<typename T>
inline
SGVec4<T>
operator*(const TransNegRef<T>& tm, const SGVec4<T>& v)
{
  const SGMatrix<T>& m = tm.m;
  SGVec4<T> mv;
  SGVec3<T> v2;
  T tmp = v(3);
  mv(0) = v2(0) = -tmp*m(0,3);
  mv(1) = v2(1) = -tmp*m(1,3);
  mv(2) = v2(2) = -tmp*m(2,3);
  mv(3) = tmp*m(3,3);
  for (unsigned i = 0; i < SGMatrix<T>::nCols - 1; ++i) {
    T tmp = v(i) + v2(i);
    mv(0) += tmp*m(i,0);
    mv(1) += tmp*m(i,1);
    mv(2) += tmp*m(i,2);
    mv(3) += tmp*m(3,i);
  }
  return mv;
}

/// Matrix multiplication
template<typename T>
inline
SGMatrix<T>
operator*(const SGMatrix<T>& m1, const SGMatrix<T>& m2)
{
  SGMatrix<T> m;
  for (unsigned j = 0; j < SGMatrix<T>::nCols; ++j) {
    T tmp = m2(0,j);
    m(0,j) = tmp*m1(0,0);
    m(1,j) = tmp*m1(1,0);
    m(2,j) = tmp*m1(2,0);
    m(3,j) = tmp*m1(3,0);
    for (unsigned i = 1; i < SGMatrix<T>::nCols; ++i) {
      T tmp = m2(i,j);
      m(0,j) += tmp*m1(0,i);
      m(1,j) += tmp*m1(1,i);
      m(2,j) += tmp*m1(2,i);
      m(3,j) += tmp*m1(3,i);
    }
  }
  return m;
}

/// Inplace matrix multiplication, post multiply
template<typename T>
inline
SGMatrix<T>&
SGMatrix<T>::operator*=(const SGMatrix<T>& m2)
{ (*this) = operator*(*this, m2); return *this; }

/// Return a reference to the matrix typed to make sure we use the transposed
/// negated matrix
template<typename T>
inline
TransNegRef<T>
transNeg(const SGMatrix<T>& m)
{ return TransNegRef<T>(m); }

/// Compute the inverse if the matrix src. Store the result in dst.
/// Return if the matrix is nonsingular. If it is singular dst contains
/// undefined values
template<typename T>
inline
bool
invert(SGMatrix<T>& dst, const SGMatrix<T>& src)
{
  // Do a LU decomposition with row pivoting and solve into dst
  SGMatrix<T> tmp = src;
  dst = SGMatrix<T>::unit();

  for (unsigned i = 0; i < 4; ++i) {
    T val = tmp(i,i);
    unsigned ind = i;

    // Find the row with the maximum value in the i-th colum
    for (unsigned j = i + 1; j < 4; ++j) {
      if (fabs(tmp(j, i)) > fabs(val)) {
        ind = j;
        val = tmp(j, i);
      }
    }

    // Do row pivoting
    if (ind != i) {
      for (unsigned j = 0; j < 4; ++j) {
        T t;
        t = dst(i,j); dst(i,j) = dst(ind,j); dst(ind,j) = t;
        t = tmp(i,j); tmp(i,j) = tmp(ind,j); tmp(ind,j) = t;
      }
    }

    // Check for singularity
    if (fabs(val) <= SGLimits<T>::min())
      return false;

    T ival = 1/val;
    for (unsigned j = 0; j < 4; ++j) {
      tmp(i,j) *= ival;
      dst(i,j) *= ival;
    }

    for (unsigned j = 0; j < 4; ++j) {
      if (j == i)
        continue;

      val = tmp(j,i);
      for (unsigned k = 0; k < 4; ++k) {
        tmp(j,k) -= tmp(i,k) * val;
        dst(j,k) -= dst(i,k) * val;
      }
    }
  }
  return true;
}

/// The 1-norm of the matrix, this is the largest column sum of
/// the absolute values of A.
template<typename T>
inline
T
norm1(const SGMatrix<T>& m)
{
  T nrm = 0;
  for (unsigned i = 0; i < SGMatrix<T>::nRows; ++i) {
    T sum = fabs(m(i, 0)) + fabs(m(i, 1)) + fabs(m(i, 2)) + fabs(m(i, 3));
    if (nrm < sum)
      nrm = sum;
  }
  return nrm;
}

/// The inf-norm of the matrix, this is the largest row sum of
/// the absolute values of A.
template<typename T>
inline
T
normInf(const SGMatrix<T>& m)
{
  T nrm = 0;
  for (unsigned i = 0; i < SGMatrix<T>::nCols; ++i) {
    T sum = fabs(m(0, i)) + fabs(m(1, i)) + fabs(m(2, i)) + fabs(m(3, i));
    if (nrm < sum)
      nrm = sum;
  }
  return nrm;
}

/// Return true if exactly the same
template<typename T>
inline
bool
operator==(const SGMatrix<T>& m1, const SGMatrix<T>& m2)
{
  for (unsigned i = 0; i < SGMatrix<T>::nEnts; ++i)
    if (m1[i] != m2[i])
      return false;
  return true;
}

/// Return true if not exactly the same
template<typename T>
inline
bool
operator!=(const SGMatrix<T>& m1, const SGMatrix<T>& m2)
{ return ! (m1 == m2); }

/// Return true if equal to the relative tolerance tol
template<typename T>
inline
bool
equivalent(const SGMatrix<T>& m1, const SGMatrix<T>& m2, T rtol, T atol)
{ return norm1(m1 - m2) < rtol*(norm1(m1) + norm1(m2)) + atol; }

/// Return true if equal to the relative tolerance tol
template<typename T>
inline
bool
equivalent(const SGMatrix<T>& m1, const SGMatrix<T>& m2, T rtol)
{ return norm1(m1 - m2) < rtol*(norm1(m1) + norm1(m2)); }

/// Return true if about equal to roundoff of the underlying type
template<typename T>
inline
bool
equivalent(const SGMatrix<T>& m1, const SGMatrix<T>& m2)
{
  T tol = 100*SGLimits<T>::epsilon();
  return equivalent(m1, m2, tol, tol);
}

#ifndef NDEBUG
template<typename T>
inline
bool
isNaN(const SGMatrix<T>& m)
{
  for (unsigned i = 0; i < SGMatrix<T>::nEnts; ++i) {
    if (SGMisc<T>::isNaN(m[i]))
      return true;
  }
  return false;
}
#endif

/// Output to an ostream
template<typename char_type, typename traits_type, typename T> 
inline
std::basic_ostream<char_type, traits_type>&
operator<<(std::basic_ostream<char_type, traits_type>& s, const SGMatrix<T>& m)
{
  s << "[ " << m(0,0) << ", " << m(0,1) << ", " << m(0,2) << ", " << m(0,3) << "\n";
  s << "  " << m(1,0) << ", " << m(1,1) << ", " << m(1,2) << ", " << m(1,3) << "\n";
  s << "  " << m(2,0) << ", " << m(2,1) << ", " << m(2,2) << ", " << m(2,3) << "\n";
  s << "  " << m(3,0) << ", " << m(3,1) << ", " << m(3,2) << ", " << m(3,3) << " ]";
  return s;
}

inline
SGMatrixf
toMatrixf(const SGMatrixd& m)
{
  return SGMatrixf((float)m(0,0), (float)m(0,1), (float)m(0,2), (float)m(0,3),
                   (float)m(1,0), (float)m(1,1), (float)m(1,2), (float)m(1,3),
                   (float)m(2,0), (float)m(2,1), (float)m(2,2), (float)m(2,3),
                   (float)m(3,0), (float)m(3,1), (float)m(3,2), (float)m(3,3));
}

inline
SGMatrixd
toMatrixd(const SGMatrixf& m)
{
  return SGMatrixd(m(0,0), m(0,1), m(0,2), m(0,3),
                   m(1,0), m(1,1), m(1,2), m(1,3),
                   m(2,0), m(2,1), m(2,2), m(2,3),
                   m(3,0), m(3,1), m(3,2), m(3,3));
}

#endif
