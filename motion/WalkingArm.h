#ifndef WalkingArm_h
#define WalkingArm_h

#include "WalkingConstants.h"
#include "Kinematics.h"
#include "Step.h"
#include "Gait.h"

#include <boost/shared_ptr.hpp>
#include <boost/tuple/tuple.hpp>


typedef boost::tuple<std::vector<float>,
                     std::vector<float> > ArmJointStiffTuple;

class WalkingArm{
public:
    WalkingArm(Kinematics::ChainID id);
    ~WalkingArm();

    ArmJointStiffTuple tick(boost::shared_ptr<Step> );

    void startLeft();
    void startRight();

    void resetGait(const Gait * _wp);

private:
    bool shouldSwitchStates();
    void switchToNextState();
    SupportMode nextState();
    void setState(SupportMode newState);

    const float getShoulderPitchAddition(boost::shared_ptr<Step> supportStep);

private:
    SupportMode state;
    Kinematics::ChainID chainID;
    const Gait *gait;

    unsigned int frameCounter;
    unsigned int singleSupportFrames;
    unsigned int doubleSupportFrames;
    bool startStep;
    StepType lastStepType;
};

#endif
