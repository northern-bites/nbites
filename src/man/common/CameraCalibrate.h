#ifndef _CameraCalibrate_h_DEFINED
#define _CameraCalibrate_h_DEFINED

#include <boost/numeric/ublas/matrix.hpp>
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
    static void init(std::string name);
    static void UpdateWithParams(float paramsTop[], float paramsBottom[]);
    static ufmatrix4* getTransforms(man::image::Camera::Type which) {
        if (which == man::image::Camera::TOP) {
            return TransformsTop;
        } else {
            return TransformsBottom;
        }
    }

};


#endif
