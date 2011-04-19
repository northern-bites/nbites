#include "WalkingArm.h"

using namespace Kinematics;
using boost::shared_ptr;
using namespace std;

WalkingArm::WalkingArm(const MetaGait * _gait,ChainID id)
    :state(SUPPORTING),
     chainID(id),
     gait(_gait),
     frameCounter(0),
     startStep(true),
     lastStepType(REGULAR_STEP)
{}

WalkingArm::~WalkingArm(){}


ArmJointStiffTuple WalkingArm::tick(shared_ptr<Step> supportStep){
    singleSupportFrames = supportStep->singleSupportFrames;
    doubleSupportFrames = supportStep->doubleSupportFrames;

    vector<float> armJoints = (chainID == LARM_CHAIN ?
                               vector<float>(LARM_WALK_ANGLES,
                                             &LARM_WALK_ANGLES[ARM_JOINTS]):
                               vector<float>(RARM_WALK_ANGLES,
                                             &RARM_WALK_ANGLES[ARM_JOINTS]));

    armJoints[0] += getShoulderPitchAddition(supportStep);
    vector<float> armStiffnesses(ARM_JOINTS,gait->stiffness[WP::ARM]);
	armStiffnesses[0] = gait->stiffness[WP::ARM_PITCH];

    frameCounter++;
    for(unsigned int i = 0; shouldSwitchStates() && i < 2; i++){
        switchToNextState();
        lastStepType = supportStep->type;
    };

    return ArmJointStiffTuple(armJoints,armStiffnesses);
}

/*
 * Currently, the arms only move in the forward direction by modulating the
 * shoulderPitch
 */
const float WalkingArm::getShoulderPitchAddition(shared_ptr<Step> supportStep){
    float direction = 1.0f; //forward = negative
    float percentComplete = 0.0f;
    switch(state){
    case SUPPORTING:
        //When the leg on this side is supporting (i.e. swinging back)
        //this arm should swing forward with the foot from the other side
        direction = -1.0f;
        percentComplete = static_cast<float>(frameCounter)/
            static_cast<float>(singleSupportFrames);
        break;
    case SWINGING:
        //When the leg on this side is swinging (i.e. swinging forward)
        //this arm should swing backward with the foot from the other side
        direction = 1.0f;
        percentComplete = static_cast<float>(frameCounter)/
            static_cast<float>(singleSupportFrames);
        break;
    case DOUBLE_SUPPORT:
        //When the leg on this side is in non-persistent double support,
        //it was just recently SUPPORTING, so this arm was swinging back
        //so during double support it should stay back
        direction = -1.0f;
        percentComplete = 1.0f;
        break;
    case PERSISTENT_DOUBLE_SUPPORT:
        //When the leg on this side is in persistent double support,
        //it was just recently SWINGING, so this arm was swinging forwward
        //so during persistent double support it should stay forward
        direction = 1.0f;
        percentComplete = 1.0f;
        break;
    }

    float start = -direction*gait->arm[WP::AMPLITUDE];
    float end = direction*gait->arm[WP::AMPLITUDE];

    //We need to intelligently deal with non-regular steps
    //Since end steps are employed in both the starting and stopping contexts
    //and since we don't pay attention to the src, dest, etc for the swinging
    //leg as in WalkingLeg, we need to make sure we do the right thing in each case
    //The manipulations of end, start targets for the pitch are a bit hackish
    //but they work.
    //also note the problem of getting two end steps in a row, since in such a case
    //the arms should be held at their default values
    if(supportStep->type== END_STEP){
        if(lastStepType == END_STEP){
            start = end = 0.0f;
        }else if(startStep){
            start = 0.0f;
            end = -end;
        }else{
            end = 0.0f;
            start = -start;
        }
        //all end steps should be entirely composed of DOUBLE SUPPORT!
        percentComplete = static_cast<float>(frameCounter)/
            static_cast<float>(doubleSupportFrames);
    }

    //Even though we already calcualted percent complete, we should really
    //have a more gradual arm motion, which we can do by employing a cycloid

    const float theta = percentComplete*2.0f*M_PI_FLOAT;
    const float percentToDest = NBMath::cycloidx(theta)/(2.0f*M_PI_FLOAT);

    return start + percentToDest*(end - start);

}

void WalkingArm::startRight(){
    if(chainID == LARM_CHAIN){
        //we will start walking first by swinging right leg (not this leg), so
        //we want persistent double support
        setState(PERSISTENT_DOUBLE_SUPPORT);
    }else{
        setState(DOUBLE_SUPPORT);
    }
    startStep = true;
    lastStepType = REGULAR_STEP;
}
void WalkingArm::startLeft(){
    if(chainID == LARM_CHAIN){
        //we will start walking first by swinging left leg (this leg), so
        //we want double, not persistent, support
        setState(DOUBLE_SUPPORT);
    }else{
        setState(PERSISTENT_DOUBLE_SUPPORT);
    }
    startStep = true;
    lastStepType = REGULAR_STEP;
}

bool WalkingArm::shouldSwitchStates(){
    switch(state){
    case SUPPORTING:
        return frameCounter >= singleSupportFrames;
    case SWINGING:
        return frameCounter >= singleSupportFrames;
    case DOUBLE_SUPPORT:
        return frameCounter >= doubleSupportFrames;
    case PERSISTENT_DOUBLE_SUPPORT:
        return frameCounter >= doubleSupportFrames;
    }

    throw "Non existent state";
    return false;
}

void WalkingArm::switchToNextState(){
    setState(nextState());
    startStep = false;
}

SupportMode WalkingArm::nextState(){
    switch(state){
    case SUPPORTING:
        return DOUBLE_SUPPORT;
    case SWINGING:
        return PERSISTENT_DOUBLE_SUPPORT;
    case DOUBLE_SUPPORT:
        return SWINGING;
    case PERSISTENT_DOUBLE_SUPPORT:
        return SUPPORTING;
    default:
        throw "Non existent state";
        return PERSISTENT_DOUBLE_SUPPORT;
    }
}

void WalkingArm::setState(SupportMode newState){
    state = newState;
    frameCounter = 0;
}
