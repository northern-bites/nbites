#include "NullTouch.hpp"

UNSWSensorValues NullTouch::getSensors(UNSWKinematics &kinematics) {
   return nullSensors;
}

bool NullTouch::getStanding() {
   return false;
}

ButtonPresses NullTouch::getButtons() {
   ButtonPresses b;
   return b;
}
