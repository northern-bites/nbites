#ifndef _WalkingConstants_h_DEFINED
#define _WalkingConstants_h_DEFINED

#include <vector>
#include <boost/shared_ptr.hpp>
#include "Kinematics.h"
using namespace Kinematics;

#include "motionconfig.h" // for cmake set debugging flags like MOTION_DEBUG

#ifdef DEBUG_MOTION
#  define WALK_DEBUG
#endif


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

enum StepType {
    REGULAR_STEP=0,
    //START_STEP,
    END_STEP,
    //NULL_STEP
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
    StepType type;
    bool zmpd;
    Step(const float _x, const float _y, const float _theta,
         const float _duration, const Foot _foot,
         const StepType _type = REGULAR_STEP)
        : x(_x),y(_y), theta(_theta), duration(_duration),
          foot(_foot),type(_type),zmpd(false){}
    // Copy constructor to allow changing reference frames:
    Step(const float new_x, const float new_y, const float new_theta,
         const boost::shared_ptr<Step> other)
        : x(new_x),y(new_y), theta(new_theta), duration(other->duration),
          foot(other->foot),type(other->type),zmpd(other->zmpd){}
    friend std::ostream& operator<< (std::ostream &o, const Step &s)
        {
            return o << "Step(" << s.x << "," << s.y << "," << s.theta
                     << ") in " << s.duration <<" secs with foot "
                     << s.foot << " and type " << s.type;
        }
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
//parameters, passed into constructor in this order:
public:
    float motion_frame_length_s;
    float bodyHeight;
    float hipOffsetX;
    float stepDuration; // seconds
    float doubleSupportFraction; //Fraction of time spent in double support
    float stepHeight; // in mm
    float footLengthX;
    float dblSupInactivePercentage; // time we aren't moving zmp in dbl support
    float leftSwingHipRollAddition; //angle added to hip during single suppor
    float rightSwingHipRollAddition; // in rad
    float leftZMPSwingOffsetY; //in mm, distance to offset zmp from center of foot
    float rightZMPSwingOffsetY;//in the Y direction (side to side)

//calculated from parameters
public:
    int stepDurationFrames; //one double + one single support
    int doubleSupportFrames; //num frames to spend in double support
    int singleSupportFrames; //num frames to spend in single support

    // There are no defaults for walking parameters. All of them need to be
    // inputted.
public:
    WalkingParameters(const float _motion_frame_length_s, const float _bh,
                      const float _hox, const float _dur,
                      const float _dblSupFrac, const float _stepHeight,
                      const float _footLengthX, const float _dblInactivePerc,
                      const float _lSwHRAdd,const float _rSwHRAdd,
                      const float _lZMPSwOffY,const float _rZMPSwOffY)
        :  motion_frame_length_s( _motion_frame_length_s),
           bodyHeight(_bh), hipOffsetX(_hox), stepDuration(_dur),
           doubleSupportFraction(_dblSupFrac),
           stepHeight(_stepHeight),footLengthX(_footLengthX),
           dblSupInactivePercentage(_dblInactivePerc),
           leftSwingHipRollAddition(_lSwHRAdd),
           rightSwingHipRollAddition(_rSwHRAdd),
           leftZMPSwingOffsetY(_lZMPSwOffY),rightZMPSwingOffsetY(_rZMPSwOffY)
        {
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

    //Returns the 20 body joints
    std::vector<float> getWalkStance() const {
        //cout << "getWalkStance" <<endl;
        //calculate the walking stance of the robot
        const float z = bodyHeight;
        const float x = hipOffsetX;
        const float ly = HIP_OFFSET_Y;
        const float ry = -HIP_OFFSET_Y;


        //just assume we start at zero
        float zeroJoints[LEG_JOINTS] = {0.0f,0.0f,0.0f,
                                        0.0f,0.0f,0.0f};
        //Use inverse kinematics to find the left leg angles
        ufvector3 lgoal = ufvector3(3);
        lgoal(0)=-x; lgoal(1) = ly; lgoal(2) = -z;
        IKLegResult lresult = Kinematics::dls(LLEG_CHAIN,lgoal,zeroJoints);
        std::vector<float> lleg_angles(lresult.angles, lresult.angles + LEG_JOINTS);

        //Use inverse kinematics to find the right leg angles
        ufvector3 rgoal = ufvector3(3);
        rgoal(0)=-x; rgoal(1) = ry; rgoal(2) = -z;
        IKLegResult rresult = Kinematics::dls(RLEG_CHAIN,rgoal,zeroJoints);
        std::vector<float> rleg_angles(rresult.angles, rresult.angles + LEG_JOINTS);

        std::vector<float> allJoints;

        //Make up something arbitrary for the arms
        const float larm[ARM_JOINTS] = {M_PI/2,M_PI/10,-M_PI/2,-M_PI/2};
        const float rarm[ARM_JOINTS] = {M_PI/2,-M_PI/10,M_PI/2,M_PI/2};
        const std::vector<float>larm_angles(larm,larm+ARM_JOINTS);
        const std::vector<float>rarm_angles(rarm,rarm+ARM_JOINTS);
        //now combine all the vectors together
        allJoints.insert(allJoints.end(),larm_angles.begin(),larm_angles.end());
        allJoints.insert(allJoints.end(),lleg_angles.begin(),lleg_angles.end());
        allJoints.insert(allJoints.end(),rleg_angles.begin(),rleg_angles.end());
        allJoints.insert(allJoints.end(),rarm_angles.begin(),rarm_angles.end());
        return allJoints;
    }


};


enum GaitID {
    DEFAULT_P = 0,
    GOALIE_P
};

const WalkingParameters DEFAULT_PARAMETERS
= WalkingParameters(0.02f,        // motion frame length - FIXME constant
                    310.0f,       // COM height
                    19.0f,        // hipOffsetX
                    0.5f,         // stepDuration
                    0.1f,         // fraction in double support mode
                    16.5f,        // stepHeight
                    10.0f,        // footLengthX
                    0.4f,         // zmp static percentage in double support
                    4.0f*TO_RAD,  // leftSwingHipRollAddition
                    4.0f*TO_RAD,  // rightSwingHipRollAddition
                    12.0f,        // leftZMPSwingOffestY,
                    12.0f);       // rightZMPSwingOffestY
const WalkingParameters GOALIE_PARAMETERS = DEFAULT_PARAMETERS;

const WalkingParameters WALK_PARAMS[] = {DEFAULT_PARAMETERS,GOALIE_PARAMETERS};

//Sensitivity to new walk vectors -- currently 0, giving maximum sensitivity
//when a new vector differs by more than these constants, the internal
//walk vector held in the StepGenerator changes
const float NEW_VECTOR_THRESH_MMS = 0.0f; //difference in speed in mm/second
const float NEW_VECTOR_THRESH_RADS = 0.0f; //difference in speed in radians/second






#endif
