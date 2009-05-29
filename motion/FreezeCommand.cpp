#include "FreezeCommand.h"



FreezeCommand::FreezeCommand(float stiffness)
    :MotionCommand(MotionConstants::FREEZE)
{
    setChainList();
    targetStiffness = stiffness;
}

void FreezeCommand::setChainList() {
    chainList.insert(chainList.end(),
                     MotionConstants::FREEZE_CHAINS,
                     MotionConstants::FREEZE_CHAINS +
                     MotionConstants::FREEZE_NUM_CHAINS);
}

FreezeCommand::~FreezeCommand(){

}


float FreezeCommand::getStiffness(){
    return targetStiffness;
}
