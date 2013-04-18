#pragma once

#include <iostream>
#include <boost/shared_ptr.hpp>

#include "Kinematics.h"

namespace man
{
namespace motion
{

class SetHeadCommand : public MotionCommand
{
public:
    typedef boost::shared_ptr<SetHeadCommand> ptr;

    SetHeadCommand(const float _yaw,
                   const float _pitch,
                   const float _maxSpeedYaw =
                   Kinematics::jointsMaxVelNominal[Kinematics::HEAD_YAW],
                   const float _maxSpeedPitch =
                   Kinematics::jointsMaxVelNominal[Kinematics::HEAD_PITCH])
        : MotionCommand(MotionConstants::SET_HEAD),
           yaw(_yaw),
           pitch(_pitch),
           maxSpeedYaw(_maxSpeedYaw),
           maxSpeedPitch(_maxSpeedPitch)
        {
            setChainList();
        }
       const float getYaw() const {return yaw;}
       const float getPitch() const {return pitch;}
       const float getMaxSpeedYaw() const {return maxSpeedYaw; }
       const float getMaxSpeedPitch() const {return maxSpeedPitch; }
private:
       virtual void setChainList() {
           chainList.insert(chainList.end(),
                            MotionConstants::HEAD_JOINT_CHAINS,
                            MotionConstants::HEAD_JOINT_CHAINS
                            + MotionConstants::HEAD_JOINT_NUM_CHAINS);
       }
private:
       const float yaw,pitch;
       const float maxSpeedYaw,maxSpeedPitch;
};

} // namespace motion
} // namespace man
