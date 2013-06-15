#include "ArmContactModule.h"

namespace man {
namespace arms {

Displacement getAverageDisplacement(DisplacementBuffer& buf)
{
    Displacement avg;
    avg.push_back(0.f);
    avg.push_back(0.f);

    for (DisplacementBuffer::iterator i = buf.begin();
         i != buf.end(); i++)
    {
        avg[PITCH] += (*i)[PITCH];
        avg[ROLL] += (*i)[ROLL];
    }

    avg[PITCH] /= float(buf.size());
    avg[ROLL] /= float(buf.size());

    return avg;
}

ArmContactModule::ArmContactModule() : Module(),
                                       contactOut(base())
{
}

void ArmContactModule::run_()
{
    actualJointsIn.latch();
    expectedJointsIn.latch();
    handSpeedsIn.latch();

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

    std::cout << current.get()->DebugString() << std::endl;

    contactOut.setMessage(current);
}

void ArmContactModule::determineContactState()
{
    Displacement r, l;

    float rCorrect = std::max(0.f, 1.f -
                              (handSpeedsIn.message().right_speed() /
                               SPEED_BASED_ERROR_REDUCTION));

    float lCorrect = std::max(0.f, 1.f -
                              (handSpeedsIn.message().left_speed() /
                               SPEED_BASED_ERROR_REDUCTION));

    r.push_back(rCorrect*(actualJointsIn.message().r_shoulder_pitch() -
                          jointsWithDelay->r_shoulder_pitch()));
    r.push_back(rCorrect*(actualJointsIn.message().r_shoulder_roll() -
                          jointsWithDelay->r_shoulder_roll()));

    l.push_back(lCorrect*(actualJointsIn.message().l_shoulder_pitch() -
                          jointsWithDelay->l_shoulder_pitch()));
    l.push_back(lCorrect*(actualJointsIn.message().l_shoulder_roll() -
                          jointsWithDelay->l_shoulder_roll()));

    right.push_back(r);
    left.push_back(l);

    if (left.size() > FRAMES_TO_BUFFER)
    {
        left.pop_front();
    }
    if (right.size() > FRAMES_TO_BUFFER)
    {
        right.pop_front();
    }

    r = getAverageDisplacement(right);
    l = getAverageDisplacement(left);

    // Prioritize pitch...
    if (fabs(r[PITCH]) > DISPLACEMENT_THRESH)
    {
        if (r[PITCH] < 0) rightArm = messages::ArmContactState::NORTH;
        else rightArm = messages::ArmContactState::SOUTH;
    }
    else if (fabs(r[ROLL]) > DISPLACEMENT_THRESH)
    {
        if (r[ROLL] < 0) rightArm = messages::ArmContactState::EAST;
        else rightArm = messages::ArmContactState::WEST;
    }
    else rightArm = messages::ArmContactState::NONE;

    if (fabs(l[PITCH]) > DISPLACEMENT_THRESH)
    {
        if (l[PITCH] < 0) leftArm = messages::ArmContactState::NORTH;
        else leftArm = messages::ArmContactState::SOUTH;
    }
    else if (fabs(l[ROLL]) > DISPLACEMENT_THRESH)
    {
        if (l[ROLL] < 0) leftArm = messages::ArmContactState::EAST;
        else leftArm = messages::ArmContactState::WEST;
    }
    else leftArm = messages::ArmContactState::NONE;
}

}
}
