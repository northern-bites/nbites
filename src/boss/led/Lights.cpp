#include "Lights.h"
const std::string Lights::LED_NAMES[Lights::NUM_LED_NAMES] = {
	"brain0", "brain1", "brain2", "brain3", "brain4", "brain5", "brain6",
	"brain7", "brain8", "brain9", "brain10", "brain11",

	"rightEye0", "rightEye1", "rightEye2", "rightEye3", "rightEye4", 
	"rightEye5", "rightEye6", "rightEye7", 

	"leftEye0", "leftEye1", "leftEye2", "leftEye3", "leftEye4", "leftEye5", 
	"leftEye6", "leftEye7", 

	"rightEar0", "rightEar1", "rightEar2", "rightEar3", "rightEar4", 
	"rightEar5", "rightEar6", "rightEar7", "rightEar8", "rightEar9", 

	"leftEar0", "leftEar1", "leftEar2", "leftEar3", "leftEar4", "leftEar5", 
	"leftEar6", "leftEar7", "leftEar8", "leftEar9",

	"chest", "rightFoot", "leftFoot"};
 //    "LeftCalibration1","LeftCalibration2","LeftCalibration3","LeftCalibration4","LeftCalibration5",
 //    "RightCalibration1","RightCalibration2","RightCalibration3","RightCalibration4","RightCalibration5","LeftComm1","LeftComm2","LeftComm3",
 //    "LeftComm4","LeftComm5","RightComm1","RightComm2","RightComm3","RightComm4","RightComm5",
 //    "Role",                       // Left Eye
 //    "Ball", "GoalBox",            // Right Eye
 //    "Chest",
 //    "LeftFoot", "RightFoot",
	// "eyeZero", "eyeOne", "eyeTwo", "eyeThree", "eyeFour", "eyeFive", "eyeSix", "eyeSeven", "Brain8", "Brain9", "Brain10", "Brain11"};


Lights::Lights(){};
Lights::~Lights(){};
