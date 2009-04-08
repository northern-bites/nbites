
#include "StiffnessCommand.h"
#include "NBMath.h"

using namespace Kinematics;
using namespace std;

StiffnessCommand::StiffnessCommand()
    : MotionCommand(MotionConstants::STIFFNESS)
{
    init();
}

StiffnessCommand::StiffnessCommand(const float stiffness)
    : MotionCommand(MotionConstants::STIFFNESS)
{
    init();
    setChainStiffness(HEAD_CHAIN,stiffness);
    setChainStiffness(LARM_CHAIN,stiffness);
    setChainStiffness(LLEG_CHAIN,stiffness);
    setChainStiffness(RLEG_CHAIN,stiffness);
    setChainStiffness(RARM_CHAIN,stiffness);

}

StiffnessCommand::StiffnessCommand(const ChainID id,
                                   const float chain_stiffness)
    : MotionCommand(MotionConstants::STIFFNESS)
{
    init();
    setChainStiffness(id,chain_stiffness);
}



StiffnessCommand::~StiffnessCommand()
{
}


const  std::vector<float>*
StiffnessCommand::getChainStiffness(const Kinematics::ChainID chainID) const{
    return stiffnesses[chainID];

}


const float clipStiffness(const float oldval){
    if (oldval == StiffnessCommand::DECOUPLED ||
         oldval == StiffnessCommand::NOT_SET ){
        //check for special value
        return oldval;
    }

    //otherwise check if it is in the correct range
    return NBMath::clip(oldval,0.0f,1.0f);

}

void StiffnessCommand::setChainStiffness(const Kinematics::ChainID chainID,
                                         const float chain_stiffness)
{
    const float safe_stiffness = clipStiffness(chain_stiffness);

    if(stiffnesses[chainID]){
        delete stiffnesses[chainID];
    }
    stiffnesses[chainID]=  new vector<float>(chain_lengths[chainID],
                                                    safe_stiffness);

}
