#include "NullTouch.hpp"

SensorValues NullTouch::getSensors(Kinematics &kinematics) {
   return nullSensors;
}

bool NullTouch::getStanding() {
   return false;
}

ButtonPresses NullTouch::getButtons() {
   ButtonPresses b;
   return b;
}
