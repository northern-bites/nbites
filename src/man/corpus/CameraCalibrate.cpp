#include "CameraCalibrate.h"

//TODO: introduce an unified configure file system

#ifdef ROBOT_NAME_zaphod
float CameraCalibrate::Params[CameraCalibrate::NUM_PARAMS] = {-0.01f, 0.061f};
#else
#ifdef ROBOT_NAME_marvin
float CameraCalibrate::Params[CameraCalibrate::NUM_PARAMS] = {0.0f, 0.047f};
#else
#ifdef ROBOT_NAME_trillian
float CameraCalibrate::Params[CameraCalibrate::NUM_PARAMS] = {0.0f, 0.04f};
#else
#ifdef ROBOT_NAME_slarti
float CameraCalibrate::Params[CameraCalibrate::NUM_PARAMS] = {0.0f, 0.068f};
#else
#ifdef ROBOT_NAME_spock
float CameraCalibrate::Params[CameraCalibrate::NUM_PARAMS] = {0.017f, 0.113f};
#else
#ifdef ROBOT_NAME_annika
float CameraCalibrate::Params[CameraCalibrate::NUM_PARAMS] = {0.015f, 0.075f};
#else
#ifdef ROBOT_NAME_dax
float CameraCalibrate::Params[CameraCalibrate::NUM_PARAMS] = {-0.03f, 0.13f};
#else
#ifdef ROBOT_NAME_scotty
float CameraCalibrate::Params[CameraCalibrate::NUM_PARAMS] = {0.015f, 0.126f};
#else
#ifdef ROBOT_NAME_data
float CameraCalibrate::Params[CameraCalibrate::NUM_PARAMS] = {0.03f, 0.066f};
#else
float CameraCalibrate::Params[CameraCalibrate::NUM_PARAMS] = {0.0f, 0.0f};
#endif
#endif
#endif
#endif
#endif
#endif
#endif
#endif
#endif

// order is Y, X because when we when we multiply a vector to the right the order
// in which the vector is multiplied with the transforms is X, Y (since it goes right
// to left; e.g. for M = ROT_Y * ROT_X, we have Mv = ROT_Y * ROT_X * v, so v gets multiplied
// with ROT_X first, which is the proper order 
// WARNING: matrix multiplication not commutative

ufmatrix4 CameraCalibrate::Transforms[2] =
{ CoordFrame4D::rotation4D(CoordFrame4D::Y_AXIS,
                           CameraCalibrate::Params[CameraCalibrate::PITCH]),
  CoordFrame4D::rotation4D(CoordFrame4D::X_AXIS,
                           CameraCalibrate::Params[CameraCalibrate::ROLL])};

void CameraCalibrate::UpdateWithParams(float _Params[]){
    //update the parameter array
    for (int i = 0; i < NUM_PARAMS; i++)
        Params[i] = _Params[i];
    //then update all of the transforms based on the new params

    Transforms[0] =
        CoordFrame4D::rotation4D(CoordFrame4D::Y_AXIS,
                                 Params[PITCH]);
    Transforms[1] =
        CoordFrame4D::rotation4D(CoordFrame4D::X_AXIS,
                                 Params[ROLL]);
}
