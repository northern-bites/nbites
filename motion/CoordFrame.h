#ifndef CoordFrame_h_DEFINED
#define CoordFrame_h_DEFINED

#include <boost/numeric/ublas/matrix.hpp>
#include <boost/numeric/ublas/vector.hpp>
#include <boost/numeric/ublas/triangular.hpp>
#include <boost/numeric/ublas/vector_proxy.hpp>
#include <boost/numeric/ublas/lu.hpp>              // for lu_factorize
#include <boost/numeric/ublas/io.hpp>              // for cout
using namespace boost::numeric;

#include <cmath>
//Homogeneous 3D coordinates (really this is 2D
namespace CoordFrame3D{

    enum Axis {
        X_AXIS = 0,
        Y_AXIS,
        Z_AXIS
    };

    // -------------------- Helper matrix methods --------------------

    static const ublas::matrix <float> rotation3D(const Axis axis,
                                                  const float angle) {
        ublas::matrix <float> rot = ublas::identity_matrix <float>(3);

        const float sinAngle = sin(angle);
        const float cosAngle = cos(angle);
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

    static const ublas::matrix <float> translation3D(const float dx,
                                                     const float dy) {
        ublas::matrix <float> trans = ublas::identity_matrix <float>(3);
        trans(X_AXIS, Z_AXIS) = dx;
        trans(Y_AXIS, Z_AXIS) = dy;
        return trans;
    }

    static const ublas::vector <float> vector2D(const float x, const float y) {
        ublas::vector <float> p = ublas::zero_vector <float> (2);
        p(0) = x;
        p(1) = y;
        return p;
    }

    static const ublas::vector <float> vector3D(const float x, const float y,
                                                const float z = 1.0f) {
        ublas::vector <float> p = ublas::zero_vector <float> (3);
        p(0) = x;
        p(1) = y;
        p(2) = z;
        return p;
    }


}
#endif
