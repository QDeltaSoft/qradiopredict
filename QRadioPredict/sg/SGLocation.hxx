// Copyright (C) 2012  Mathias Froehlich - Mathias.Froehlich@web.de
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

#ifndef SGLocation_HXX
#define SGLocation_HXX

/// Encapsulates a pair SGVec3 and SGQuat.
/// Together they encapsulate a cartesian position/orientation.
/// Included are methods to do a simple euler position propagation step.
template<typename T>
class SGLocation {
public:
    SGLocation(const SGVec3<T>& position = SGVec3<T>::zeros(),
               const SGQuat<T>& orientation = SGQuat<T>::unit()) :
        _position(position),
        _orientation(orientation)
    { }

    const SGVec3<T>& getPosition() const
    { return _position; }
    void setPosition(const SGVec3<T>& position)
    { _position = position; }

    const SGQuat<T>& getOrientation() const
    { return _orientation; }
    void setOrientation(const SGQuat<T>& orientation)
    { _orientation = orientation; }

    /// Returns the absolute position of a relative position relative to this
    SGVec3<T> getAbsolutePosition(const SGVec3<T>& relativePosition) const
    { return getOrientation().backTransform(relativePosition) + _position; }
    /// Returns the relative position of an absolute position relative to this
    SGVec3<T> getRelativePosition(const SGVec3<T>& absolutePosition) const
    { return getOrientation().transform(absolutePosition) - _position; }

    /// Returns the absolute orientation of a relative orientation relative to this
    SGQuat<T> getAbsoluteOrientation(const SGQuat<T>& relativeOrientation) const
    { return getOrientation()*relativeOrientation; }
    /// Returns the relative orientation of an absolute orientation relative to this
    SGQuat<T> getRelativeOrientation(const SGQuat<T>& absoluteOrientation) const
    { return inverse(getOrientation())*absoluteOrientation; }

    /// Returns the absolute location of a relative location relative to this
    SGLocation getAbsoluteLocation(const SGLocation& relativeLocation) const
    {
        return SGLocation(getAbsolutePosition(relativeLocation.getPosition()),
                          getAbsoluteOrientation(relativeLocation.getOrientation()));
    }

    /// Returns the relative location of an absolute location relative to this
    SGLocation getRelativeLocation(const SGLocation& absoluteLocation) const
    {
        return SGLocation(getRelativePosition(absoluteLocation.getPosition()),
                          getRelativeOrientation(absoluteLocation.getOrientation()));
    }

    /// Executes an euler step with the given velocities in the current location
    void eulerStepBodyVelocities(const T& dt, const SGVec3<T>& linearBodyVelocity, const SGVec3<T>& angularBodyVelocity)
    {
        // Get the derivatives of the position and orientation due to the body velocities
        SGVec3<T> pDot = getOrientation().backTransform(linearBodyVelocity);
        SGQuat<T> qDot = getOrientation().derivative(angularBodyVelocity);

        // and do the euler step.
        setPosition(getPosition() + dt*pDot);
        setOrientation(normalize(getOrientation() + dt*qDot));
    }

    /// Executes an euler step with the given velocities in the current location,
    /// The position advance is here done with orientation in the middle of the orientation change.
    /// This leads to mostly correct extrapolations with rotating motion - even for longer times.
    void eulerStepBodyVelocitiesMidOrientation(const T& dt, const SGVec3<T>& linearBodyVelocity, const SGVec3<T>& angularBodyVelocity)
    {
        // Store the old orientation ...
        SGQuat<T> orientation = getOrientation();
        // ... and compute the new orientation
        SGQuat<T> qDot = orientation.derivative(angularBodyVelocity);
        setOrientation(normalize(orientation + dt*qDot));

        // Then with the orientation in between, advance the position
        SGQuat<T> orientation05 = normalize(getOrientation() + orientation);
        SGVec3<T> pDot = orientation05.backTransform(linearBodyVelocity);
        setPosition(getPosition() + dt*pDot);
    }

    /// Executes an euler step with the given velocities in the current location
    void eulerStepGlobalVelocities(const T& dt, const SGVec3<T>& linearVelocity, const SGVec3<T>& angularVelocity)
    {
        // Get the derivatives of the orientation in the body system
        SGVec3<T> angularBodyVelocity = getOrientation().transform(angularVelocity);
        SGQuat<T> qDot = getOrientation().derivative(angularBodyVelocity);

        // and do the euler step.
        setPosition(getPosition() + dt*linearVelocity);
        setOrientation(normalize(getOrientation() + dt*qDot));
    }

private:
    SGVec3<T> _position;
    SGQuat<T> _orientation;
};

#endif
