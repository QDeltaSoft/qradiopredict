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

#ifndef SGQuat_H
#define SGQuat_H


#ifdef min
#undef min
#endif

#ifdef max
#undef max
#endif

/// Quaternion Class
template<typename T>
class SGQuat {
public:
  typedef T value_type;

  /// Default constructor. Does not initialize at all.
  /// If you need them zero initialized, SGQuat::zeros()
  SGQuat(void)
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
  SGQuat(T _x, T _y, T _z, T _w)
  { x() = _x; y() = _y; z() = _z; w() = _w; }
  /// Constructor. Initialize by the content of a plain array,
  /// make sure it has at least 4 elements
  explicit SGQuat(const T* d)
  { data()[0] = d[0]; data()[1] = d[1]; data()[2] = d[2]; data()[3] = d[3]; }

  /// Return a unit quaternion
  static SGQuat unit(void)
  { return fromRealImag(1, SGVec3<T>(0, 0, 0)); }

  /// Return a quaternion from euler angles
  static SGQuat fromEulerRad(T z, T y, T x)
  {
    SGQuat q;
    T zd2 = T(0.5)*z; T yd2 = T(0.5)*y; T xd2 = T(0.5)*x;
    T Szd2 = sin(zd2); T Syd2 = sin(yd2); T Sxd2 = sin(xd2);
    T Czd2 = cos(zd2); T Cyd2 = cos(yd2); T Cxd2 = cos(xd2);
    T Cxd2Czd2 = Cxd2*Czd2; T Cxd2Szd2 = Cxd2*Szd2;
    T Sxd2Szd2 = Sxd2*Szd2; T Sxd2Czd2 = Sxd2*Czd2;
    q.w() = Cxd2Czd2*Cyd2 + Sxd2Szd2*Syd2;
    q.x() = Sxd2Czd2*Cyd2 - Cxd2Szd2*Syd2;
    q.y() = Cxd2Czd2*Syd2 + Sxd2Szd2*Cyd2;
    q.z() = Cxd2Szd2*Cyd2 - Sxd2Czd2*Syd2;
    return q;
  }

  /// Return a quaternion from euler angles in degrees
  static SGQuat fromEulerDeg(T z, T y, T x)
  {
    return fromEulerRad(SGMisc<T>::deg2rad(z), SGMisc<T>::deg2rad(y),
                        SGMisc<T>::deg2rad(x));
  }

  /// Return a quaternion from euler angles
  static SGQuat fromYawPitchRoll(T y, T p, T r)
  { return fromEulerRad(y, p, r); }

  /// Return a quaternion from euler angles
  static SGQuat fromYawPitchRollDeg(T y, T p, T r)
  { return fromEulerDeg(y, p, r); }

  /// Return a quaternion from euler angles
  static SGQuat fromHeadAttBank(T h, T a, T b)
  { return fromEulerRad(h, a, b); }

  /// Return a quaternion from euler angles
  static SGQuat fromHeadAttBankDeg(T h, T a, T b)
  { return fromEulerDeg(h, a, b); }

  /// Return a quaternion rotation from the earth centered to the
  /// simulation usual horizontal local frame from given
  /// longitude and latitude.
  /// The horizontal local frame used in simulations is the frame with x-axis
  /// pointing north, the y-axis pointing eastwards and the z axis
  /// pointing downwards.
  static SGQuat fromLonLatRad(T lon, T lat)
  {
    SGQuat q;
    T zd2 = T(0.5)*lon;
    T yd2 = T(-0.25)*SGMisc<T>::pi() - T(0.5)*lat;
    T Szd2 = sin(zd2);
    T Syd2 = sin(yd2);
    T Czd2 = cos(zd2);
    T Cyd2 = cos(yd2);
    q.w() = Czd2*Cyd2;
    q.x() = -Szd2*Syd2;
    q.y() = Czd2*Syd2;
    q.z() = Szd2*Cyd2;
    return q;
  }
  /// Like the above provided for convenience
  static SGQuat fromLonLatDeg(T lon, T lat)
  { return fromLonLatRad(SGMisc<T>::deg2rad(lon), SGMisc<T>::deg2rad(lat)); }
  /// Like the above provided for convenience
  static SGQuat fromLonLat(const SGGeod& geod)
  { return fromLonLatRad(geod.getLongitudeRad(), geod.getLatitudeRad()); }


  /// Create a quaternion from the angle axis representation
  static SGQuat fromAngleAxis(T angle, const SGVec3<T>& axis)
  {
    T angle2 = T(0.5)*angle;
    return fromRealImag(cos(angle2), T(sin(angle2))*axis);
  }

  /// Create a quaternion from the angle axis representation
  static SGQuat fromAngleAxisDeg(T angle, const SGVec3<T>& axis)
  { return fromAngleAxis(SGMisc<T>::deg2rad(angle), axis); }

  /// Create a quaternion from the angle axis representation where the angle
  /// is stored in the axis' length
  static SGQuat fromAngleAxis(const SGVec3<T>& axis)
  {
    T nAxis = norm(axis);
    if (nAxis <= SGLimits<T>::min())
      return SGQuat::unit();
    T angle2 = T(0.5)*nAxis;
    return fromRealImag(cos(angle2), T(sin(angle2)/nAxis)*axis);
  }

  /// Create a normalized quaternion just from the imaginary part.
  /// The imaginary part should point into that axis direction that results in
  /// a quaternion with a positive real part.
  /// This is the smallest numerically stable representation of an orientation
  /// in space. See getPositiveRealImag()
  static SGQuat fromPositiveRealImag(const SGVec3<T>& imag)
  {
    T r = sqrt(SGMisc<T>::max(T(0), T(1) - dot(imag, imag)));
    return fromRealImag(r, imag);
  }

  /// Return a quaternion that rotates the from vector onto the to vector.
  static SGQuat fromRotateTo(const SGVec3<T>& from, const SGVec3<T>& to)
  {
    T nfrom = norm(from);
    T nto = norm(to);
    if (nfrom <= SGLimits<T>::min() || nto <= SGLimits<T>::min())
      return SGQuat::unit();

    return SGQuat::fromRotateToNorm((1/nfrom)*from, (1/nto)*to);
  }

  /// Return a quaternion that rotates v1 onto the i1-th unit vector
  /// and v2 into a plane that is spanned by the i2-th and i1-th unit vector.
  static SGQuat fromRotateTo(const SGVec3<T>& v1, unsigned i1,
                             const SGVec3<T>& v2, unsigned i2)
  {
    T nrmv1 = norm(v1);
    T nrmv2 = norm(v2);
    if (nrmv1 <= SGLimits<T>::min() || nrmv2 <= SGLimits<T>::min())
      return SGQuat::unit();

    SGVec3<T> nv1 = (1/nrmv1)*v1;
    SGVec3<T> nv2 = (1/nrmv2)*v2;
    T dv1v2 = dot(nv1, nv2);
    if (fabs(fabs(dv1v2)-1) <= SGLimits<T>::epsilon())
      return SGQuat::unit();

    // The target vector for the first rotation
    SGVec3<T> nto1 = SGVec3<T>::zeros();
    SGVec3<T> nto2 = SGVec3<T>::zeros();
    nto1[i1] = 1;
    nto2[i2] = 1;

    // The first rotation can be done with the usual routine.
    SGQuat q = SGQuat::fromRotateToNorm(nv1, nto1);

    // The rotation axis for the second rotation is the
    // target for the first one, so the rotation axis is nto1
    // We need to get the angle.

    // Make nv2 exactly orthogonal to nv1.
    nv2 = normalize(nv2 - dv1v2*nv1);

    SGVec3<T> tnv2 = q.transform(nv2);
    T cosang = dot(nto2, tnv2);
    T cos05ang = T(0.5)+T(0.5)*cosang;
    if (cos05ang <= 0)
      cosang = 0;
    cos05ang = sqrt(cos05ang);
    T sig = dot(nto1, cross(nto2, tnv2));
    T sin05ang = T(0.5)-T(0.5)*cosang;
    if (sin05ang <= 0)
      sin05ang = 0;
    sin05ang = copysign(sqrt(sin05ang), sig);
    q *= SGQuat::fromRealImag(cos05ang, sin05ang*nto1);

    return q;
  }


  // Return a quaternion which rotates the vector given by v
  // to the vector -v. Other directions are *not* preserved.
  static SGQuat fromChangeSign(const SGVec3<T>& v)
  {
    // The vector from points to the oposite direction than to.
    // Find a vector perpendicular to the vector to.
    T absv1 = fabs(v(0));
    T absv2 = fabs(v(1));
    T absv3 = fabs(v(2));

    SGVec3<T> axis;
    if (absv2 < absv1 && absv3 < absv1) {
      T quot = v(1)/v(0);
      axis = (1/sqrt(1+quot*quot))*SGVec3<T>(quot, -1, 0);
    } else if (absv1 < absv2 && absv3 < absv2) {
      T quot = v(2)/v(1);
      axis = (1/sqrt(1+quot*quot))*SGVec3<T>(0, quot, -1);
    } else if (absv1 < absv3 && absv2 < absv3) {
      T quot = v(0)/v(2);
      axis = (1/sqrt(1+quot*quot))*SGVec3<T>(-1, 0, quot);
    } else {
      // The all zero case.
      return SGQuat::unit();
    }

    return SGQuat::fromRealImag(0, axis);
  }

  /// Return a quaternion from real and imaginary part
  static SGQuat fromRealImag(T r, const SGVec3<T>& i)
  {
    SGQuat q;
    q.w() = r;
    q.x() = i.x();
    q.y() = i.y();
    q.z() = i.z();
    return q;
  }

  /// Return an all zero vector
  static SGQuat zeros(void)
  { return SGQuat(0, 0, 0, 0); }

  /// write the euler angles into the references
  void getEulerRad(T& zRad, T& yRad, T& xRad) const
  {
    T sqrQW = w()*w();
    T sqrQX = x()*x();
    T sqrQY = y()*y();
    T sqrQZ = z()*z();

    T num = 2*(y()*z() + w()*x());
    T den = sqrQW - sqrQX - sqrQY + sqrQZ;
    if (fabs(den) <= SGLimits<T>::min() &&
        fabs(num) <= SGLimits<T>::min())
      xRad = 0;
    else
      xRad = atan2(num, den);

    T tmp = 2*(x()*z() - w()*y());
    if (tmp <= -1)
      yRad = T(0.5)*SGMisc<T>::pi();
    else if (1 <= tmp)
      yRad = -T(0.5)*SGMisc<T>::pi();
    else
      yRad = -asin(tmp);

    num = 2*(x()*y() + w()*z());
    den = sqrQW + sqrQX - sqrQY - sqrQZ;
    if (fabs(den) <= SGLimits<T>::min() &&
        fabs(num) <= SGLimits<T>::min())
      zRad = 0;
    else {
      T psi = atan2(num, den);
      if (psi < 0)
        psi += 2*SGMisc<T>::pi();
      zRad = psi;
    }
  }

  /// write the euler angles in degrees into the references
  void getEulerDeg(T& zDeg, T& yDeg, T& xDeg) const
  {
    getEulerRad(zDeg, yDeg, xDeg);
    zDeg = SGMisc<T>::rad2deg(zDeg);
    yDeg = SGMisc<T>::rad2deg(yDeg);
    xDeg = SGMisc<T>::rad2deg(xDeg);
  }

  /// write the angle axis representation into the references
  void getAngleAxis(T& angle, SGVec3<T>& axis) const
  {
    T nrm = norm(*this);
    if (nrm <= SGLimits<T>::min()) {
      angle = 0;
      axis = SGVec3<T>(0, 0, 0);
    } else {
      T rNrm = 1/nrm;
      angle = acos(SGMisc<T>::max(-1, SGMisc<T>::min(1, rNrm*w())));
      T sAng = sin(angle);
      if (fabs(sAng) <= SGLimits<T>::min())
        axis = SGVec3<T>(1, 0, 0);
      else
        axis = (rNrm/sAng)*imag(*this);
      angle *= 2;
    }
  }

  /// write the angle axis representation into the references
  void getAngleAxis(SGVec3<T>& axis) const
  {
    T angle;
    getAngleAxis(angle, axis);
    axis *= angle;
  }

  /// Get the imaginary part of the quaternion.
  /// The imaginary part should point into that axis direction that results in
  /// a quaternion with a positive real part.
  /// This is the smallest numerically stable representation of an orientation
  /// in space. See fromPositiveRealImag()
  SGVec3<T> getPositiveRealImag() const
  {
    if (real(*this) < T(0))
      return (T(-1)/norm(*this))*imag(*this);
    else
      return (T(1)/norm(*this))*imag(*this);
  }

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
  /// Access the w component
  const T& w(void) const
  { return data()[3]; }
  /// Access the w component
  T& w(void)
  { return data()[3]; }

  /// Get the data pointer
  const T (&data(void) const)[4]
  { return _data; }
  /// Get the data pointer
  T (&data(void))[4]
  { return _data; }

  /// Inplace addition
  SGQuat& operator+=(const SGQuat& v)
  { data()[0]+=v(0);data()[1]+=v(1);data()[2]+=v(2);data()[3]+=v(3);return *this; }
  /// Inplace subtraction
  SGQuat& operator-=(const SGQuat& v)
  { data()[0]-=v(0);data()[1]-=v(1);data()[2]-=v(2);data()[3]-=v(3);return *this; }
  /// Inplace scalar multiplication
  template<typename S>
  SGQuat& operator*=(S s)
  { data()[0] *= s; data()[1] *= s; data()[2] *= s; data()[3] *= s; return *this; }
  /// Inplace scalar multiplication by 1/s
  template<typename S>
  SGQuat& operator/=(S s)
  { return operator*=(1/T(s)); }
  /// Inplace quaternion multiplication
  SGQuat& operator*=(const SGQuat& v);

  /// Transform a vector from the current coordinate frame to a coordinate
  /// frame rotated with the quaternion
  SGVec3<T> transform(const SGVec3<T>& v) const
  {
    T r = 2/dot(*this, *this);
    SGVec3<T> qimag = imag(*this);
    T qr = real(*this);
    return (r*qr*qr - 1)*v + (r*dot(qimag, v))*qimag - (r*qr)*cross(qimag, v);
  }
  /// Transform a vector from the coordinate frame rotated with the quaternion
  /// to the current coordinate frame
  SGVec3<T> backTransform(const SGVec3<T>& v) const
  {
    T r = 2/dot(*this, *this);
    SGVec3<T> qimag = imag(*this);
    T qr = real(*this);
    return (r*qr*qr - 1)*v + (r*dot(qimag, v))*qimag + (r*qr)*cross(qimag, v);
  }

  /// Rotate a given vector with the quaternion
  SGVec3<T> rotate(const SGVec3<T>& v) const
  { return backTransform(v); }
  /// Rotate a given vector with the inverse quaternion
  SGVec3<T> rotateBack(const SGVec3<T>& v) const
  { return transform(v); }

  /// Return the time derivative of the quaternion given the angular velocity
  SGQuat
  derivative(const SGVec3<T>& angVel) const
  {
    SGQuat deriv;

    deriv.w() = T(0.5)*(-x()*angVel(0) - y()*angVel(1) - z()*angVel(2));
    deriv.x() = T(0.5)*( w()*angVel(0) - z()*angVel(1) + y()*angVel(2));
    deriv.y() = T(0.5)*( z()*angVel(0) + w()*angVel(1) - x()*angVel(2));
    deriv.z() = T(0.5)*(-y()*angVel(0) + x()*angVel(1) + w()*angVel(2));

    return deriv;
  }

  /// Return the angular velocity w that makes q0 translate to q1 using
  /// an explicit euler step with stepsize h.
  /// That is, look for an w where
  /// q1 = normalize(q0 + h*q0.derivative(w))
  static SGVec3<T>
  forwardDifferenceVelocity(const SGQuat& q0, const SGQuat& q1, const T& h)
  {
    // Let D_q0*w = q0.derivative(w), D_q0 the above 4x3 matrix.
    // Then D_q0^t*D_q0 = 0.25*Id and D_q0*q0 = 0.
    // Let lambda be a nonzero normailzation factor, then
    //  q1 = normalize(q0 + h*q0.derivative(w))
    // can be rewritten
    //  lambda*q1 = q0 + h*D_q0*w.
    // Multiply left by the transpose D_q0^t and reorder gives
    //  4*lambda/h*D_q0^t*q1 = w.
    // Now compute lambda by substitution of w into the original
    // equation
    //  lambda*q1 = q0 + 4*lambda*D_q0*D_q0^t*q1,
    // multiply by q1^t from the left
    //  lambda*<q1,q1> = <q0,q1> + 4*lambda*<D_q0^t*q1,D_q0^t*q1>
    // and solving for lambda gives
    //  lambda = <q0,q1>/(1 - 4*<D_q0^t*q1,D_q0^t*q1>).

    // The transpose of the derivative matrix
    // the 0.5 factor is handled below
    // also note that the initializer uses x, y, z, w instead of w, x, y, z
    SGQuat d0(q0.w(), q0.z(), -q0.y(), -q0.x());
    SGQuat d1(-q0.z(), q0.w(), q0.x(), -q0.y());
    SGQuat d2(q0.y(), -q0.x(), q0.w(), -q0.z());
    // 2*D_q0^t*q1
    SGVec3<T> Dq(dot(d0, q1), dot(d1, q1), dot(d2, q1));
    // Like above, but take into account that Dq = 2*D_q0^t*q1
    T lambda = dot(q0, q1)/(T(1) - dot(Dq, Dq));
    return (2*lambda/h)*Dq;
  }

private:

  // Private because it assumes normalized inputs.
  static SGQuat
  fromRotateToSmaller90Deg(T cosang,
                           const SGVec3<T>& from, const SGVec3<T>& to)
  {
    // In this function we assume that the angle required to rotate from
    // the vector from to the vector to is <= 90 deg.
    // That is done so because of possible instabilities when we rotate more
    // then 90deg.

    // Note that the next comment does actually cover a *more* *general* case
    // than we need in this function. That shows that this formula is even
    // valid for rotations up to 180deg.

    // Because of the signs in the axis, it is sufficient to care for angles
    // in the interval [-pi,pi]. That means that 0.5*angle is in the interval
    // [-pi/2,pi/2]. But in that range the cosine is allways >= 0.
    // So we do not need to care for egative roots in the following equation:
    T cos05ang = sqrt(T(0.5)+T(0.5)*cosang);


    // Now our assumption of angles <= 90 deg comes in play.
    // For that reason, we know that cos05ang is not zero.
    // It is even more, we can see from the above formula that
    // sqrt(0.5) < cos05ang.


    // Compute the rotation axis, that is
    // sin(angle)*normalized rotation axis
    SGVec3<T> axis = cross(to, from);

    // We need sin(0.5*angle)*normalized rotation axis.
    // So rescale with sin(0.5*x)/sin(x).
    // To do that we use the equation:
    // sin(x) = 2*sin(0.5*x)*cos(0.5*x)
    return SGQuat::fromRealImag( cos05ang, (1/(2*cos05ang))*axis);
  }

  // Private because it assumes normalized inputs.
  static SGQuat
  fromRotateToNorm(const SGVec3<T>& from, const SGVec3<T>& to)
  {
    // To avoid instabilities with roundoff, we distinguish between rotations
    // with more then 90deg and rotations with less than 90deg.

    // Compute the cosine of the angle.
    T cosang = dot(from, to);

    // For the small ones do direct computation
    if (T(-0.5) < cosang)
      return SGQuat::fromRotateToSmaller90Deg(cosang, from, to);

    // For larger rotations. first rotate from to -from.
    // Past that we will have a smaller angle again.
    SGQuat q1 = SGQuat::fromChangeSign(from);
    SGQuat q2 = SGQuat::fromRotateToSmaller90Deg(-cosang, -from, to);
    return q1*q2;
  }

  T _data[4];
};

/// Unary +, do nothing ...
template<typename T>
inline
const SGQuat<T>&
operator+(const SGQuat<T>& v)
{ return v; }

/// Unary -, do nearly nothing
template<typename T>
inline
SGQuat<T>
operator-(const SGQuat<T>& v)
{ return SGQuat<T>(-v(0), -v(1), -v(2), -v(3)); }

/// Binary +
template<typename T>
inline
SGQuat<T>
operator+(const SGQuat<T>& v1, const SGQuat<T>& v2)
{ return SGQuat<T>(v1(0)+v2(0), v1(1)+v2(1), v1(2)+v2(2), v1(3)+v2(3)); }

/// Binary -
template<typename T>
inline
SGQuat<T>
operator-(const SGQuat<T>& v1, const SGQuat<T>& v2)
{ return SGQuat<T>(v1(0)-v2(0), v1(1)-v2(1), v1(2)-v2(2), v1(3)-v2(3)); }

/// Scalar multiplication
template<typename S, typename T>
inline
SGQuat<T>
operator*(S s, const SGQuat<T>& v)
{ return SGQuat<T>(s*v(0), s*v(1), s*v(2), s*v(3)); }

/// Scalar multiplication
template<typename S, typename T>
inline
SGQuat<T>
operator*(const SGQuat<T>& v, S s)
{ return SGQuat<T>(s*v(0), s*v(1), s*v(2), s*v(3)); }

/// Quaterion multiplication
template<typename T>
inline
SGQuat<T>
operator*(const SGQuat<T>& v1, const SGQuat<T>& v2)
{
  SGQuat<T> v;
  v.x() = v1.w()*v2.x() + v1.x()*v2.w() + v1.y()*v2.z() - v1.z()*v2.y();
  v.y() = v1.w()*v2.y() - v1.x()*v2.z() + v1.y()*v2.w() + v1.z()*v2.x();
  v.z() = v1.w()*v2.z() + v1.x()*v2.y() - v1.y()*v2.x() + v1.z()*v2.w();
  v.w() = v1.w()*v2.w() - v1.x()*v2.x() - v1.y()*v2.y() - v1.z()*v2.z();
  return v;
}

/// Now define the inplace multiplication
template<typename T>
inline
SGQuat<T>&
SGQuat<T>::operator*=(const SGQuat& v)
{ (*this) = (*this)*v; return *this; }

/// The conjugate of the quaternion, this is also the
/// inverse for normalized quaternions
template<typename T>
inline
SGQuat<T>
conj(const SGQuat<T>& v)
{ return SGQuat<T>(-v(0), -v(1), -v(2), v(3)); }

/// The conjugate of the quaternion, this is also the
/// inverse for normalized quaternions
template<typename T>
inline
SGQuat<T>
inverse(const SGQuat<T>& v)
{ return (1/dot(v, v))*SGQuat<T>(-v(0), -v(1), -v(2), v(3)); }

/// The imagniary part of the quaternion
template<typename T>
inline
T
real(const SGQuat<T>& v)
{ return v.w(); }

/// The imagniary part of the quaternion
template<typename T>
inline
SGVec3<T>
imag(const SGQuat<T>& v)
{ return SGVec3<T>(v.x(), v.y(), v.z()); }

/// Scalar dot product
template<typename T>
inline
T
dot(const SGQuat<T>& v1, const SGQuat<T>& v2)
{ return v1(0)*v2(0) + v1(1)*v2(1) + v1(2)*v2(2) + v1(3)*v2(3); }

/// The euclidean norm of the vector, that is what most people call length
template<typename T>
inline
T
norm(const SGQuat<T>& v)
{ return sqrt(dot(v, v)); }

/// The euclidean norm of the vector, that is what most people call length
template<typename T>
inline
T
length(const SGQuat<T>& v)
{ return sqrt(dot(v, v)); }

/// The 1-norm of the vector, this one is the fastest length function we
/// can implement on modern cpu's
template<typename T>
inline
T
norm1(const SGQuat<T>& v)
{ return fabs(v(0)) + fabs(v(1)) + fabs(v(2)) + fabs(v(3)); }

/// The euclidean norm of the vector, that is what most people call length
template<typename T>
inline
SGQuat<T>
normalize(const SGQuat<T>& q)
{ return (1/norm(q))*q; }

/// Return true if exactly the same
template<typename T>
inline
bool
operator==(const SGQuat<T>& v1, const SGQuat<T>& v2)
{ return v1(0)==v2(0) && v1(1)==v2(1) && v1(2)==v2(2) && v1(3)==v2(3); }

/// Return true if not exactly the same
template<typename T>
inline
bool
operator!=(const SGQuat<T>& v1, const SGQuat<T>& v2)
{ return ! (v1 == v2); }

/// Return true if equal to the relative tolerance tol
/// Note that this is not the same than comparing quaternions to represent
/// the same rotation
template<typename T>
inline
bool
equivalent(const SGQuat<T>& v1, const SGQuat<T>& v2, T tol)
{ return norm1(v1 - v2) < tol*(norm1(v1) + norm1(v2)); }

/// Return true if about equal to roundoff of the underlying type
/// Note that this is not the same than comparing quaternions to represent
/// the same rotation
template<typename T>
inline
bool
equivalent(const SGQuat<T>& v1, const SGQuat<T>& v2)
{ return equivalent(v1, v2, 100*SGLimits<T>::epsilon()); }

#ifndef NDEBUG
template<typename T>
inline
bool
isNaN(const SGQuat<T>& v)
{
  return SGMisc<T>::isNaN(v(0)) || SGMisc<T>::isNaN(v(1))
    || SGMisc<T>::isNaN(v(2)) || SGMisc<T>::isNaN(v(3));
}
#endif

/// quaternion interpolation for t in [0,1] interpolate between src (=0)
/// and dst (=1)
template<typename T>
inline
SGQuat<T>
interpolate(T t, const SGQuat<T>& src, const SGQuat<T>& dst)
{
  T cosPhi = dot(src, dst);
  // need to take the shorter way ...
  int signCosPhi = SGMisc<T>::sign(cosPhi);
  // cosPhi must be corrected for that sign
  cosPhi = fabs(cosPhi);

  // first opportunity to fail - make sure acos will succeed later -
  // result is correct
  if (1 <= cosPhi)
    return dst;

  // now the half angle between the orientations
  T o = acos(cosPhi);

  // need the scales now, if the angle is very small, do linear interpolation
  // to avoid instabilities
  T scale0, scale1;
  if (fabs(o) <= SGLimits<T>::epsilon()) {
    scale0 = 1 - t;
    scale1 = t;
  } else {
    // note that we can give a positive lower bound for sin(o) here
    T sino = sin(o);
    T so = 1/sino;
    scale0 = sin((1 - t)*o)*so;
    scale1 = sin(t*o)*so;
  }

  return scale0*src + signCosPhi*scale1*dst;
}

/// Output to an ostream
template<typename char_type, typename traits_type, typename T>
inline
std::basic_ostream<char_type, traits_type>&
operator<<(std::basic_ostream<char_type, traits_type>& s, const SGQuat<T>& v)
{ return s << "[ " << v(0) << ", " << v(1) << ", " << v(2) << ", " << v(3) << " ]"; }

inline
SGQuatf
toQuatf(const SGQuatd& v)
{ return SGQuatf((float)v(0), (float)v(1), (float)v(2), (float)v(3)); }

inline
SGQuatd
toQuatd(const SGQuatf& v)
{ return SGQuatd(v(0), v(1), v(2), v(3)); }

#endif
