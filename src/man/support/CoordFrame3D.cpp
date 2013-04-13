
// This file is part of Man, a robotic perception, locomotion, and
// team strategy application created by the Northern Bites RoboCup
// team of Bowdoin College in Brunswick, Maine, for the Aldebaran
// Nao robot.
//
// Man is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// Man is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser Public License for more details.
//
// You should have received a copy of the GNU General Public License
// and the GNU Lesser Public License along with Man.  If not, see
// <http://www.gnu.org/licenses/>.

#include <math.h>

#include <boost/numeric/ublas/matrix.hpp>
#include <boost/numeric/ublas/vector.hpp>
#include <boost/numeric/ublas/triangular.hpp>
#include <boost/numeric/ublas/vector_proxy.hpp>
#include <boost/numeric/ublas/lu.hpp>              // for lu_factorize
#include <boost/numeric/ublas/io.hpp>              // for cout

#include "CoordFrame.h"

using namespace NBMath;
using namespace CoordFrame3D;

// -------------------- Helper matrix methods --------------------
const NBMath::ufmatrix3 CoordFrame3D::rotation3D(const Axis axis,
                                                 const float angle) {
    NBMath::ufmatrix3 rot =
        boost::numeric::ublas::identity_matrix <float>(3);

    if (angle == 0.0) { //OPTIMIZAION POINT
        return rot;
    }
    const float sinAngle = std::sin(angle);
    const float cosAngle = std::cos(angle);

    switch(axis) {
    case Z_AXIS:
        rot(X_AXIS, X_AXIS) =  cosAngle;
        rot(X_AXIS, Y_AXIS) = -sinAngle;
        rot(Y_AXIS, X_AXIS) =  sinAngle;
        rot(Y_AXIS, Y_AXIS) =  cosAngle;
        break;
    default:
        break;
    }
    return rot;
}

const NBMath::ufmatrix3 CoordFrame3D::translation3D(const float dx,
                                                    const float dy) {
    boost::numeric::ublas::matrix <float> trans =
        boost::numeric::ublas::identity_matrix <float>(3);
    trans(X_AXIS, Z_AXIS) = dx;
    trans(Y_AXIS, Z_AXIS) = dy;
    return trans;
}

const NBMath::ufvector3 CoordFrame3D::vector3D(const float x, const float y,
                                               const float z) {
    NBMath::ufvector3 p = boost::numeric::ublas::zero_vector <float> (3);
    p(0) = x;
    p(1) = y;
    p(2) = z;
    return p;
}

const NBMath::ufrowVector3 CoordFrame3D::rowVector3D(const float x,
                                                  const float y,
                                                  const float z) {
    NBMath::ufrowVector3 p = boost::numeric::ublas::zero_matrix <float> (1,3);
    p(0,0) = x;
    p(0,1) = y;
    p(0,2) = z;
    return p;
}

const NBMath::ufmatrix3 CoordFrame3D::identity3D(){
    return boost::numeric::ublas::identity_matrix <float> (3);
}
