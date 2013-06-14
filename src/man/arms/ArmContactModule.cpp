#include "ArmContactModule.h"

namespace man {
namespace arms {

ArmContactModule::ArmContactModule() : Module(),
                                       contactOut(base())
{
}

void ArmContactModule::run_()
{
    actualJointsIn.latch();
    expectedJointsIn.latch();

    expectedJoints.push(expectedJointsIn.message());

    jointsWithDelay = &expectedJoints.front();

    if (expectedJoints.size() > FRAMES_DELAY)
    {
        expectedJoints.pop();
    }

    portals::Message<messages::ArmContactState> current(0);

    determineContactState();

    current.get()->set_right_push_direction(rightArm);
    current.get()->set_left_push_direction(leftArm);

    //std::cout << current.get()->DebugString() << std::endl;

    contactOut.setMessage(current);
}

void ArmContactModule::determineContactState()
{
    float leftPitchD = (jointsWithDelay->l_shoulder_pitch() -
                        actualJointsIn.message().l_shoulder_pitch());
    float leftRollD = (jointsWithDelay->l_shoulder_roll() -
                       actualJointsIn.message().l_shoulder_roll());

    float rightPitchD = (jointsWithDelay->r_shoulder_pitch() -
                        actualJointsIn.message().r_shoulder_pitch());
    float rightRollD = (jointsWithDelay->r_shoulder_roll() -
                       actualJointsIn.message().r_shoulder_roll());

    // LEFT arm
    if (fabs(leftPitchD) > fabs(leftRollD))
    {
        if (fabs(leftPitchD) > DISPLACEMENT_THRESH)
        {
            if (leftPitchD > 0) leftArm = messages::ArmContactState::NORTH;
            else leftArm = messages::ArmContactState::SOUTH;
        }
        else leftArm = messages::ArmContactState::NONE;
    }
    else
    {
        if (fabs(leftRollD) > DISPLACEMENT_THRESH)
        {
            if (leftRollD > 0) leftArm = messages::ArmContactState::EAST;
            else leftArm = messages::ArmContactState::WEST;
        }
        else leftArm = messages::ArmContactState::NONE;
    }

    // RIGHT arm
    if (fabs(rightPitchD) > fabs(rightRollD))
    {
        if (fabs(rightPitchD) > DISPLACEMENT_THRESH)
        {
            if (rightPitchD > 0) rightArm = messages::ArmContactState::NORTH;
            else rightArm = messages::ArmContactState::SOUTH;
        }
        else rightArm = messages::ArmContactState::NONE;
    }
    else
    {
        if (fabs(rightRollD) > DISPLACEMENT_THRESH)
        {
            if (rightRollD > 0) rightArm = messages::ArmContactState::EAST;
            else rightArm = messages::ArmContactState::WEST;
        }
        else rightArm = messages::ArmContactState::NONE;
    }

}

}
}
