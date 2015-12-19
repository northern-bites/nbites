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
#include "../BodyJointCommand.h"
#include "../MotionProvider.h"

#include "RoboGrams.h"
#include "RobotLocation.pb.h"

//UNSW
#include "generator/Walk2014Generator.hpp"
#include "blackboard/Blackboard.hpp"

namespace man
{
namespace motion 
{

class UNSWalkProvider : public MotionProvider
{
public:
	UNSWalkProvider();
	virtual ~UNSWalkProvider();

	bool calibrated() const;
	bool upright() const;

	void requestStopFirstInstance();
	void calculateNextJointsAndStiffnesses(
		std::vector<float>&				sensorAngles,
		std::vector<float>&				sensorCurrents,
		const messages::InertialState&	sensorInertials,
		const messages::FSR&			sensorFSRs
		);

	void hardReset();
	void resetOdometry();

	void setCommand(const WalkCommand::ptr command);
    void setCommand(const DestinationCommand::ptr command);
    // StepCommand (currently not used) is actually an odometry destination walk
    void setCommand(const StepCommand::ptr command);

    bool isStanding() const;
    bool isWalkActive() const;

    void setStandby(bool value) { standby = value; }

	void resetAll();

protected:
	void stand();
    void setActive() {}

private:
	bool requestedToStop;
	bool standby;
	bool tryingToWalk;
	MotionCommand::ptr currentCommand;
	Walk2014Generator *walk2014gen;
};

}
}
