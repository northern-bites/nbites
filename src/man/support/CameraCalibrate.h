#ifndef _CameraCalibrate_h_DEFINED
#define _CameraCalibrate_h_DEFINED

#include <boost/numeric/ublas/matrix.hpp>
#include <stdlib.h>
#include "CoordFrame.h" //for rotation4D, etc
#include "NBMatrixMath.h" // for ufmatrix4

#include "Camera.h"

using namespace NBMath;

class CameraCalibrate {

public:
    enum Angle {
        ROLL = 0,
        PITCH
        };

    static const int NUM_PARAMS = 2;

    static float ParamsTop[NUM_PARAMS];
    static float ParamsBottom[NUM_PARAMS];
    static ufmatrix4 TransformsTop[2];
    static ufmatrix4 TransformsBottom[2];

    //TODO: not the best way to do this, but works
    //hack to get the parameters in
    //we should just update the params once
    static void init();
    static void UpdateByName(std::string name);
    static void UpdateWithParams(float paramsTop[], float paramsBottom[]);

    static ufmatrix4* getTransforms(Camera::Type which) {
        if (which == Camera::TOP) {
            return TransformsTop;
        } else {
            return TransformsBottom;
        }
    }

    static float* getCurrentParameters(Camera::Type which)
    {
        if (which == Camera::TOP)
        {
            return ParamsTop;
        }
        else
        {
            return ParamsBottom;
        }
    }

};


#endif
