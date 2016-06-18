#include "UNSWalkProvider.h"
#include "Profiler.h"
#include "Common.h"
#include "NaoPaths.h"
#include "Kinematics.h"
#include "SensorTypes.h"

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
#include "touch/FilteredTouch.hpp"
// #include 

namespace man 
{
namespace motion 
{

// NICE AND STRAIGHT LEGS
// const float UNSWalkProvider::INITIAL_BODY_POSE_ANGLES[] {
// 	1.57f, 0.17f, -1.57f, -0.05f,
// 	0.f,0.f,0.f,0.f,0.f,0.f,
// 	0.f,0.f,0.f,0.f,0.f,0.f,
// 	1.57f, -0.17f, 1.57f, 0.05f,
// 	0.f,0.f,
// };

const float UNSWalkProvider::INITIAL_BODY_POSE_ANGLES[] {
	1.57f, 0.17f, -1.57f, -0.05f,
	0.0f, 0.0f, -0.44f, 0.98f, -0.55f, 0.0f,
	0.0f, 0.0f, -0.44f, 0.98f, -0.55f, 0.0f,
	1.57f, -0.17f, 1.57f, 0.05f,
	0.f,0.f,
};

// Runswift takes walk commands in MM, we use CM
const float MM_PER_CM  = 10.0;
const float MAX_FORWARD = .25;
const float MAX_LEFT = .1; //.2;                                 // meters
const float MAX_TURN = .87;                                // radians


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
	std::cout << "Constructing walk provider!\n";
	generator =  new ClippedGenerator((Generator*) new DistributedGenerator());
	odometry = new Odometry();
	joints = new JointValues();
	imuAdjuster = new IMUAdjuster();
	// Touch* nullTouch = (Touch*) new NullTouch();
	touch = (Touch*) new FilteredTouch();
    ((FilteredTouch*)touch)->NBSetOptions();
    savedHeadingOdo = 0.0;
    slipAverage = 0.0;
    lastAngleZ = 0.0;
    angleChanged = 0.0;

    counter = 0;
	
	resetAll();
}

UNSWalkProvider::~UNSWalkProvider()
{
	delete generator;
	delete odometry;
	delete &joints;
	delete startOdometry;
	delete imuAdjuster;
}

void UNSWalkProvider::resetAll() {
	inactive();
	generator->reset();
	resetOdometry();
	//reset odometry through walk2014gen
	//reset other parts of walk2014gen
}

bool UNSWalkProvider::calibrated() const {
	// std::cout << "not calibrated!!" << std::endl;
	// imuAdjuster->findAvgOffset(sensorInertials.gyr_x(), sensorInertials.gyr_y());
	return imuAdjuster->isDone();
	// return false;
}

void UNSWalkProvider::adjustIMU(const ::messages::InertialState&	sensorInertials) {
	imuAdjuster->findAvgOffset(sensorInertials.gyr_x(), sensorInertials.gyr_y(), 
		sensorInertials.acc_x(), sensorInertials.acc_y(), sensorInertials.acc_z());
}

bool UNSWalkProvider::upright() const {
	return imuAdjuster->isFalling();
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
	const ::messages::InertialState&  sensorInertials,
	const ::messages::FSR& 			sensorFSRs
	) 
{
	PROF_ENTER(P_WALK);
	// logMsg("\n");

	ActionCommand::All* request = new ActionCommand::All();
	request->body.actionType = ActionCommand::Body::WALK;


	// if (!calibrated()) { logMsg("not calibrated");} 
	// else {logMsg("calibrated!!!"); }
	// std::cout << "" << std::endl;

	// logMsg("Calculating next joints and stiffnesses \n");

	if (standby) {
		// logMsg("in standby");
		tryingToWalk = false;
	} else {
		if (requestedToStop || !isActive()) {
			tryingToWalk = false;
			// logMsg("requested to stop or is not active");
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
			// logMsg("Walk command - Walking!");
		 	float WALK_SPEED_SCALE_X = 1000.0;
		 	float WALK_SPEED_SCALE_Y = 1000.0;
		 	float SPEED_SCALE = 1000.0;
			// HANDLE
			tryingToWalk = true;

			WalkCommand::ptr command = boost::shared_static_cast<WalkCommand>(currentCommand);
			// std::cout << "Walk Command: " << command->x_percent << "," << command->y_percent << "," << command->theta_percent << ") \n";
			// std::cout << "Walk Command Scaled: " << command->x_percent * MAX_FORWARD << "," << command->y_percent * MAX_LEFT << "," << command->theta_percent * MAX_TURN << ") \n";
			// std::cout << "Walk Command Scaled: " << command->x_percent * MAX_FORWARD * WALK_SPEED_SCALE_X << "," << command->y_percent * MAX_LEFT * WALK_SPEED_SCALE_X << ") \n";
			// request->body.forward = command->x_percent * WALK_SPEED_SCALE_X ;
			// request->body.left = command->y_percent * WALK_SPEED_SCALE_Y;
			// request->body.turn = command->theta_percent ;
			// std::cout << "FORWARD: " << request->body.forward << std::endl;
			request->body.speed = 0.0f;


			request->body.forward = command->x_percent * MAX_FORWARD * WALK_SPEED_SCALE_X;
			request->body.left = command->y_percent * WALK_SPEED_SCALE_Y;
			request->body.turn = command->theta_percent * MAX_TURN;

			// std::cout << "\nAfter walk: " << request->body.forward << " and command: " << command->x_percent << std::endl;
			// std::cout << "\nAfter walk y: " << request->body.left << " and command: " << command->y_percent << std::endl;

		} else if (currentCommand.get() && currentCommand->getType() == MotionConstants::DESTINATION) {
			// logMsg("\n\nDestination command - Destination Walking!");
			tryingToWalk = true;
			float DEST_SCALE = 1;

			DestinationCommand::ptr command = boost::shared_static_cast<DestinationCommand>(currentCommand);
			request->body.forward = command->x_mm * DEST_SCALE;
			request->body.left = command->y_mm * DEST_SCALE;
			request->body.turn = command->theta_rads;

			request->body.speed = command->gain;

			// std::cout << "Dest Command: " << command->x_mm << "," << command->y_mm << "," << command->theta_rads << ") \n";
			// std::cout << "Scaled Dest Command: " << command->x_mm * DEST_SCALE << "," << command->y_mm * DEST_SCALE << ") \n";
			// std::cout << "Dest gain: " << command->gain << std::endl;
			// TODO incorporate motion kicks


		} else if (currentCommand.get() && currentCommand->getType() == MotionConstants::KICK) {
			// logMsg("Kick command sent now!");
			// tryingToWalk = false;
			//std::cout << "Kick Command" << std::endl;
            // kickCommand = boost::shared_static_cast<KickCommand>(currentCommand);

            // // Only set kicking to true once
            // if (kickCommand->timeStamp != kickIndex) {
            //     std::cout << "Sent" << std::endl;
            //     kickIndex = kickCommand->timeStamp;
            //     // kickOut = KickEngineOutput();
            //     kicking = true;
            // }
            // else if (!kicking) { // Ignore the command if we've finished kicking
            //     std::cout << "!kicking" << std::endl;
            //     stand();
            // }

		} else if (currentCommand.get() && currentCommand->getType() == MotionConstants::WALK_IN_PLACE) {
			std::cout << "Walking in place! " << std::endl;
			request->body.forward = 00.0; //command->x_percent ;
			request->body.left = 00.0; //command->y_percent ;
			request->body.turn = 0.0; //UNSWDEG2RAD(90.0); //command->theta_percent ;
			request->body.speed = 0.0f;
		}
		else if (!currentCommand.get()) {
			// logMsg("Can't get current command! Requesting stand");
			tryingToWalk = false;
			// call stand
			request->body.actionType = ActionCommand::Body::STAND;
		}

		// TODO handle kick
	}
	request->body.speed = 0.0f;
	adjustIMU(sensorInertials);


	// Testing kick
	// request->body.actionType = ActionCommand::Body::KICK;

	// std::cout << "[WALK PROVIDER] Odometry: forward: " << odometry->forward << " left: " << odometry->left << " turn: " << odometry->turn << std::endl;
	// request->body.forward = -200.0; //command->x_percent ;
	// request->body.left = -100.0; //command->y_percent ;
	// request->body.turn = 0.0; //UNSWDEG2RAD(90.0); //command->theta_percent ;
	// request->body.speed = 0.0f;

	// For testing stand action
	// logMsg("Can't get current command! Requesting stand");
	// request->body.actionType = ActionCommand::Body::STAND;

	// Update sensor values
	UNSWSensorValues sensors = new UNSWSensorValues();
	sensors.joints = joints;

	sensors.sensors[Sensors::InertialSensor_GyrX] = sensorInertials.gyr_x();
    sensors.sensors[Sensors::InertialSensor_GyrY] = sensorInertials.gyr_y();
 //    sensors.sensors[Sensors::InertialSensor_GyrZ] = sensorInertials.gyr_z(); // they don't use this

    sensors.sensors[Sensors::InertialSensor_AccX] = sensorInertials.acc_x() * 0.01;
    sensors.sensors[Sensors::InertialSensor_AccY] = sensorInertials.acc_y() * 0.01;
    sensors.sensors[Sensors::InertialSensor_AccZ] = sensorInertials.acc_z() * 0.01;

    // Possibly wrong scale, but also it doesn't look like these are used in the walk generator
    sensors.sensors[Sensors::InertialSensor_AngleX] = sensorInertials.angle_x();
    sensors.sensors[Sensors::InertialSensor_AngleY] = sensorInertials.angle_y();

    imuAdjuster->adjustIMUs(sensorInertials.gyr_x(), sensorInertials.gyr_y(), sensorInertials.angle_x(),
    	sensorInertials.angle_y());
    float adjGyrX = imuAdjuster->getGyrX();
    float adjGyrY = imuAdjuster->getGyrY();

    // std::cout << "ADJUSTED InertialSensor_GyrX: " << adjGyrX << ", InertialSensor_GyrY: " << adjGyrY << std::endl;
    // std::cout << "ADJUSTED InertialSensor_AccX: " << adjAccX << ", InertialSensor_AccY: " << adjAccY << ", InertialSensor_AccZ: " << adjAccZ << std::endl;

    // std::cout << "InertialSensor_AccX: " << sensorInertials.acc_x() << ", InertialSensor_AccY: " << sensorInertials.acc_y() << std::endl;
    
    // std::cout << "InertialSensor_AngleX: " << -RAD2DEG(sensorInertials.angle_x()) << ", InertialSensor_AngleY: " << -RAD2DEG(sensorInertials.angle_y()) << std::endl;
    // sensors.sensors[Sensors::angleZ] = sensorInertials.angle_z(); // this either

    sensors.sensors[Sensors::LFoot_FSR_FrontLeft] = sensorFSRs.lfl();
    sensors.sensors[Sensors::LFoot_FSR_FrontRight] = sensorFSRs.lfr();
    sensors.sensors[Sensors::LFoot_FSR_RearLeft] = sensorFSRs.lrl();
    sensors.sensors[Sensors::LFoot_FSR_RearRight] = sensorFSRs.lrr();

    sensors.sensors[Sensors::RFoot_FSR_FrontLeft] = sensorFSRs.rfl();
    sensors.sensors[Sensors::RFoot_FSR_FrontRight] = sensorFSRs.rfr();
    sensors.sensors[Sensors::RFoot_FSR_RearLeft] = sensorFSRs.rrl();
    sensors.sensors[Sensors::RFoot_FSR_RearRight] = sensorFSRs.rrr();

    // I'm distrustful of this for loop, so I've explicitly wired the joints
    // for (int i = 0; i < 21; i++) {
    // 	sensors.joints.angles[nb_joint_order[i]] = sensorAngles[i];
    // }

    sensors.joints.angles[Joints::LShoulderPitch] = sensorAngles[sensors::LShoulderPitch];
    sensors.joints.angles[Joints::LShoulderRoll] = sensorAngles[sensors::LShoulderRoll];
    sensors.joints.angles[Joints::LElbowYaw] = sensorAngles[sensors::LElbowYaw];
    sensors.joints.angles[Joints::LElbowRoll] = sensorAngles[sensors::LElbowRoll];
    
    sensors.joints.angles[Joints::LHipYawPitch] = sensorAngles[sensors::LHipYawPitch];
    sensors.joints.angles[Joints::LHipRoll] = sensorAngles[sensors::LHipRoll];
    sensors.joints.angles[Joints::LHipPitch] = sensorAngles[sensors::LHipPitch];
    sensors.joints.angles[Joints::LKneePitch] = sensorAngles[sensors::LKneePitch];
    sensors.joints.angles[Joints::LAnklePitch] = sensorAngles[sensors::LAnklePitch];
    sensors.joints.angles[Joints::LAnkleRoll] = sensorAngles[sensors::LAnkleRoll];
    
    sensors.joints.angles[Joints::RHipRoll] = sensorAngles[sensors::RHipRoll];
    sensors.joints.angles[Joints::RHipPitch] = sensorAngles[sensors::RHipPitch];
    sensors.joints.angles[Joints::RKneePitch] = sensorAngles[sensors::RKneePitch];
    sensors.joints.angles[Joints::RAnklePitch] = sensorAngles[sensors::RAnklePitch];
    sensors.joints.angles[Joints::RAnkleRoll] = sensorAngles[sensors::RAnkleRoll];

    sensors.joints.angles[Joints::RShoulderPitch] = sensorAngles[sensors::RShoulderPitch];
    sensors.joints.angles[Joints::RShoulderRoll] = sensorAngles[sensors::RShoulderRoll];
    sensors.joints.angles[Joints::RElbowYaw] = sensorAngles[sensors::RElbowYaw];
    sensors.joints.angles[Joints::RElbowRoll] = sensorAngles[sensors::RElbowRoll];


    // Not sure if this is totally necessary, TBH
    UNSWSensorValues filteredSensors = ((FilteredTouch*)touch)->getSensors(kinematics, sensors);

    sensors = filteredSensors;

	sensors.sensors[Sensors::InertialSensor_GyrX] = adjGyrX * 0.01;
    sensors.sensors[Sensors::InertialSensor_GyrY] = adjGyrY * 0.01;

	// TODO kinematics parameters - move these...
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
	float ballX = 1.0;
	float ballY = 1.0;

    // Update the body model
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


    // std::cout << "Right before make joints: " << request->body.forward << std::endl;
    joints = generator->makeJoints(request, odometry, sensors, bodyModel, ballX, ballY);

    updateOdometry(sensorInertials.angle_z());

    angles = joints.angles;
    hardness = joints.stiffnesses;


    // THIS IS WHERE WE ACTUALLY SET THE NEXT JOINTS AND STIFFNESSES
    // Ignore the first chain because it's the head
    for (unsigned i = 1; i < Kinematics::NUM_CHAINS; i++) {
    	std::vector<float> chain_angles;
    	std::vector<float> chain_hardness;
    	for (unsigned j = Kinematics::chain_first_joint_UNSWALK[i]; j <= Kinematics::chain_last_joint_UNSWALK[i]; j++) {

    		if ((Kinematics::ChainID)i == Kinematics::RLEG_CHAIN && j == 11) {
    			// RHIPYAWPITCH -- technically doesn't exist but in all our infinite grace and wisdom we have a variable for it anyway
    			chain_angles.push_back(joints.angles[nb_joint_order[6]]);
    			// logMsg("SPECIAL CASE: used " + Joints::jointNames[nb_joint_order[6]]);
    		} else {
	    		chain_angles.push_back(joints.angles[nb_joint_order[j]]); 
    		}

   //  		logMsgNoEL("ANGLE in "  + Joints::jointNames[nb_joint_order[j]] + " = ");
			// // std::cout << RAD2DEG(joints.angles[nb_joint_order[j]]);
			// std::cout << (joints.angles[nb_joint_order[j]]) << std::endl;

    		if (hardness[nb_joint_order[j]] == 0) {
				// JUST PUSH BACK 1 so arms move
    			// TODO maybe take this out so the limp arm things is implemented
				chain_hardness.push_back(1.0);
    			// chain_hardness.push_back(MotionConstants::NO_STIFFNESS);
    		} else {
    			chain_hardness.push_back(joints.stiffnesses[nb_joint_order[j]]);
    		}
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
	savedHeadingOdo = odometry->turn;
	lastAngleZ = 0.0;
	angleChanged = 0.0;
	odometry->clear();
}

void UNSWalkProvider::setCommand(const WalkCommand::ptr command) {
	if (command->theta_percent == 0 && command->x_percent == 0 && command->y_percent == 0) {
		std::cout << "Stand command!\n";
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

void UNSWalkProvider::setCommand(const WalkInPlaceCommand::ptr command) {
	currentCommand = command;
	active();
}

void UNSWalkProvider::updateOdometry(float angleZ) {

#ifdef V5_ROBOT
    angleZ = -angleZ;
    if (angleChanged == 0.0 && lastAngleZ == 0.0) {
    	// First change, so there's not a big jump in the angle and we don't think we've turned
    	lastAngleZ = angleZ;
    	// std::cout << "Initting the angle changing!\n";
    	return;
    }
    // std::cout << "Angle Z: " << angleZ << std::endl;
    angleChanged += angleZ - lastAngleZ;
    lastAngleZ = angleZ;
    // odometry->turn = angleChanged;
#else
    angleChanged = odometry->turn;
#endif

    // if (abs(gyroZ) > 0) {
    // 	std::cout << "[GYROZ] the abs gyroz was: " << gyroZ << std::endl;
    //     // convert to radians per frame, same direction as walkChange
    //     gyroZ *= -0.01f * 0.95f;

    //     float slipError = gyroZ - odometry->turn;
    //     slipAverage = 0.3f*slipError + (1.f - 0.3f)*slipAverage;

    //     // use gyroZ if there is sufficient difference, don't always use it due to gyro drift skewing the result
    //     if (fabs(slipAverage) > 0.005f){
    //     	std::cout << "USING GYRO!!!\n";
    //         odometry->turn = gyroZ;
    //     }
    // }
    
}

void UNSWalkProvider::getOdometryUpdate(portals::OutPortal< ::messages::RobotLocation>& out) const {

	portals::Message< ::messages::RobotLocation> odometryData(0);

    // double dx = odometry->forward*cos(odometry->turn) - odometry->left*sin(odometry->turn);
    // double dy = odometry->forward*sin(odometry->turn) + odometry->left*cos(odometry->turn);

    double dx = odometry->forward*cos(angleChanged) - odometry->left*sin(angleChanged);
    double dy = odometry->forward*sin(angleChanged) + odometry->left*cos(angleChanged);

	odometryData.get()->set_x(dx * MM_TO_CM);
	odometryData.get()->set_y(dy * MM_TO_CM);
	odometryData.get()->set_h(angleChanged);


    // std::cout << "MY ODO: " << odometry->forward << " Y: " << odometry->left * 100 << " TURN: " << odometry->turn << std::endl;
    // std::cout << "MY DX: " << dx << " MY DY: " << dy << std::endl;
    // std::cout << "AngledChanged: " << angleChanged << std::endl << std::endl;

	out.setMessage(odometryData);
}

double normaliseTheta(double theta) {
   double r = fmod(theta - M_PI, 2.0*M_PI);
   if (r > 0) {
      return r - M_PI;
   } else {
      return r + M_PI;
   }
}

bool UNSWalkProvider::isStanding() const { //is going to stand rather than at complete standstill
	return generator->isStanding(); //1 corresponds to process of moving from WALK crouch to STAND
}

bool UNSWalkProvider::isWalkActive() const {
	// TODO check this!
	return generator->isStanding();
}

void UNSWalkProvider::stand() {
	// std::cout << "STAND IS BEING CALLED!!\n";
	currentCommand = MotionCommand::ptr();

	// UNTIL STAND IS WORKED OUT, JUST WALK IN PLACE
	// CHANGE THIS BEFORE COMPETITION
	// currentCommand = WalkInPlaceCommand::ptr(new WalkInPlaceCommand());
}


// void UNSWalkProvider::restingStand()
// {

// }

}

}