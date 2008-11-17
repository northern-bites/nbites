#ifndef _WalkingConstants_h_DEFINED
#define _WalkingConstants_h_DEFINED

#include "Kinematics.h"
using namespace Kinematics;

enum SupportMode{
    SUPPORTING=0,
    SWINGING
};


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
    float bodyHeight;
    float hipOffsetX;

    // There are no defaults for walking parameters. All of them need to be
    // inputted.

    WalkingParameters(const WalkingParameters &other)
        : bodyHeight(other.bodyHeight), hipOffsetX(other.hipOffsetX) {

    }

    WalkingParameters(const float _bh, const float _hox)
        : bodyHeight(_bh), hipOffsetX(_hox) {

    }
};

#endif
