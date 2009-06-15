
// This file is part of Man, a robotic perception, locomotion, and
// team strategy application created by the Northern Bites RoboCup
// team of Bowdoin College in Brunswick, Maine, for the Aldebaran
// Nao robot.
//
// Man is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// Man is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser Public License for more details.
//
// You should have received a copy of the GNU General Public License
// and the GNU Lesser Public License along with Man.  If not, see
// <http://www.gnu.org/licenses/>.

#ifndef _WalkingConstants_h_DEFINED
#define _WalkingConstants_h_DEFINED

#include <vector>
#include <boost/shared_ptr.hpp>
#include "Kinematics.h"
#include "InverseKinematics.h"

#include "motionconfig.h" // for cmake set debugging flags like MOTION_DEBUG
#ifdef DEBUG_MOTION
#  define WALK_DEBUG
#endif

#include "MotionConstants.h"


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
class Step{
public:
    float x;
    float y;
    float theta;
    float stepDuration;
    unsigned int stepDurationFrames;
    unsigned int doubleSupportFrames;
    unsigned int singleSupportFrames;
    Foot foot;
    StepType type;
    bool zmpd;
    Step(const float _x, const float _y, const float _theta,
         const float _duration, const float doubleSupportFraction,const Foot _foot,
         const StepType _type = REGULAR_STEP)
        : x(_x),y(_y), theta(_theta), stepDuration(_duration),
          foot(_foot),type(_type),zmpd(false)
        {
            updateFrameLengths(doubleSupportFraction);
        }
    // Copy constructor to allow changing reference frames:
    Step(const float new_x, const float new_y, const float new_theta,
         const boost::shared_ptr<Step> other)
        : x(new_x),y(new_y), theta(new_theta),
          stepDuration(other->stepDuration),
          stepDurationFrames(other->stepDurationFrames),
          doubleSupportFrames(other->doubleSupportFrames),
          singleSupportFrames(other->singleSupportFrames),
          foot(other->foot),type(other->type),zmpd(other->zmpd){}

    void updateFrameLengths(float doubleSupportFraction){

        //need to calculate how many frames to spend in double, single
        stepDurationFrames =
            static_cast<unsigned int>(stepDuration /
                                      MotionConstants::MOTION_FRAME_LENGTH_S);

        doubleSupportFrames =
            static_cast<unsigned int>(stepDuration *
                                      doubleSupportFraction/
                                      MotionConstants::MOTION_FRAME_LENGTH_S);
        singleSupportFrames = stepDurationFrames - doubleSupportFrames;
    }

    friend std::ostream& operator<< (std::ostream &o, const Step &s)
        {
            return o << "Step(" << s.x << "," << s.y << "," << s.theta
                     << ") in " << s.stepDuration <<" secs with foot "
                     << s.foot << " and type " << s.type;
        }
};

static const boost::shared_ptr<Step> EMPTY_STEP =
    boost::shared_ptr<Step>(new Step(0.0f,0.0f,0.0f,
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
    Kinematics::ChainID leg;
    LegConstants(const Kinematics::ChainID _leg){
        leg = _leg;
        switch(leg){
        case Kinematics::LLEG_CHAIN:
            hipOffsetY = Kinematics::HIP_OFFSET_Y;
            hipOffsetZ = -Kinematics::HIP_OFFSET_Z;
            break;
        case Kinematics::RLEG_CHAIN:
            hipOffsetY = -Kinematics::HIP_OFFSET_Y;
            hipOffsetZ = -Kinematics::HIP_OFFSET_Z;
            break;
        default:
            throw "Invalid ChainID passed to LegConstants in WalkingConstants";
        }
    }
};


static const float LARM_WALK_ANGLES[Kinematics::ARM_JOINTS] =
{M_PI_FLOAT/2.0f ,.26f,0.0f,0.0f};
static const float RARM_WALK_ANGLES[Kinematics::ARM_JOINTS] =
{M_PI_FLOAT/2.0f,-.26f,0.0f,0.0f};

//NOTE You must never set these variables except with a constructor
//or you must update single,doubleSupportFrames  manually with
// updateFrameLengths()
struct WalkingParameters{
//parameters, passed into constructor in this order:
public:
    float motion_frame_length_s;
    float bodyHeight;
    float hipOffsetX;
    float XAngleOffset;
    float stepDuration; // seconds
    float doubleSupportFraction; //Fraction of time spent in double support
    float stepHeight; // in mm
    float footLengthX;
    float dblSupInactivePercentage; // time we aren't moving zmp in dbl support
    float leftSwingHipRollAddition; //angle added to hip during single suppor
    float rightSwingHipRollAddition; // in rad
    float leftZMPSwingOffsetY; //in mm, distance to offset zmp from center of foot
    float rightZMPSwingOffsetY;//in the Y direction (side to side)
    float turnZMPOffsetY; // in mm per radians of turn
    float strafeZMPOffsetY; // in mm per mm of step->y
    float maxXSpeed;
    float maxYSpeed;
    float maxThetaSpeed;
    float sensorFeedback;
    float maxStiffness; //default stiffness
    float kneeStiffness; //knee stiffness when swinging
    float anklePitchStiffness; //ankle pitch stiffness
    float ankleRollStiffness; //ankle roll stiffnesses
    float armStiffness;
    float xOdoScale;
    float yOdoScale;
    float thetaOdoScale;
    float armAmplitude;   //amount, in rads, that the arms should swing
//calculated from parameters
public:
    int stepDurationFrames; //one double + one single support
    int doubleSupportFrames; //num frames to spend in double support
    int singleSupportFrames; //num frames to spend in single support

    // There are no defaults for walking parameters. All of them need to be
    // inputted.
public:
    WalkingParameters(const float _motion_frame_length_s, const float _bh,
                      const float _hox, const float _yao, const float _dur,
                      const float _dblSupFrac, const float _stepHeight,
                      const float _footLengthX, const float _dblInactivePerc,
                      const float _lSwHRAdd,const float _rSwHRAdd,
                      const float _lZMPSwOffY,const float _rZMPSwOffY,
                      const float _tZMPOffY, const float _sZMPOffY,
                      const float maxx_mms, const float maxy_mms,
                      const float maxtheta_rads,
                      const float _sensorFeedback, const float _maxStiffness,
                      const float _kneeStiffness, const float _anklePitchStiffness,
                      const float _ankleRollStiffness,
                      const float _armStiffness, const float _xOdoScale,
                      const float _yOdoScale, const float _thetaOdoScale,
                      const float _armAmplitude)
        :  motion_frame_length_s( _motion_frame_length_s),
           bodyHeight(_bh), hipOffsetX(_hox), XAngleOffset(_yao),
           stepDuration(_dur),
           doubleSupportFraction(_dblSupFrac),
           stepHeight(_stepHeight),footLengthX(_footLengthX),
           dblSupInactivePercentage(_dblInactivePerc),
           leftSwingHipRollAddition(_lSwHRAdd),
           rightSwingHipRollAddition(_rSwHRAdd),
           leftZMPSwingOffsetY(_lZMPSwOffY),rightZMPSwingOffsetY(_rZMPSwOffY),
           turnZMPOffsetY(_tZMPOffY), strafeZMPOffsetY(_sZMPOffY),
           maxXSpeed(maxx_mms),maxYSpeed(maxy_mms),maxThetaSpeed(maxtheta_rads),
           sensorFeedback(_sensorFeedback), maxStiffness(_maxStiffness),
           kneeStiffness(_kneeStiffness), 
           anklePitchStiffness(_anklePitchStiffness),
           ankleRollStiffness(_ankleRollStiffness),
           armStiffness(_armStiffness), xOdoScale(_xOdoScale),
           yOdoScale(_yOdoScale), thetaOdoScale(_thetaOdoScale),
           armAmplitude(_armAmplitude)
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
    std::vector<float> * getWalkStance() const {
        //cout << "getWalkStance" <<endl;
        //calculate the walking stance of the robot
        const float z = bodyHeight;
        const float x = hipOffsetX;
        const float ly = Kinematics::HIP_OFFSET_Y;
        const float ry = -Kinematics::HIP_OFFSET_Y;


        //just assume we start at zero
        float zeroJoints[Kinematics::LEG_JOINTS] = {0.0f,0.0f,0.0f,
                                                    0.0f,0.0f,0.0f};
        //Use inverse kinematics to find the left leg angles
        NBMath::ufvector3 lgoal = NBMath::ufvector3(3);
        lgoal(0)=-x; lgoal(1) = ly; lgoal(2) = -z;
        Kinematics::IKLegResult lresult =
            Kinematics::dls(Kinematics::LLEG_CHAIN,
                            lgoal, zeroJoints);
        std::vector<float> lleg_angles(lresult.angles,
                                       lresult.angles +
                                       Kinematics::LEG_JOINTS);

        //Use inverse kinematics to find the right leg angles
        NBMath::ufvector3 rgoal = NBMath::ufvector3(3);
        rgoal(0)=-x; rgoal(1) = ry; rgoal(2) = -z;
        Kinematics::IKLegResult rresult =
            Kinematics::dls(Kinematics::RLEG_CHAIN,
                            rgoal, zeroJoints);
        std::vector<float> rleg_angles(rresult.angles,
                                       rresult.angles +
                                       Kinematics::LEG_JOINTS);

        //X AXIS angle offset:
        const unsigned int leftHipPitchIndex
            = Kinematics::L_HIP_PITCH -
            Kinematics::chain_first_joint[Kinematics::LLEG_CHAIN];
        const unsigned int rightHipPitchIndex
            = Kinematics::R_HIP_PITCH -
            Kinematics::chain_first_joint[Kinematics::RLEG_CHAIN];

        lleg_angles[leftHipPitchIndex]  -= XAngleOffset;
        rleg_angles[rightHipPitchIndex] -= XAngleOffset;

        std::vector<float> * allJoints = new std::vector<float>();

        const std::vector<float>larm_angles(LARM_WALK_ANGLES,
                                            LARM_WALK_ANGLES + Kinematics::ARM_JOINTS);
        const std::vector<float>rarm_angles(RARM_WALK_ANGLES,
                                            RARM_WALK_ANGLES +Kinematics::ARM_JOINTS);
        //now combine all the vectors together
        allJoints->insert(allJoints->end(),larm_angles.begin(),larm_angles.end());
        allJoints->insert(allJoints->end(),lleg_angles.begin(),lleg_angles.end());
        allJoints->insert(allJoints->end(),rleg_angles.begin(),rleg_angles.end());
        allJoints->insert(allJoints->end(),rarm_angles.begin(),rarm_angles.end());
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
                    0.0f,          // XAngleOffset
                    0.5f,         // stepDuration
                    0.1f,         // fraction in double support mode
                    16.5f,        // stepHeight
                    10.0f,        // footLengthX
                    0.4f,         // zmp static percentage in double support
                    4.0f*TO_RAD,  // leftSwingHipRollAddition
                    4.0f*TO_RAD,  // rightSwingHipRollAddition
                    12.0f,        // leftZMPSwingOffestY,
                    12.0f,        // rightZMPSwingOffestY
                    6.6f,         // turnZMPOffsetY
                    0.010f,       // strafeZMPOffsetY
                    100.0f,       // max speed x (mm/s)
                    50.0f,        // max speed y (mm/s)
                    0.50f,        // max speed theta (rad/s)
                    0.0f,         // sensor feedback
                    0.85f,        // max stiffness
                    0.85f,        // knee stiffness
                    0.85f,        // ankle Pitch stiffness
                    0.85f,        // ankle Roll stiffness
                    0.2f,         // arm stiffness
                    1.0f,         // xOdoScale
                    1.0f,         // yOdoScale
                    1.0f,         //thetaOdoScale
                    0.0f);        //armAmplitude
const WalkingParameters GOALIE_PARAMETERS = DEFAULT_PARAMETERS;

const WalkingParameters WALK_PARAMS[] = {DEFAULT_PARAMETERS,GOALIE_PARAMETERS};

//Sensitivity to new walk vectors -- currently 0, giving maximum sensitivity
//when a new vector differs by more than these constants, the internal
//walk vector held in the StepGenerator changes
const float NEW_VECTOR_THRESH_MMS = 0.0f; //difference in speed in mm/second
const float NEW_VECTOR_THRESH_RADS = 0.0f; //difference in speed in radians/second






#endif
