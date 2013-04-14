#include "BaseFreezeCommand.h"

namespace man
{
namespace motion
{
BaseFreezeCommand::BaseFreezeCommand(float stiffness)
    : MotionCommand(MotionConstants::FREEZE)
{
    setChainList();
    targetStiffness = stiffness;
}

void BaseFreezeCommand::setChainList() {
    chainList.insert(chainList.end(),
                     MotionConstants::FREEZE_CHAINS,
                     MotionConstants::FREEZE_CHAINS +
                     MotionConstants::FREEZE_NUM_CHAINS);
}

BaseFreezeCommand::~BaseFreezeCommand(){

}


float BaseFreezeCommand::getStiffness(){
    return targetStiffness;
}
}
}
