/**
* @class UNSWalkProvider
*
*A MotionProvider that uses rUNSWift walk engine to compute next angle joints
*
*@author Konstantine Mushegian (port of 2014 walk engine)
*/

#pragma once

#include <vector>

#include "../WalkCommand.h"
#include "../StepCommand.h"
#include "../DestinationCommand.h"
#include "../KickCommand.h"
#include "../BodyJointCommand.h"
#include "../WalkInPlaceCommand.h"
#include "../MotionProvider.h"

#include "RoboGrams.h"
#include "RobotLocation.pb.h"
#include "MotionConstants.h"

//UNSW
#include "generator/Walk2014Generator.hpp"
#include "generator/Generator.hpp"
#include "generator/DistributedGenerator.hpp"
#include "generator/StandGenerator.hpp"
#include "generator/ClippedGenerator.hpp"
#include "blackboard/Blackboard.hpp"
#include "generator/WalkEnginePreProcessor.hpp"


#include "effector/Effector.hpp"
#include "IMUAdjuster.h"
// #include "generator/Generator.hpp"
#include "touch/Touch.hpp"
#include "touch/NullTouch.hpp"
// // #include "blackboard/Adapter.hpp"
#include "generator/BodyModel.hpp"
#include "perception/kinematics/UNSWKinematics.hpp"
// #include "SonarRecorder.hpp"

namespace man
{
namespace motion 
{

class UNSWalkProvider : public MotionProvider
{
public:
	UNSWalkProvider();
	~UNSWalkProvider();

	bool calibrated() const;
	bool upright() const;

	float leftHandSpeed() const;
    float rightHandSpeed() const;
    bool kicking;

	void requestStopFirstInstance();
	void calculateNextJointsAndStiffnesses(
		std::vector<float>&				sensorAngles,
		std::vector<float>&				sensorCurrents,
		const ::messages::InertialState&	sensorInertials,
		const ::messages::FSR&			sensorFSRs
		);

	void hardReset();
	void resetOdometry();

	void setCommand(const WalkCommand::ptr command);
	void setCommand(const WalkInPlaceCommand::ptr command);
    void setCommand(const DestinationCommand::ptr command);
    void setCommand(const KickCommand::ptr command);
    // StepCommand (currently not used) is actually an odometry destination walk
    void setCommand(const StepCommand::ptr command);

    std::vector<BodyJointCommand::ptr> getGaitTransitionCommand() {
        return std::vector<BodyJointCommand::ptr>();
    }

    void getOdometryUpdate(portals::OutPortal< ::messages::RobotLocation>& out) const;

    static const float INITIAL_BODY_POSE_ANGLES[Kinematics::NUM_JOINTS];

   	std::vector<float> getInitialStance() {
        return std::vector<float>(INITIAL_BODY_POSE_ANGLES,
                                  INITIAL_BODY_POSE_ANGLES + Kinematics::NUM_BODY_JOINTS);
    }

    void adjustIMU(const ::messages::InertialState&	sensorInertials);
    void resetIMU() { imuAdjuster->reset(); }

    bool isStanding() const;
    bool isWalkActive() const;

    void setStandby(bool value) { standby = value; }

	void resetAll();

	JointValues walkOutput;
	
	int counter;
	// KickCommand::ptr kickCommand;
	// int kickIndex;


protected:
	void stand();
    void setActive() {}

private:

	bool requestedToStop;
	bool standby;
	bool tryingToWalk;

	void updateOdometry(float gyroZ);

	MotionCommand::ptr currentCommand;
	Touch *touch;
	ClippedGenerator *generator;
	Odometry* startOdometry;
	// Effector* effector;
	BodyModel bodyModel;
	UNSWKinematics kinematics;
	Odometry* odometry;

	JointValues joints;
	IMUAdjuster* imuAdjuster;

	float savedHeadingOdo;
	float slipAverage;
	float lastAngleZ;
	float angleChanged;

	void logMsg(std::string msg) { return; std::cout << "[UNSW WALK DEBUG] " << msg << std::endl; }
	void logMsgNoEL(std::string msg) { return; std::cout << "[UNSW WALK DEBUG] " << msg; }

};

}
}
