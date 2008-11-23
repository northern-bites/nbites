#ifndef _WalkingConstants_h_DEFINED
#define _WalkingConstants_h_DEFINED

#include "Kinematics.h"
using namespace Kinematics;

enum SupportMode{
    SUPPORTING=0,
    SWINGING,
    DOUBLE_SUPPORT,
    PERSISTENT_DOUBLE_SUPPORT
};

enum Foot {
    LEFT_FOOT = 0,
    RIGHT_FOOT
};



/*
struct WalkLegsResult{
    vector<float> left;
    vector<float> right;
    WalkLegsResult(vector<float> r, vector<float> l)
        : left(l), right(r){}
}
*/

struct LegConstants{
    float hipOffsetY;
    float hipOffsetZ;
    ChainID leg;
    LegConstants(const ChainID _leg){
        leg = _leg;
        switch(leg){
        case LLEG_CHAIN:
            hipOffsetY = HIP_OFFSET_Y;
            hipOffsetZ = -HIP_OFFSET_Z;
            break;
        case RLEG_CHAIN:
            hipOffsetY = -HIP_OFFSET_Y;
            hipOffsetZ = -HIP_OFFSET_Z;
            break;
        default:
            throw "Invalid ChainID passed to LegConstants in WalkingConstants";
        }
    }
};

struct WalkingParameters{
    float motion_frame_length_s;
    float bodyHeight;
    float hipOffsetX;
    float stepDuration;
    float stepHeight; // in mm

    // There are no defaults for walking parameters. All of them need to be
    // inputted.

    WalkingParameters(const float _motion_frame_length_s, const float _bh,
                      const float _hox, const float _dur,
                      const float _stepHeight)
        :  motion_frame_length_s( _motion_frame_length_s),
           bodyHeight(_bh), hipOffsetX(_hox), stepDuration(_dur),
           stepHeight(_stepHeight) {

    }
};

#endif
