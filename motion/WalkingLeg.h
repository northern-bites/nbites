
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

#ifndef _WalkingLeg_h_DEFINED
#define _WalkingLeg_h_DEFINED

/**
 *
 * This class implements the basic strucutre of an FSA to model the 
 * state transitions of a robot's leg during walking. Particularly,
 * it switches between single and double support modes.
 *
 * This class is designed to switch states automatically, without external
 * input. The only time it needs external input is when it should start
 * Stopping is handled implicitly by ceasing calls to tick(), followed
 * eventually (potentially?) by a call to startRight/startLeft
 *
 * Take note of the autonomous state switching because it will be critical
 * to make sure the dest_x, dest_y sent to this class are calculated from
 * ZMP values in sync with the state transitions of this class.
 *
 * Currently, the state transitions of this class are based on the walking
 * parameters pointer stored locally. This means that items such as step
 * length, etc are currently not variable per step. This will change eventually
 *
 * Eventually, the tick() method should probably be passed something like a
 * LocalStep, which is a step defined in the c frame. If this step has attr.
 * such as destination for the legs, duration, etc, we should be able to move
 * forward with steps of variable length, etc 
 *
 */

#include <cstdio>
#include <string>
#include <vector>

#include <boost/shared_ptr.hpp>
#include <boost/tuple/tuple.hpp>
#include "WalkingConstants.h"
#include "WalkParameters.h"
#include "CoordFrame.h"
#include "Kinematics.h"
#include "NBMatrixMath.h"
#include  "Sensors.h"

//DEBUG Switches:
#ifdef WALK_DEBUG
//#define DEBUG_WALKING_STATE_TRANSITIONS
//#define DEBUG_WALKING_GOAL_CONTINUITY
#  define DEBUG_WALKING_LOCUS_LOGGING
#  define DEBUG_WALKING_DEST_LOGGING
#endif

typedef boost::tuple<std::vector<float>,
                     std::vector<float> > LegJointStiffTuple;


enum JointStiffIndex {
    JOINT_INDEX,
    STIFF_INDEX
};

struct WalkCycle{
    boost::shared_ptr<Step> supportStep;
    NBMath::ufvector3 swing_src;
    NBMath::ufvector3 swing_dest;
    const WalkParameters * params;
};

class WalkingLeg  {
public:
    WalkingLeg(    boost::shared_ptr<Sensors> s,
                   Kinematics::ChainID id);
    ~WalkingLeg();

    LegJointStiffTuple tick(boost::shared_ptr<Step> step,
                             boost::shared_ptr<Step> swing_src,
                             boost::shared_ptr<Step> _suppoting,
                             NBMath::ufmatrix3 fc_Transform);

    void setSteps(boost::shared_ptr<Step> _swing_src,
                  boost::shared_ptr<Step> _swing_dest,
                  boost::shared_ptr<Step> _suppoting);

    //Hopefully these never need to get called (architecturally).
    //Instead, use methods like startLeft, right etc
    //void setSupportMode(SupportMode newMode){setState(newMode);}
    //void switchSupportMode() {nextState();}

    //methods to setup starting the walk
    void startLeft();
    void startRight();

    //Public FSA methods
    SupportMode getSupportMode(){return state;}
    //True if the next call to tick() will be in a different support mode
    bool isSwitchingSupportMode(){return firstFrame();}

    bool stateIsDoubleSupport(){
        return state == DOUBLE_SUPPORT ||
            state == PERSISTENT_DOUBLE_SUPPORT;
    };
    bool isSupporting(){
        return state == DOUBLE_SUPPORT ||
            state == PERSISTENT_DOUBLE_SUPPORT || state == SUPPORTING;
    };
    void resetGait(const WalkParameters * _wp);

    std::vector<float> getOdoUpdate();
    void computeOdoUpdate();

    static std::vector<float>
    getAnglesFromGoal(const Kinematics::ChainID chainID,
                      const NBMath::ufvector3 & goal,
                      const float stance[WP::LEN_STANCE_CONFIG]);

private:
    //Execution methods, get called depending on which state the leg is in
    LegJointStiffTuple supporting(NBMath::ufmatrix3 fc_Transform);
    LegJointStiffTuple swinging(NBMath::ufmatrix3 fc_Transform);

    //FSA methods
    void setState(SupportMode newState);
    void switchToNextState();
    SupportMode nextState();
    bool shouldSwitchStates();
    bool firstFrame(){return frameCounter == 0;}
    void assignStateTimes(boost::shared_ptr<Step> step);
    void debugProcessing();

//hack
public:
    const float getFootRotation();
private:
    const float getHipYawPitch();
    const std::vector<float>
    getStiffnesses();
    const boost::tuple<const float,const float>getHipHack(const float HYPAngle);
    const float cycloidy(float theta);
    const float cycloidx(float theta);


    inline Kinematics::ChainID getOtherLegChainID();

private:
    boost::shared_ptr<Sensors> sensors;
    //FSA Attributes
    SupportMode state;
    SupportMode supportMode; //soon to be deprecated
    unsigned int frameCounter;
    unsigned int doubleSupportFrames;
    unsigned int singleSupportFrames;
    unsigned int cycleFrames;

    //destination attributes
    boost::shared_ptr<Step> cur_dest, swing_src, swing_dest,support_step;

    //Leg Attributes
    Kinematics::ChainID chainID; //keep track of which leg this is
    const WalkParameters * walkParams;
    float lastJoints[Kinematics::LEG_JOINTS];
    NBMath::ufvector3 goal;
    NBMath::ufvector3 last_goal;
    float lastRotation;
    std::vector<float> odoUpdate;
    int leg_sign; //-1 for right leg, 1 for left leg
    std::string leg_name;
#ifdef DEBUG_WALKING_LOCUS_LOGGING
    FILE * locus_log;
#endif
#ifdef DEBUG_WALKING_DEST_LOGGING
    FILE * dest_log;
#endif
};

#endif
