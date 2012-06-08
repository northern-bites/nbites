#include "NullProvider.h"

using namespace std;

NullProvider::NullProvider(boost::shared_ptr<Sensors> s,
                           const bool chain_mask[Kinematics::NUM_CHAINS])
    :MotionProvider(NULL_PROVIDER),
     sensors(s),
     nextStiffness(Kinematics::NUM_JOINTS,0.0f),
     lastStiffness(Kinematics::NUM_JOINTS,0.0f),
     frozen(false), freezingOn(false), freezingOff(false), newCommand(false),
     doOnce(false) //Hack
{
    pthread_mutex_init(&null_provider_mutex,NULL);
    for(unsigned int c =0; c <  Kinematics::NUM_CHAINS; c++){
        chainMask[c] = chain_mask[c];
    }
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

void NullProvider::setCommand(const boost::shared_ptr<FreezeCommand> command){
    pthread_mutex_lock(&null_provider_mutex);
    nextCommand = command;
    pthread_mutex_unlock(&null_provider_mutex);
    freezingOn = true;
    newCommand = true;
    setActive();
}
void NullProvider::setCommand(const boost::shared_ptr<UnfreezeCommand> command){
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
    vector<float> curMotionAngles = sensors->getBodyAngles();

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
    pthread_mutex_lock(&null_provider_mutex);
    if (freezingOff && doOnce){
        frozen = false;
        freezingOff = false;
		freezingOn = false;
        doOnce = false;
    }

    if(newCommand){
        nextStiffness = vector<float>(Kinematics::NUM_JOINTS,
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
    pthread_mutex_unlock(&null_provider_mutex);
}
