/**
 * @class BHWalkProvider
 *
 * A MotionProvider that uses the B-Human walk engine to compute the next body joints
 *
 * @author Octavian Neamtu (port of 2011 BH walk engine)
 * @author Josh Imhoff (port of the 2013 BH walk engine)
 */

#pragma once

#include <vector>

#include "../WalkCommand.h"
#include "../StepCommand.h"
#include "../DestinationCommand.h"
#include "../BodyJointCommand.h"
#include "../MotionProvider.h"

#include "RoboGrams.h"
#include "RobotLocation.pb.h"

//BH
#include "WalkingEngine.h"
#include "Representations/Blackboard.h"
#include "Tools/Math/Pose2D.h"

namespace man
{
namespace motion
{

class BHWalkProvider : public MotionProvider
{
public:
    BHWalkProvider();
    virtual ~BHWalkProvider();

    // Provide calibration boolean to the rest of the system
    bool calibrated() const;

    // Provide fall down detection to the rest of the system
    bool upright() const;

    // Provide hand speeds to the rest of the system
    float leftHandSpeed() const;
    float rightHandSpeed() const;

    void requestStopFirstInstance();
    void calculateNextJointsAndStiffnesses(
        std::vector<float>&            sensorAngles,
        std::vector<float>&            sensorCurrents,
        const messages::InertialState& sensorInertials,
        const messages::FSR&           sensorFSRs
        );

    void hardReset();
    void resetOdometry();

    void setCommand(const WalkCommand::ptr command);
    void setCommand(const DestinationCommand::ptr command);
    // StepCommand (currently not used) is actually an odometry destination walk
    void setCommand(const StepCommand::ptr command);

    std::vector<BodyJointCommand::ptr> getGaitTransitionCommand() {
        return std::vector<BodyJointCommand::ptr>();
    }

    void getOdometryUpdate(portals::OutPortal<messages::RobotLocation>& out) const;

    static const float INITIAL_BODY_POSE_ANGLES[Kinematics::NUM_JOINTS];

    std::vector<float> getInitialStance() {
        return std::vector<float>(INITIAL_BODY_POSE_ANGLES,
                                  INITIAL_BODY_POSE_ANGLES + Kinematics::NUM_BODY_JOINTS);
    }

    // TODO rename this to isGoingToStand since it flags whether we are going to
    // a stand rather than be at a complete standstill
    bool isStanding() const;
    // !isWalkActive() means we're at a complete standstill, everything else is walking
    bool isWalkActive() const;

    void setStandby(bool value) { standby = value; }

protected:
    void stand();
    void setActive() {}

private:
    bool requestedToStop;
    bool standby;
    bool justMotionKicked;
    bool tryingToWalk;
    MotionCommand::ptr currentCommand;
    Pose2DBH startOdometry;

    WalkingEngine *walkingEngine;
    Blackboard *blackboard;
    };
} // namespace motion
} // namespace man
