/**
 * @class BHWalkProvider
 *
 * A MotionProvider that uses the B-Human walk engine to compute the next body joints
 *
 * @author Octavian Neamtu
 */

#pragma once

#include <vector>

#include "Sensors.h"
#include "NaoPose.h"
#include "WalkCommand.h"
#include "StepCommand.h"
#include "DestinationCommand.h"
#include "MetaGait.h"
#include "BodyJointCommand.h"
#include "EKFStructs.h"
#include "MotionProvider.h"

//BH
#include "WalkingEngine.h"

#include "memory/MObjects.h"

namespace man {
namespace motion {

using namespace memory;

class BHWalkProvider : public MotionProvider {

public:
    //TODO: naopose is for nothing, I just kept it here because the
	//WalkProvider is the same but it should be eliminated from both
	//places (e.g. it is not used here, it is not used
    //in walkprovider)
    BHWalkProvider(boost::shared_ptr<Sensors> s, boost::shared_ptr<NaoPose> p);
    virtual ~BHWalkProvider() {}

	// Provide calibration boolean to the rest of the system.
	bool calibrated() {
		return walkingEngine.theInertiaSensorData.calibrated;
	}

    void requestStopFirstInstance();
    void calculateNextJointsAndStiffnesses();

    void hardReset();

    void setCommand(const WalkCommand::ptr command);
    void setCommand(const Gait::ptr command) {}
    void setCommand(const DestinationCommand::ptr command);
    //TODO: I'm taking over StepCommand (currently not used) and making
    //it an odometry destination walk
    void setCommand(const StepCommand::ptr command);

    std::vector<BodyJointCommand::ptr> getGaitTransitionCommand() {
        return std::vector<BodyJointCommand::ptr>();
	}

    MotionModel getOdometryUpdate() const;
    virtual const SupportFoot getSupportFoot() const;

    static const float INITIAL_BODY_POSE_ANGLES[Kinematics::NUM_JOINTS];
    //returns only body angles
    //TODO: this is in nature due to the fact that we don't separate head providers
    //from body providers - if we did we could separate the methods for each
    std::vector<float> getInitialStance() {
        return std::vector<float>(INITIAL_BODY_POSE_ANGLES,
                                  INITIAL_BODY_POSE_ANGLES + Kinematics::NUM_BODY_JOINTS);
    }

    //TODO: rename this to isGoingToStand since it flags whether we are going to
    //a stand rather than be at a complete standstill
    bool isStanding() const;
    // !isWalkActive() means we're at a complete standstill. everything else is walking.
    bool isWalkActive() const;

    void update(proto::WalkProvider* walkProvider) const;

protected:
    void stand();
    void setActive() {}

//    void playDead();

private:
    bool requestedToStop;
    boost::shared_ptr<Sensors> sensors;
    WalkingEngine walkingEngine;
    MotionCommand::ptr currentCommand;
    Pose2D startOdometry;
};

}
}
