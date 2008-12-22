#ifndef _WalkingConstants_h_DEFINED
#define _WalkingConstants_h_DEFINED

#include <boost/shared_ptr.hpp>
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

/**
 * Simple container to hold information about future steps.
 */
struct Step{
    float x;
    float y;
    float theta;
    float duration;
    Foot foot;
    Step(const float _x, const float _y, const float _theta,
         const float _duration, const Foot _foot)
        : x(_x),y(_y), theta(_theta), duration(_duration), foot(_foot){}
};

static const boost::shared_ptr<Step> EMPTY_STEP =
    boost::shared_ptr<Step>(new Step(0.0f,0.0f,
                                     0.0f,0.0f,
                                     LEFT_FOOT));

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

//NOTE You must never set these variables except with a constructor
//or you must update single,doubleSupportFrames  manually with
// updateFrameLengths()
struct WalkingParameters{
public:
    float motion_frame_length_s;
    float bodyHeight;
    float hipOffsetX;
    float stepDuration; // seconds
    float doubleSupportFraction; //Fraction of time spent in double support
    float stepHeight; // in mm
    int stepDurationFrames; //one double + one single support
    int doubleSupportFrames; //num frames to spend in double support
    int singleSupportFrames; //num frames to spend in single support

    // There are no defaults for walking parameters. All of them need to be
    // inputted.
public:
    WalkingParameters(const float _motion_frame_length_s, const float _bh,
                      const float _hox, const float _dur,
                      const float _dblSupFrac, const float _stepHeight)
        :  motion_frame_length_s( _motion_frame_length_s),
           bodyHeight(_bh), hipOffsetX(_hox), stepDuration(_dur),
           doubleSupportFraction(_dblSupFrac),
           stepHeight(_stepHeight) {

        updateFrameLengths();
    }

    void updateFrameLengths(){

        //need to calculate how many frames to spend in double, single
        stepDurationFrames = static_cast<int>(stepDuration /
                                              motion_frame_length_s);

        doubleSupportFrames = static_cast<int>(stepDuration *
                                               doubleSupportFraction/
                                               motion_frame_length_s);
        singleSupportFrames = stepDurationFrames - doubleSupportFrames;
    }

};

#endif
