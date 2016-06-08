#include "UNSWalkProvider.h"
#include "Profiler.h"
#include "Common.h"
#include "NaoPaths.h"
#include "Kinematics.h"

#include <cassert>
#include <string>
#include <iostream>

#include <stdlib.h>

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

// Runswift takes walk commands in MM, we use CM
const float MM_PER_CM  = 10.0;

/*
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
		// Joints::RHipYawPitch,
		Joints::RHipRoll,
		Joints::RHipPitch,
		Joints::RKneePitch,
		Joints::RAnklePitch,
		Joints::RAnkleRoll,
		Joints::RShoulderPitch,
		Joints::RShoulderRoll,
		Joints::RElbowYaw,
		Joints::RElbowRoll
		// Joints::LWristYaw,
		//LHand,
		//RWristYaw,
		//RHand,
  };

UNSWalkProvider::UNSWalkProvider() : MotionProvider(WALK_PROVIDER), 
 									 requestedToStop(false), tryingToWalk(false) 
{
	generator =  new ClippedGenerator((Generator*) new DistributedGenerator());
	odometry = new Odometry();
	joints = new JointValues();
	resetAll();
}

UNSWalkProvider::~UNSWalkProvider()
{
	delete generator;
	delete odometry;
	delete &joints;
	delete startOdometry;
}

void UNSWalkProvider::resetAll() {
	inactive();
	generator->reset();
	resetOdometry();
	//reset odometry through walk2014gen
	//reset other parts of walk2014gen
}

bool UNSWalkProvider::calibrated() const {
	return true;
}

bool UNSWalkProvider::upright() const {
	return true;
}

float UNSWalkProvider::leftHandSpeed() const {
	return NULL;
}

float UNSWalkProvider::rightHandSpeed() const {
	return NULL;
}

void UNSWalkProvider::requestStopFirstInstance() {
	requestedToStop = true;
}

bool hasLargerMagnitude(float x, float y) {
	if (y > 0.0f) {
		return x > y;
	}
	if (y < 0.0f) {
		return x < y;
	}
	return true;
}

bool hasPassed(Odometry& odo1, Odometry& odo2) {
	return (hasLargerMagnitude(odo1.forward, odo2.forward) &&
		hasLargerMagnitude(odo1.left, odo2.left) &&
		hasLargerMagnitude(odo1.turn, odo2.turn));
}

void UNSWalkProvider::calculateNextJointsAndStiffnesses(
	std::vector<float>& 			sensorAngles,
	std::vector<float>& 			sensorCurrents,
	const messages::InertialState&  sensorInertials,
	const messages::FSR& 			sensorFSRs
	) 
{
	PROF_ENTER(P_WALK);


	ActionCommand::All* request = new ActionCommand::All();
	request->body.actionType = ActionCommand::Body::WALK;

	if (standby) {
		logMsg("In standby");
		tryingToWalk = false;
	} else {
		if (requestedToStop || !isActive()) {
			tryingToWalk = false;
			logMsg("requested to stop or is not active");
		} else if (currentCommand.get() && currentCommand->getType() == MotionConstants::STEP) {
			logMsg("STEP command! Odometry walk!");

			StepCommand::ptr command = boost::shared_static_cast<StepCommand>(currentCommand);
			Odometry deltaOdometry = odometry - startOdometry;
			Odometry absoluteTarget = Odometry(command->x_mms, command->y_mms, command->theta_rads);
			Odometry relativeTarget = absoluteTarget - (deltaOdometry); 

			if (!hasPassed(deltaOdometry, absoluteTarget)) { // not reached target yet

				request->body.forward = relativeTarget.forward / MM_PER_CM;
				request->body.left = relativeTarget.left / MM_PER_CM;
				request->body.turn = relativeTarget.turn;
			} else {
				tryingToWalk = false;
				request->body.actionType = ActionCommand::Body::STAND;

			}



			// TODO odometry, handle
		} else if (currentCommand.get() && currentCommand->getType() == MotionConstants::WALK) {
			logMsg("Walk command - Walking!");
		 
			// HANDLE
			tryingToWalk = true;

			WalkCommand::ptr command = boost::shared_static_cast<WalkCommand>(currentCommand);
			std::cout << "Walk Command: " << command->x_percent << "," << command->y_percent << "," << command->theta_percent << ") \n";
			request->body.forward = 1.0; //command->x_percent ;
			request->body.left = 0.0; //command->y_percent ;
			request->body.turn = 0.0; //command->theta_percent ;

		} else if (currentCommand.get() && currentCommand->getType() == MotionConstants::DESTINATION) {
			logMsg("Destination command - Destination Walking!");
			tryingToWalk = true;

			DestinationCommand::ptr command = boost::shared_static_cast<DestinationCommand>(currentCommand);
			request->body.forward = command->x_mm;
			request->body.left = command->y_mm;
			request->body.turn = command->theta_rads;

			// TODO incorporate motion kicks


		} else if (currentCommand.get() && currentCommand->getType() == MotionConstants::KICK) {
			logMsg("Kick command sent now!");
			tryingToWalk = false;

		} else if (!currentCommand.get()) {
			logMsg("Can't get current command! Requesting stand");
			tryingToWalk = false;
			// call stand
			request->body.actionType = ActionCommand::Body::STAND;
		}

		// TODO handle kick
	}

	logMsg("==========================\nwalking now!");


	// Update sensor values
	UNSWSensorValues sensors = new UNSWSensorValues();
	sensors.joints = joints;

	sensors.sensors[Sensors::InertialSensor_GyrX] = sensorInertials.gyr_x();
    sensors.sensors[Sensors::InertialSensor_GyrY] = sensorInertials.gyr_y();
    // sensors.sensors[Sensors::InertialSensor_GyrZ] = sensorInertials.gyr_z(); // they don't use this

    sensors.sensors[Sensors::InertialSensor_AccX] = sensorInertials.acc_x();
    sensors.sensors[Sensors::InertialSensor_AccY] = sensorInertials.acc_y();
    sensors.sensors[Sensors::InertialSensor_AccZ] = sensorInertials.acc_z();

    sensors.sensors[Sensors::InertialSensor_AngleX] = sensorInertials.angle_x();
    sensors.sensors[Sensors::InertialSensor_AngleY] = sensorInertials.angle_y();
    // sensors.sensors[Sensors::angleZ] = sensorInertials.angle_z(); // this either

    sensors.sensors[Sensors::LFoot_FSR_FrontLeft] = sensorFSRs.lfl();
    sensors.sensors[Sensors::LFoot_FSR_FrontRight] = sensorFSRs.lfr();
    sensors.sensors[Sensors::LFoot_FSR_RearLeft] = sensorFSRs.lrl();
    sensors.sensors[Sensors::LFoot_FSR_RearRight] = sensorFSRs.lrr();

    sensors.sensors[Sensors::RFoot_FSR_FrontLeft] = sensorFSRs.rfl();
    sensors.sensors[Sensors::RFoot_FSR_FrontRight] = sensorFSRs.rfr();
    sensors.sensors[Sensors::RFoot_FSR_RearLeft] = sensorFSRs.rrl();
    sensors.sensors[Sensors::RFoot_FSR_RearRight] = sensorFSRs.rrr();

    for (int i = 0; i < 21; i++) {
    	sensors.joints.angles[nb_joint_order[i]] = sensorAngles[i];
    }

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
    // sensors = touch->getSensors(kinematics);


	// Update kinematics TODO sensors lagging ? ?
	// TODO kinematics parameters? - move these..., also hopefully right units
	kinematics.setSensorValues(sensors);
	kinematics.parameters.cameraYawTop = Kinematics::CAMERA_TOP_OFF_Z;
	kinematics.parameters.cameraPitchTop = Kinematics::CAMERA_TOP_PITCH_ANGLE;
	kinematics.parameters.cameraRollTop = Kinematics::CAMERA_TOP_OFF_X;

	kinematics.parameters.cameraYawBottom = Kinematics::CAMERA_BOTTOM_OFF_Z;
	kinematics.parameters.cameraPitchBottom = Kinematics::CAMERA_BOTTOM_PITCH_ANGLE;
	kinematics.parameters.cameraRollBottom = Kinematics::CAMERA_BOTTOM_OFF_X;
	kinematics.updateDHChain();

	// Get the position of the ball in robot relative cartesian coordinates
	// Is this necessary for our system? 
	float ballX = 5.0;
	float ballY = 5.0;

    // Update the body model
    logMsg("Updating body model");
    bodyModel.kinematics = &kinematics;
    bodyModel.update(odometry, sensors);


	// TODO figure out how to tell if we are standing this seems important
	// if (standing) {
	// 	generator->reset();
	// 	request.body = ActionCommand::Body::INITIAL;
	// 	odometry.clear();
	// }

    const float* angles = NULL;
    const float* hardness = NULL;

    joints = generator->makeJoints(request, odometry, sensors, bodyModel, ballX, ballY);
    // (ActionCommand::All* request,
    //                       Odometry* odometry,
    //                       const UNSWSensorValues &sensors,
    //                       BodyModel &bodyModel,
    //                       float ballX,
    //                       float ballY);

    angles = joints.angles;
    hardness = joints.stiffnesses;

    // for (uint8_t i = 0; i < Joints::NUMBER_OF_JOINTS; ++i) {

	   //    std::cout << "WALK PROVIDER: Joint " << Joints::jointNames[i] << " angle: " << angles[i] << " stiffness: " << joints.stiffnesses[i] << std::endl;
    // }

	// generator->makeJoints(ActionCommand::All* request,
 //                                          Odometry* odometry,
 //                                          const UNSWSensorValues &sensors,
 //                                          BodyModel &bodyModel,
 //                                          float ballX,
 //                                          float ballY);

    // THIS IS WHERE WE ACTUALLY SET THE NEXT JOINTS AND STIFFNESSES
    // Ignore the first chain bc it's the head
    for (unsigned i = 1; i < Kinematics::NUM_CHAINS; i++) {
    	std::vector<float> chain_angles;
    	std::vector<float> chain_hardness;
    	for (unsigned j = Kinematics::chain_first_joint[i]; j <= Kinematics::chain_last_joint[i]; j++) {
    		chain_angles.push_back(joints.angles[nb_joint_order[j]]); // TODO I think this should be a lot more complicated than it seems right now...
    		logMsgNoEL("ANGLE in "  + Joints::jointNames[nb_joint_order[j]] + " = ");
			// std::cout << RAD2DEG(joints.angles[nb_joint_order[j]]);
			std::cout << (joints.angles[nb_joint_order[j]]);


    		if (hardness[nb_joint_order[j]] == 0) {
    			// logMsg("NO STIFFNESS in " + Joints::jointNames[nb_joint_order[j]]);
    			chain_hardness.push_back(MotionConstants::NO_STIFFNESS);
    		} else {
    			chain_hardness.push_back(joints.stiffnesses[nb_joint_order[j]]);
    			// TODO Double check this...
    		}
    		std::cout << " STIFFNESS: " << joints.stiffnesses[nb_joint_order[j]] << std::endl;
    	}

    	this->setNextChainJoints((Kinematics::ChainID)i, chain_angles);
    	this->setNextChainStiffnesses((Kinematics::ChainID)i, chain_hardness);

    }

    // We only leave when we do a sweet move, so request a special action
    if (requestedToStop) {
    	logMsg("Requested to stop");
    	inactive();
    	requestedToStop = false;
    	resetOdometry();
    }

	PROF_EXIT(P_WALK);
}

void UNSWalkProvider::hardReset() {
	generator->reset();
}

void UNSWalkProvider::resetOdometry() {
	odometry->clear();
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
	currentCommand = command;

	startOdometry = odometry;

	active();
}

void UNSWalkProvider::setCommand(const StepCommand::ptr command) {
	currentCommand = command;
	active();


}

void UNSWalkProvider::setCommand(const KickCommand::ptr command) {
	currentCommand = command;
	active();
}

void UNSWalkProvider::getOdometryUpdate(portals::OutPortal<messages::RobotLocation>& out) const {
	portals::Message<messages::RobotLocation> odometryData(0);
	//odometryData.get()->set_x();
	//odometryData.get()->set_y();
	//odometryData.get()->set_h();
	out.setMessage(odometryData);
}

bool UNSWalkProvider::isStanding() const { //is going to stand rather than at complete standstill
	return generator->isStanding(); //1 corresponds to process of moving from WALK crouch to STAND
}

bool UNSWalkProvider::isWalkActive() const {
	return generator->isActive();
}

void UNSWalkProvider::stand() {

}


// void UNSWalkProvider::restingStand()
// {

// }

}

}