#include "Oracle.h"
#include "utils/Logger.hpp"
#include "motion/touch/SimTouch.hpp"

using namespace std;

SimTouch::SimTouch() : isFirstTime(true) {
   llog(INFO) << "Sim Touch initialised" << endl;
}

SimTouch::~SimTouch() {
   llog(INFO) << "Sim Touch destroyed" << endl;
}

SensorValues SimTouch::getSensors(Kinematics &kinematics) {
   return oracle->getSensorValues();
}

bool SimTouch::getStanding() {
   if (isFirstTime) {
      isFirstTime = false;
      return true;
   }
   return false;
}


ButtonPresses SimTouch::getButtons() {
   return *(new ButtonPresses());
}
