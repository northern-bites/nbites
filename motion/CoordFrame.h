
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

#ifndef CoordFrame_h_DEFINED
#define CoordFrame_h_DEFINED

#include <math.h>

#include <boost/numeric/ublas/matrix.hpp>
#include <boost/numeric/ublas/vector.hpp>
#include <boost/numeric/ublas/triangular.hpp>
#include <boost/numeric/ublas/vector_proxy.hpp>
#include <boost/numeric/ublas/lu.hpp>              // for lu_factorize
#include <boost/numeric/ublas/io.hpp>              // for cout

#include "NBMatrixMath.h"
#include "NBMath.h"

//Homogeneous 3D coordinates (really this is 2D
namespace CoordFrame3D{

    enum Axis {
        X_AXIS = 0,
        Y_AXIS,
        Z_AXIS
    };

    // -------------------- Helper matrix methods --------------------
    static const NBMath::ufmatrix3 rotation3D(const Axis axis,
                                              const float angle) {
        NBMath::ufmatrix3 rot =
            boost::numeric::ublas::identity_matrix <float>(3);

        const float sinAngle = std::sin(angle);
        const float cosAngle = std::cos(angle);
        if (angle == 0.0) { //OPTIMIZAION POINT
            return rot;
        }
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

    static const NBMath::ufmatrix3 translation3D(const float dx,
                                                 const float dy) {
        boost::numeric::ublas::matrix <float> trans =
            boost::numeric::ublas::identity_matrix <float>(3);
        trans(X_AXIS, Z_AXIS) = dx;
        trans(Y_AXIS, Z_AXIS) = dy;
        return trans;
    }

    static const NBMath::ufvector3 vector3D(const float x, const float y,
                                            const float z = 1.0f) {
        NBMath::ufvector3 p = boost::numeric::ublas::zero_vector <float> (3);
        p(0) = x;
        p(1) = y;
        p(2) = z;
        return p;
    }

    static const NBMath::ufmatrix3 identity3D(){
        return translation3D(0.0f,0.0f);
    }

};


namespace CoordFrame4D{
    enum Axis {
        X_AXIS = 0,
        Y_AXIS,
        Z_AXIS,
        W_AXIS
    };

    // -------------------- Helper matrix methods --------------------

    //TODO: Move all these rot4D, etc into a Coord4D namespace.
    // also, make all these use bounded matrices (see NBMatrixMath)
    static const NBMath::ufmatrix4
    rotation4D(const Axis axis,
               const float angle) {
        NBMath::ufmatrix4 rot = boost::numeric::ublas::identity_matrix <float>(4);

        //TODO: Make this one call:
        float sinAngle;
        float cosAngle;
        sincosf(angle, &sinAngle, &cosAngle);

        if (angle == 0.0) { //OPTIMIZAION POINT
            return rot;
        }
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

    static const NBMath::ufmatrix4 translation4D(const float dx,
                                                 const float dy,
                                                 const float dz) {
        NBMath::ufmatrix4 trans = boost::numeric::ublas::identity_matrix <float>(4);
        trans(X_AXIS, W_AXIS) = dx;
        trans(Y_AXIS, W_AXIS) = dy;
        trans(Z_AXIS, W_AXIS) = dz;
        return trans;
    }

    static const NBMath::ufvector4 vector4D(const float x, const float y,
                                            const float z,
                                            const float w = 1.0f) {
        NBMath::ufvector4 p = boost::numeric::ublas::zero_vector <float> (4);
        p(0) = x;
        p(1) = y;
        p(2) = z;
        p(3) = w;
        return p;
    }

};
#endif
