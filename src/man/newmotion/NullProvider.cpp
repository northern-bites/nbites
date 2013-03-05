#include "NullProvider.h"

namespace man
{
namespace motion
{
NullProvider::NullProvider(const bool chain_mask[Kinematics::NUM_CHAINS])
    : MotionProvider(NULL_PROVIDER),
      nextStiffness(Kinematics::NUM_JOINTS,0.0f),
      lastStiffness(Kinematics::NUM_JOINTS,0.0f),
      frozen(false), freezingOn(false), freezingOff(false), newCommand(false),
      doOnce(false) //Hack
{
    for(unsigned int c =0; c <  Kinematics::NUM_CHAINS; c++){
        chainMask[c] = chain_mask[c];
    }
}
NullProvider::~NullProvider(){
}

void NullProvider::setActive(){
    if(frozen || freezingOn || freezingOff)
        active();
    else
        inactive();
}

void NullProvider::setCommand(const boost::shared_ptr<FreezeCommand> command){
    nextCommand = command;
    freezingOn = true;
    newCommand = true;
    setActive();
}
void NullProvider::setCommand(const boost::shared_ptr<UnfreezeCommand> command){
    nextCommand = command;
    freezingOff = true;
    newCommand = true;
    setActive();
}


void NullProvider::calculateNextJointsAndStiffnesses(
    std::vector<float>&      sensorAngles,
    messages::InertialState& sensorInertials,
    messages::FSR&           sensorFSRs)
{
    readNewStiffness();

    //transcode the appropriate stiffness and joint values
    for(unsigned int chain = 0; chain < Kinematics::NUM_CHAINS; chain++){
        if( !chainMask[chain] )
            continue;

        //Iterate through the 22 long list of stiff/joints and
        //copy them into chain specific vectors to be sent to
        //the motion provider super class, on a per chain basis
	std::vector<float> chainJoints(Kinematics::chain_lengths[chain]);
	std::vector<float> chainStiffness(Kinematics::chain_lengths[chain]);

        unsigned int startI = Kinematics::chain_first_joint[chain];
        unsigned int endI = Kinematics::chain_last_joint[chain];
        for(unsigned int joint = startI, chain_joint = 0;
            joint <= endI;
            joint++,chain_joint++){
            chainJoints[chain_joint] = sensorAngles[joint];
            chainStiffness[chain_joint] = nextStiffness[joint];
        }
        setNextChainJoints(static_cast<Kinematics::ChainID>(chain),chainJoints);
        setNextChainStiffnesses(static_cast<Kinematics::ChainID>(chain),
                                chainStiffness);

    }
    setActive();
}


void NullProvider::readNewStiffness(){
    //parse any new commands
    if (freezingOff && doOnce){
        frozen = false;
        freezingOff = false;
		freezingOn = false;
        doOnce = false;
    }

    if(newCommand){
        nextStiffness = std::vector<float>(Kinematics::NUM_JOINTS,
                                         nextCommand->getStiffness());
        newCommand = false;
        //maybe change if we want to change duration of transition
        if(freezingOff){
            doOnce = true;
        }else if(freezingOn){
            frozen = true;
            freezingOn = false;
        }
    }
}
}
}
