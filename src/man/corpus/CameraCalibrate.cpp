#include "CameraCalibrate.h"

//TODO: introduce an unified configure file system

namespace Kinematics {

// order is Y, X because when we when we multiply a vector to the right the order
// in which the vector is multiplied with the transforms is X, Y (since it goes right
// to left; e.g. for M = ROT_Y * ROT_X, we have Mv = ROT_Y * ROT_X * v, so v gets multiplied
// with ROT_X first, which is the proper order 
// WARNING: matrix multiplication not commutative

//default parameters/transforms
float CameraCalibrate::Params[NUM_PARAMS] = {0.0f, 0.0f};
ufmatrix4 CameraCalibrate::Transforms[2] =
{ CoordFrame4D::rotation4D(CoordFrame4D::Y_AXIS,
                           CameraCalibrate::Params[CameraCalibrate::PITCH]),
  CoordFrame4D::rotation4D(CoordFrame4D::X_AXIS,
                           CameraCalibrate::Params[CameraCalibrate::ROLL])};


void CameraCalibrate::init(std::string name) {

    float params[] = {0.0f, 0.0f};

    if (name == "river")
        params[PITCH] = -.03f;
    else if (name == "mal")
        params[PITCH] = 0.04f;

    UpdateWithParams(params);
}

void CameraCalibrate::UpdateWithParams(float _Params[]){
    //update the parameter array
    for (int i = 0; i < NUM_PARAMS; i++) {
        Params[i] = _Params[i];
    }

    //then update all of the transforms based on the new params
    Transforms[0] =
        CoordFrame4D::rotation4D(CoordFrame4D::Y_AXIS,
                                 Params[PITCH]);
    Transforms[1] =
        CoordFrame4D::rotation4D(CoordFrame4D::X_AXIS,
                                 Params[ROLL]);
}

}
