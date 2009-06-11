#include "WalkingArm.h"


using namespace Kinematics;
using boost::shared_ptr;
using namespace std;

WalkingArm::WalkingArm(ChainID id)
    :state(SUPPORTING),
     chainID(id),
     walkParams(NULL),
     frameCounter(0)
{}

WalkingArm::~WalkingArm(){}




ArmJointStiffTuple WalkingArm::tick(shared_ptr<Step> supportStep){
    singleSupportFrames = supportStep->singleSupportFrames;
    doubleSupportFrames = supportStep->doubleSupportFrames;
    ArmJointStiffTuple result;
    switch(state){
    case SUPPORTING:
    case SWINGING:
        result = swinging();
        break;
    case DOUBLE_SUPPORT:
    case PERSISTENT_DOUBLE_SUPPORT:
        result = supporting();
        break;
    }
    frameCounter++;
    for(unsigned int  i = 0; shouldSwitchStates() && i < 2; i++,switchToNextState());
    return result;
}

ArmJointStiffTuple WalkingArm::swinging(){
    const vector<float> armStiffnesses(ARM_JOINTS,walkParams->armStiffness);
    if(chainID == LARM_CHAIN){
        const vector<float> armJoints(LARM_WALK_ANGLES,
                                      &LARM_WALK_ANGLES[ARM_JOINTS]);
        return ArmJointStiffTuple(armJoints,armStiffnesses);
    }else if(chainID == RARM_CHAIN){
        const vector<float> armJoints(RARM_WALK_ANGLES,
                                      &RARM_WALK_ANGLES[ARM_JOINTS]);
        return ArmJointStiffTuple(armJoints,armStiffnesses);
    }
    cout << "Invalid chainID in WalkingArm::swinging"<<endl;
    return ArmJointStiffTuple(vector<float>(ARM_JOINTS,0.0f),armStiffnesses);
}

ArmJointStiffTuple WalkingArm::supporting(){
    const vector<float> armStiffnesses(ARM_JOINTS,walkParams->armStiffness);
    if(chainID == LARM_CHAIN){
        const vector<float> armJoints(LARM_WALK_ANGLES,
                                      &LARM_WALK_ANGLES[ARM_JOINTS]);

        return ArmJointStiffTuple(armJoints,armStiffnesses);
    }else if(chainID == RARM_CHAIN){
        const vector<float> armJoints(RARM_WALK_ANGLES,
                                      &RARM_WALK_ANGLES[ARM_JOINTS]);
        return ArmJointStiffTuple(armJoints,armStiffnesses);
    }
    cout << "Invalid chainID in WalkingArm::swinging"<<endl;
    return ArmJointStiffTuple(vector<float>(ARM_JOINTS,0.0f),armStiffnesses);
}


void WalkingArm::startRight(){
    if(chainID == LARM_CHAIN){
        //we will start walking first by swinging right leg (not this leg), so
        //we want persistent double support
        setState(PERSISTENT_DOUBLE_SUPPORT);
    }else{
        setState(DOUBLE_SUPPORT);
    }
}
void WalkingArm::startLeft(){
    if(chainID == LARM_CHAIN){
        //we will start walking first by swinging left leg (this leg), so
        //we want double, not persistent, support
        setState(DOUBLE_SUPPORT);
    }else{
        setState(PERSISTENT_DOUBLE_SUPPORT);
    }
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



void WalkingArm::resetGait(const WalkingParameters * _wp){
    walkParams = _wp;
}
