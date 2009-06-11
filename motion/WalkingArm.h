#ifndef WalkingArm_h
#define WalkingArm_h

#include "WalkingConstants.h"
#include "Kinematics.h"


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

    void resetGait(const WalkingParameters * _wp);

private:
    bool shouldSwitchStates();
    void switchToNextState();
    SupportMode nextState();
    void setState(SupportMode newState);

    ArmJointStiffTuple swinging();
    ArmJointStiffTuple supporting();

private:
    SupportMode state;
    Kinematics::ChainID chainID;
    const WalkingParameters *walkParams;

    unsigned int frameCounter;
    unsigned int singleSupportFrames;
    unsigned int doubleSupportFrames;

};

#endif
