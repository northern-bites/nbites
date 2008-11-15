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
    LegConstants(ChainID _leg){
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

    //Default constructor, sets all the values to their defaults
    WalkingParameters(){
        bodyHeight = 310; //millimeters
        hipOffsetX = 0.0f;
    }
    //Learning constructor: allows creation of new parameter sets at runtime
    WalkingParameters(float _bh, float _hox){
        bodyHeight = _bh;
        hipOffsetX = _hox;
    }
};

#endif
