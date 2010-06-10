#ifndef _CameraCalibrate_h_DEFINED
#define _CameraCalibrate_h_DEFINED

#include <boost/numeric/ublas/matrix.hpp>
#include "CoordFrame.h" //for rotation4D, etc
#include "NBMatrixMath.h" // for ufmatrix4
#include "Common.h" // for robot name

using namespace NBMath;

class CameraCalibrate{

public:
    enum Angle {
        ROLL = 0,
        PITCH
        };

    static const int NUM_PARAMS = 2;

    static float Params[NUM_PARAMS];
    static ufmatrix4 Transforms[2];

    static void UpdateWithParams(float params[]);

    };


#endif
