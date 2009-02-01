
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
using namespace std;

#include <boost/shared_ptr.hpp>
#include <boost/numeric/ublas/matrix.hpp>
using namespace boost::numeric;

#include "WalkingConstants.h"
#include "CoordFrame.h"
#include "Kinematics.h"

using namespace Kinematics;

//DEBUG Switches:
#ifdef WALK_DEBUG
//#define DEBUG_WALKING_STATE_TRANSITIONS
//#define DEBUG_WALKING_GOAL_CONTINUITY
#define DEBUG_WALKING_LOCUS_LOGGING
#define DEBUG_WALKING_DEST_LOGGING
#endif
class WalkingLeg  {
public:
    WalkingLeg(ChainID id, const WalkingParameters * walkP);
    ~WalkingLeg();

    vector <float> tick(boost::shared_ptr<Step> step,
                        boost::shared_ptr<Step> swing_src,
                        boost::shared_ptr<Step> _suppoting,
                        ufmatrix3 fc_Transform);

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
private:
    //Execution methods, get called depending on which state the leg is in
    vector <float> supporting(ufmatrix3 fc_Transform);//float dest_x, float dest_y);
    vector <float> swinging(ufmatrix3 fc_Transform);//float dest_x, float dest_y);

    //FSA methods
    void setState(SupportMode newState);
    void switchToNextState();
    SupportMode nextState();
    bool shouldSwitchStates();
    bool firstFrame(){return frameCounter == 0;}
    void debugProcessing();

    float getHipHack();
    float cycloidy(float theta);
    float cycloidx(float theta);

    inline ChainID getOtherLegChainID();

private:
    //FSA Attributes
    SupportMode state, lastState,lastDiffState;
    SupportMode supportMode; //soon to be deprecated
    int frameCounter;

    //destination attributes
    boost::shared_ptr<Step> cur_dest, swing_src, swing_dest;

    //Leg Attributes
    ChainID chainID; //keep track of which leg this is
    const WalkingParameters *walkParams;
    float lastJoints[LEG_JOINTS];
    ufvector3 goal;
    ufvector3 last_goal;
    int leg_sign; //-1 for right leg, 1 for left leg
    string leg_name;
#ifdef DEBUG_WALKING_LOCUS_LOGGING
    FILE * locus_log;
#endif
#ifdef DEBUG_WALKING_DEST_LOGGING
    FILE * dest_log;
#endif
};

#endif
