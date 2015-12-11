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

protected:

private:
	bool requestedToStop;
	bool standby;
	bool tryingToWalk;
	Walk2014Generator *walk2014gen;
};

}
}
