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

#ifndef SGMathFwd_H
#define SGMathFwd_H

// All forward declarations in case they only need to be declared

class SGGeoc;
class SGGeod;

template<typename T>
class SGLocation;
template<typename T>
class SGLimits;
template<typename T>
class SGMatrix;
template<typename T>
class SGMisc;
template<typename T>
class SGQuat;
template<typename T>
class SGVec2;
template<typename T>
class SGVec3;
template<typename T>
class SGVec4;

typedef SGLocation<float> SGLocationf;
typedef SGLocation<double> SGLocationd;
typedef SGLimits<float> SGLimitsf;
typedef SGLimits<double> SGLimitsd;
typedef SGMatrix<float> SGMatrixf;
typedef SGMatrix<double> SGMatrixd;
typedef SGMisc<float> SGMiscf;
typedef SGMisc<double> SGMiscd;
typedef SGQuat<float> SGQuatf;
typedef SGQuat<double> SGQuatd;
typedef SGVec2<float> SGVec2f;
typedef SGVec2<double> SGVec2d;
typedef SGVec2<int> SGVec2i;
typedef SGVec3<float> SGVec3f;
typedef SGVec3<double> SGVec3d;
typedef SGVec3<int> SGVec3i;
typedef SGVec4<float> SGVec4f;
typedef SGVec4<double> SGVec4d;
typedef SGVec4<int> SGVec4i;

#endif
