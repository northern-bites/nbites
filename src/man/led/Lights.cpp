#include "Lights.h"
const std::string Lights::LED_NAMES[Lights::NUM_LED_NAMES] = {
    "LeftLoc1","LeftLoc2","LeftLoc3","LeftLoc4","LeftLoc5",
	"RightLoc1","RightLoc2","RightLoc3","RightLoc4","RightLoc5",
	"LeftComm1","LeftComm2","LeftComm3","LeftComm4","LeftComm5",
	"RightComm1","RightComm2","RightComm3","RightComm4","RightComm5",
    "SubRole","Role","Ball",
	"LeftGoal","RightGoal","GoalID"
    "Chest",
    "LeftFoot", "RightFoot",
	"LeftUnused","RightUnused"};


Lights::Lights(){};
Lights::~Lights(){};

