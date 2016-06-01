#include "UNSWalkProvider.h"
#include "Profiler.h"
#include "Common.h"
#include "NaoPaths.h"

#include <cassert>
#include <string>
#include <iostream>

#include "generator/Walk2014Generator.hpp"
#include <cmath>
#include "utils/angles.hpp"
#include "utils/body.hpp"
#include "utils/Logger.hpp"
#include "utils/basic_maths.hpp"
#include "utils/speech.hpp"

namespace man 
{
namespace motion 
{

// static const float INITIAL_BODY_POSE_ANGLES[] {
// 	0.f,0.f,0.f,0.f,0.f,0.f,0.f,
// 	0.f,0.f,0.f,0.f,0.f,0.f,0.f,
// 	0.f,0.f,0.f,0.f,0.f,0.f,0.f,0.f,
// };

const float UNSWalkProvider::INITIAL_BODY_POSE_ANGLES[] {
	0.f,0.f,0.f,0.f,0.f,0.f,0.f,
	0.f,0.f,0.f,0.f,0.f,0.f,0.f,
	0.f,0.f,0.f,0.f,0.f,0.f,0.f,0.f,
};

/**
*NBites joint order. commented out joints are the ones that UNSW has but we dont use
*/
static const Joints::JointCode nb_joint_order[] {
		Joints::HeadYaw,
      	Joints::HeadPitch,
      	Joints::LShoulderPitch,
		Joints::LShoulderRoll,
		Joints::LElbowYaw,
		Joints::LElbowRoll,
		Joints::LHipYawPitch,
		Joints::LHipRoll,
		Joints::LHipPitch,
		Joints::LKneePitch,
		Joints::LAnklePitch,
		Joints::LAnkleRoll,
		Joints::RHipRoll,
		Joints::RHipPitch,
		Joints::RKneePitch,
		Joints::RAnklePitch,
		Joints::RAnkleRoll,
		Joints::RShoulderPitch,
		Joints::RShoulderRoll,
		Joints::RElbowYaw,
		Joints::RElbowRoll,
		Joints::LWristYaw,
		//LHand,
		//RWristYaw,
		//RHand,
  };

UNSWalkProvider::UNSWalkProvider() : MotionProvider(WALK_PROVIDER), 
 									 requestedToStop(false), tryingToWalk(false) 
{
	generator = (Generator*) new ClippedGenerator((Generator*) new DistributedGenerator());
	resetAll();
}

UNSWalkProvider::~UNSWalkProvider()
{
	delete generator;
}

void UNSWalkProvider::resetAll() {
	inactive();

	//reset odometry through walk2014gen
	//reset other parts of walk2014gen
}

bool UNSWalkProvider::calibrated() const {
	return NULL;
}

bool UNSWalkProvider::upright() const {
	return NULL;
}

float UNSWalkProvider::leftHandSpeed() const {
	return NULL;
}

float UNSWalkProvider::rightHandSpeed() const {
	return NULL;
}

void UNSWalkProvider::requestStopFirstInstance() {

}

void UNSWalkProvider::calculateNextJointsAndStiffnesses(
	std::vector<float>& 			sensorAngles,
	std::vector<float>& 			sensorCurrents,
	const messages::InertialState&  sensorInertials,
	const messages::FSR& 			sensorFSRs
	) 
{
	PROF_ENTER(P_WALK);

	if (standby) {
		tryingToWalk = false;
	} else {
		if (requestedToStop || !isActive()) {
			tryingToWalk = false;
		} else if (currentCommand.get() && currentCommand->getType() == MotionConstants::STEP) {
			logMsg("Odometry walk!");
			// TODO odometry, handle
		} else if (currentCommand.get() && currentCommand->getType() == MotionConstants::WALK) {
			logMsg("Walking!");
		 
			// HANDLE
			tryingToWalk = true;

			WalkCommand::ptr command = boost::shared_static_cast<WalkCommand>(currentCommand);


		} else if (currentCommand.get() && currentCommand->getType() == MotionConstants::DESTINATION) {
			logMsg("Destination Walking!");
			tryingToWalk = true;


		} else if (!currentCommand.get()) {
			tryingToWalk = false;
			// call stand
		}
	}

	logMsg("walking");

	ActionCommand::All* request = new ActionCommand::All();
	request->body.actionType = ActionCommand::Body::WALK;
	Odometry* odometry = new Odometry();

	// Update sensor values
	UNSWSensorValues sensors = new UNSWSensorValues();
	// TODO investigate calibrating sensors. . .
	// if(request.body.actionType == Body::MOTION_CALIBRATE){
 //       // raw sensor values are sent to offnao for calibration
 //       // these values are straight forward copy paste into pos files
 //       sensors = nakedTouch->getSensors(kinematics);
 //       sensors.sensors[Sensors::InertialSensor_AngleX] = -RAD2DEG(sensors.sensors[Sensors::InertialSensor_AngleX]);
 //       sensors.sensors[Sensors::InertialSensor_AngleY] = -RAD2DEG(sensors.sensors[Sensors::InertialSensor_AngleY]);
 //       sensors.sensors[Sensors::InertialSensor_GyrX] = -sensors.sensors[Sensors::InertialSensor_GyrX];
 //       sensors.sensors[Sensors::InertialSensor_GyrY] = -sensors.sensors[Sensors::InertialSensor_GyrY];
 //   } else {
    sensors = touch->getSensors(kinematics);


	// Update kinematics TODO sensors lagging ? ? 


	kinematics.setSensorValues(sensors);

    // Update the body model
    bodyModel.kinematics = &kinematics;
    bodyModel.update(&odo, sensors);

	float ballX, ballY;

	// generator->makeJoints(ActionCommand::All* request,
 //                                          Odometry* odometry,
 //                                          const UNSWSensorValues &sensors,
 //                                          BodyModel &bodyModel,
 //                                          float ballX,
 //                                          float ballY);

	PROF_EXIT(P_WALK);
}

void UNSWalkProvider::hardReset() {
	generator->reset();
}

void UNSWalkProvider::resetOdometry() {

}

void UNSWalkProvider::setCommand(const WalkCommand::ptr command) {
	if (command->theta_percent == 0 && command->x_percent == 0 && command->y_percent == 0) {
		this->stand();
		return;
	}

	currentCommand = command;
	active();

}

void UNSWalkProvider::setCommand(const DestinationCommand::ptr command) {

}

void UNSWalkProvider::setCommand(const StepCommand::ptr command) {

}

void UNSWalkProvider::getOdometryUpdate(portals::OutPortal<messages::RobotLocation>& out) const {
	portals::Message<messages::RobotLocation> odometryData(0);
	//odometryData.get()->set_x();
	//odometryData.get()->set_y();
	//odometryData.get()->set_h();
}

bool UNSWalkProvider::isStanding() const { //is going to stand rather than at complete standstill
	// return generator->getIsStanding(); //1 corresponds to process of moving from WALK crouch to STAND
}

bool UNSWalkProvider::isWalkActive() const {
	return generator->isActive();
}

void UNSWalkProvider::stand() {

}


// void UNSWalkProvider::restingStand()
// {

// }

// void UNSWalkProvider::resetOdometry() {
// 	//rest odo
// }

}

}