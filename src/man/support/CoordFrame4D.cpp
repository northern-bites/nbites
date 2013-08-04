
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
using namespace CoordFrame4D;

// -------------------- Helper matrix methods --------------------

//TODO: Move all these rot4D, etc into a Coord4D namespace.
// also, make all these use bounded matrices (see NBMatrixMath)
const NBMath::ufmatrix4
CoordFrame4D::rotation4D(const Axis axis,
                         const float angle) {
    NBMath::ufmatrix4 rot = boost::numeric::ublas::identity_matrix <float>(4);

    if (angle == 0.0) { //OPTIMIZAION POINT
        return rot;
    }

    //TODO: Make this one call:
    float sinAngle;
    float cosAngle;
    sincosf(angle, &sinAngle, &cosAngle);

    switch(axis) {
    case X_AXIS:
        rot(Y_AXIS, Y_AXIS) =  cosAngle;
        rot(Y_AXIS, Z_AXIS) = -sinAngle;
        rot(Z_AXIS, Y_AXIS) =  sinAngle;
        rot(Z_AXIS, Z_AXIS) =  cosAngle;
        break;
    case Y_AXIS:
        rot(X_AXIS, X_AXIS) =  cosAngle;
        rot(X_AXIS, Z_AXIS) =  sinAngle;
        rot(Z_AXIS, X_AXIS) = -sinAngle;
        rot(Z_AXIS, Z_AXIS) =  cosAngle;
        break;
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

const NBMath::ufmatrix4 CoordFrame4D::translation4D(const float dx,
                                                    const float dy,
                                                    const float dz) {
    NBMath::ufmatrix4 trans = boost::numeric::ublas::identity_matrix <float>(4);
    trans(X_AXIS, W_AXIS) = dx;
    trans(Y_AXIS, W_AXIS) = dy;
    trans(Z_AXIS, W_AXIS) = dz;
    return trans;
}

const NBMath::ufvector4 CoordFrame4D::vector4D(const float x, const float y,
                                               const float z,
                                               const float w) {
    NBMath::ufvector4 p = boost::numeric::ublas::zero_vector <float> (4);
    p(0) = x;
    p(1) = y;
    p(2) = z;
    p(3) = w;
    return p;
}


/**
 * Returns precalculated Trans[x,y,z].Rotz[wz].Roty[wy].Rotx[wx]
 */
const NBMath::ufmatrix4 CoordFrame4D::get6DTransform(const float x,
                                                     const float y,
                                                     const float z,
                                                     const float wx,
                                                     const float wy,
                                                     const float wz){
    float cwx,cwy,cwz,
        swx,swy,swz;
    sincosf(wx,&swx,&cwx);
    sincosf(wy,&swy,&cwy);
    sincosf(wz,&swz,&cwz);
    NBMath::ufmatrix4 r = boost::numeric::ublas::identity_matrix<float>(4);

    //Row 1
    r(0,0) =cwy*cwz;
    r(0,1) =cwz*swx*swy-cwx*swz;
    r(0,2) =cwx*cwz*swy+swx*swz;
    r(0,3) =x;
    //Row2
    r(1,0) =cwy*swz;
    r(1,1) =cwx*cwz+swx*swy*swz;
    r(1,2) =-cwz*swx+cwx*swy*swz;
    r(1,3) =y;
    //Row3
    r(2,0) =-swy;
    r(2,1) =cwy*swx;
    r(2,2) =cwx*cwy;
    r(2,3) =z;

    return r;
}

const NBMath::ufmatrix4
CoordFrame4D::invertHomogenous(const NBMath::ufmatrix4 source){
    const NBMath::ufmatrix3 Rt = trans(subrange(source,0,3,0,3));
    const NBMath::ufvector3 Rtd = -prod(Rt,
                                        CoordFrame3D::vector3D(source(0,3),
                                                               source(1,3),
                                                               source(2,3)));
    NBMath::ufmatrix4 result =
        boost::numeric::ublas::identity_matrix<float>(4);
    subrange(result,0,3,0,3) = Rt;
    result(0,3) = Rtd(0);
    result(1,3) = Rtd(1);
    result(2,3) = Rtd(2);
    return result;
}
