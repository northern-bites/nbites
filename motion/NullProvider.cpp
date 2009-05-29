#include "NullProvider.h"

using namespace std;
#include "Kinematics.h"

NullProvider::NullProvider(boost::shared_ptr<Sensors> s,vector<bool> chain_mask)
    :MotionProvider(NULL_PROVIDER),
     sensors(s),
     currentStiffness(Kinematics::NUM_JOINTS,0.0f),
     lastStiffness(Kinematics::NUM_JOINTS,0.0f),
     chainMask(chain_mask),
     frozen(false), freezingOn(false), freezingOff(false), newCommand(false)
{

    pthread_mutex_init(&null_provider_mutex,NULL);
}
NullProvider::~NullProvider(){
    pthread_mutex_destroy(&null_provider_mutex);
}



void NullProvider::setActive(){
    if(frozen || freezingOn || freezingOff)
        active();
    else
        inactive();
}

void NullProvider::setCommand(const boost::shared_ptr<OnFreezeCommand> command){
    pthread_mutex_lock(&null_provider_mutex);
    nextCommand = command;
    pthread_mutex_unlock(&null_provider_mutex);
    freezingOn = true;
    newCommand = true;
    setActive();
}
void NullProvider::setCommand(const boost::shared_ptr<OffFreezeCommand> command){
    pthread_mutex_lock(&null_provider_mutex);
    nextCommand = command;
    pthread_mutex_unlock(&null_provider_mutex);
    freezingOff = true;
    newCommand = true;
    setActive();
}


void NullProvider::calculateNextJointsAndStiffnesses(){
    readNewStiffness();

    //transcode the appropriate stiffness and joint values
    vector<float> curMotionAngles = sensors->getMotionBodyAngles();

    for(unsigned int chain = 0; chain < Kinematics::NUM_CHAINS; chain++){
        if( !chainMask[chain] )
            continue;

        //Iterate through the 22 long list of stiff/joints and
        //copy them into chain specific vectors to be sent to
        //the motion provider super class, on a per chain basis
        vector<float> chainJoints(Kinematics::chain_lengths[chain]);
        vector<float> chainStiffness(Kinematics::chain_lengths[chain]);

        unsigned int startI = Kinematics::chain_first_joint[chain];
        unsigned int endI = Kinematics::chain_last_joint[chain];
        for(unsigned int joint = startI, chain_joint = 0;
            joint <= endI;
            joint++,chain_joint++){
            chainJoints[chain_joint] = curMotionAngles[joint];
            chainStiffness[chain_joint] = currentStiffness[joint];
        }
        setNextChainJoints(static_cast<Kinematics::ChainID>(chain),chainJoints);
        setNextChainStiffnesses(static_cast<Kinematics::ChainID>(chain),
                                chainStiffness);

    }

}


void NullProvider::readNewStiffness(){
    //parse any new commands
    pthread_mutex_lock(&null_provider_mutex);
    if(newCommand){
        currentStiffness = vector<float>(Kinematics::NUM_JOINTS,
                                         nextCommand->getStiffness());
        newCommand = false;
        //maybe change if we want to change duration of transition
        if(freezingOff){
            frozen = false;
            freezingOff = false;
        }else if(freezingOn){
            frozen = true;
            freezingOn = false;
        }
    }
    pthread_mutex_unlock(&null_provider_mutex);
}
