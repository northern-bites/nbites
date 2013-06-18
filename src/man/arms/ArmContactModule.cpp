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

    // Buffer the current expected joints, will use these FRAMES_DELAY
    // frames later
    expectedJoints.push(expectedJointsIn.message());

    // Get the delayed joints
    jointsWithDelay = &expectedJoints.front();

    if (expectedJoints.size() > FRAMES_DELAY)
    {
        expectedJoints.pop();
    }

    portals::Message<messages::ArmContactState> current(0);

    // Do the decision work
    determineContactState();

    // Keep track of the last push we detected
    if (rightArm != messages::ArmContactState::NONE)
    {
        lastDetectedRight = rightArm;
    }
    if (leftArm != messages::ArmContactState::NONE)
    {
        lastDetectedLeft = leftArm;
    }

    // Count how long we've been getting the same push
    if (rightArm == lastDetectedRight)  rightStuckCounter++;
    else rightStuckCounter = 0;

    if (leftArm == lastDetectedLeft)  leftStuckCounter++;
    else leftStuckCounter = 0;


    // If an arm is stuck, override what we computed for it and
    // send out NONE until it becomes unstuck... otherwise send what
    // was actually computed
    if (rightStuckCounter < STUCK_THRESH)
    {
        current.get()->set_right_push_direction(rightArm);
    }
    else
    {
        current.get()->set_right_push_direction(messages::ArmContactState::NONE);
    }
    if (leftStuckCounter < STUCK_THRESH)
    {
        current.get()->set_left_push_direction(leftArm);
    }
    else
    {
        current.get()->set_left_push_direction(messages::ArmContactState::NONE);
    }

    contactOut.setMessage(current);
}

void ArmContactModule::determineContactState()
{
    Displacement r, l;

    // Compute the *corrected* displacement based on the hand speed. If the
    // hand is moving particularly fast this speed, we weight this value
    // less in the average because it is less reliable.
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

    // Keep track of the corrected values for the averaging
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

    // Get the average to actually use
    r = getAverageDisplacement(right);
    l = getAverageDisplacement(left);

    // Figure out the right arm
    // If we have significant pitch AND roll displacement
    if ((fabs(r[PITCH]) > DISPLACEMENT_THRESH) &&
        (fabs(r[ROLL]) > DISPLACEMENT_THRESH))
    {
        if (r[PITCH] < 0 && r[ROLL] < 0)
        {
            rightArm = messages::ArmContactState::NORTHEAST;
        }
        else if (r[PITCH] < 0 && r[ROLL] > 0)
        {
            rightArm = messages::ArmContactState::NORTHWEST;
        }
        else if (r[PITCH] > 0 && r[ROLL] < 0)
        {
            rightArm = messages::ArmContactState::SOUTHEAST;
        }
        else if (r[PITCH] > 0 && r[ROLL] > 0)
        {
            rightArm = messages::ArmContactState::SOUTHWEST;
        }
    }
    // Only pitch displacement
    else if (fabs(r[PITCH]) > DISPLACEMENT_THRESH)
    {
        if (r[PITCH] < 0) rightArm = messages::ArmContactState::NORTH;
        else rightArm = messages::ArmContactState::SOUTH;
    }
    // Only roll displacement
    else if (fabs(r[ROLL]) > DISPLACEMENT_THRESH)
    {
        if (r[ROLL] < 0) rightArm = messages::ArmContactState::EAST;
        else rightArm = messages::ArmContactState::WEST;
    }
    // No displacement... no pushing!
    else rightArm = messages::ArmContactState::NONE;

    // Do the same for the left arm (copy-paste programming bad...)
    if ((fabs(l[PITCH]) > DISPLACEMENT_THRESH) &&
        (fabs(l[ROLL]) > DISPLACEMENT_THRESH))
    {
        if (l[PITCH] < 0 && l[ROLL] < 0)
        {
            leftArm = messages::ArmContactState::NORTHEAST;
        }
        else if (l[PITCH] < 0 && l[ROLL] > 0)
        {
            leftArm = messages::ArmContactState::NORTHWEST;
        }
        else if (l[PITCH] > 0 && l[ROLL] < 0)
        {
            leftArm = messages::ArmContactState::SOUTHEAST;
        }
        else if (l[PITCH] > 0 && l[ROLL] > 0)
        {
            leftArm = messages::ArmContactState::SOUTHWEST;
        }
    }
    else if (fabs(l[PITCH]) > DISPLACEMENT_THRESH)
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
