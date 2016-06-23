#include "gamecontroller/RoboCupGameControlData.hpp"

const char *gameControllerStateNames[] = {
   "initial",
   "ready",
   "set",
   "playing",
   "finished",
   "invalid",
   "penalised"
};

const char *gameControllerPenaltyNames[] = {
   "none",
   "ball holding",
   "player pushing",
   "obstruction",
   "inactive player",
   "illegal defender",
   "leaving the field",
   "hand ball",
   "request for pickup",
   "invalid",
   "invalid",
   "invalid",
   "invalid",
   "invalid",
   "invalid",
   "manual penalty",
};
