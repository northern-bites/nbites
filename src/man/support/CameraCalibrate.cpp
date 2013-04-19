#include "CameraCalibrate.h"

// order is Y, X because when we when we multiply a vector to the right the order
// in which the vector is multiplied with the transforms is X, Y (since it goes right
// to left; e.g. for M = ROT_Y * ROT_X, we have Mv = ROT_Y * ROT_X * v, so v gets multiplied
// with ROT_X first, which is the proper order 
// WARNING: matrix multiplication not commutative

//default parameters/transforms
float CameraCalibrate::ParamsTop[NUM_PARAMS] = {0.0f, 0.0f};
float CameraCalibrate::ParamsBottom[NUM_PARAMS] = {0.0f, 0.0f};
ufmatrix4 CameraCalibrate::TransformsTop[2] =
{ CoordFrame4D::rotation4D(CoordFrame4D::Y_AXIS,
                           CameraCalibrate::ParamsTop[CameraCalibrate::PITCH]),
  CoordFrame4D::rotation4D(CoordFrame4D::X_AXIS,
                           CameraCalibrate::ParamsTop[CameraCalibrate::ROLL])};
ufmatrix4 CameraCalibrate::TransformsBottom[2] =
{ CoordFrame4D::rotation4D(CoordFrame4D::Y_AXIS,
                           CameraCalibrate::ParamsBottom[CameraCalibrate::PITCH]),
  CoordFrame4D::rotation4D(CoordFrame4D::X_AXIS,
                           CameraCalibrate::ParamsBottom[CameraCalibrate::ROLL])};

void CameraCalibrate::init(std::string name) {

    float paramsTop[] = {0.0f, 0.0f};
    float paramsBottom[] = {0.0f, 0.0f};

    if (name == "river") {
        paramsTop[PITCH] = -.042f;
    } else if (name == "mal") {
        paramsTop[PITCH] = -.04f;
        paramsBottom[PITCH] = -.12f;
    } else if (name == "zoe") {
        paramsTop[ROLL] = -0.02f;
        paramsTop[PITCH] = 0.018f;
        paramsBottom[PITCH] = 0.02f;
    } else if (name == "jayne") {
        paramsTop[PITCH] = 0.025f;
        paramsBottom[PITCH] = 0.005f;
    } else if (name == "wash") {
        paramsTop[PITCH] = -.005f;
    }

    UpdateWithParams(paramsTop, paramsBottom);
}

void CameraCalibrate::UpdateWithParams(float _paramsTop[], float _paramsBottom[]){
    //update the parameter array
    for (int i = 0; i < NUM_PARAMS; i++) {
        ParamsTop[i] = _paramsTop[i];
        ParamsBottom[i] = _paramsBottom[i];
    }

    //then update all of the transforms based on the new params
    TransformsTop[0] =
        CoordFrame4D::rotation4D(CoordFrame4D::Y_AXIS,
                                 ParamsTop[PITCH]);
    TransformsTop[1] =
        CoordFrame4D::rotation4D(CoordFrame4D::X_AXIS,
                                 ParamsTop[ROLL]);

    TransformsBottom[0] =
            CoordFrame4D::rotation4D(CoordFrame4D::Y_AXIS,
                    ParamsBottom[PITCH]);
    TransformsBottom[1] =
            CoordFrame4D::rotation4D(CoordFrame4D::X_AXIS,
                    ParamsBottom[ROLL]);


}
