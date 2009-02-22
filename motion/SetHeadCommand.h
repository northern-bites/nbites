#ifndef _SetHeadCommand_h_DEFINED
#define _SetHeadCommand_h_DEFINED

class SetHeadCommand : public MotionCommand
{
public:
    SetHeadCommand(const float _yaw, const float _pitch)
        : MotionCommand(MotionConstants::SET_HEAD),
          yaw(_yaw),pitch(_pitch)
        {
            setChainList();
        }
    const float getYaw() const {return yaw;}
    const float getPitch() const {return pitch;}
private:
    virtual void setChainList() {
        chainList.insert(chainList.end(),
                         MotionConstants::HEAD_JOINT_CHAINS,
                         MotionConstants::HEAD_JOINT_CHAINS
                         + MotionConstants::HEAD_JOINT_NUM_CHAINS);
    }
private:
    const float yaw,pitch;
};

#endif
