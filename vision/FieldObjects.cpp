/* FieldObjects class
 */

#include "FieldObjects.h"

const double FieldObjects::WHITE_HEIGHT_ON_BEACON = 18.6;

FieldObjects::FieldObjects(Vision *vis, 
                           const fieldObjectID _id) 
  : vision(vis), id(_id)
{
  init();
}


FieldObjects::FieldObjects(Vision *vis) 
  // stores vision class pointer
  : vision(vis), id(UNKNOWN_FIELD_OBJECT)
{
  init();
}

FieldObjects::FieldObjects() : id(UNKNOWN_FIELD_OBJECT) {
  init();
}



// Initialization, happens every frame.
void FieldObjects::init(){
  width = 0;
  height = 0;
  x = 0;
  y = 0;
  centerX = 0;
  centerY = 0;
  angleX = 0;
  angleY = 0;
  focDist = 0;
  dist = 0;
  bearing = 0;
  elevation = 0;
  certainty = 0;

  switch (id) {
  case BLUE_GOAL_LEFT_POST:
    fieldLocation = BLUE_GOAL_LEFT_POST_LOC;
    break;
  case BLUE_GOAL_RIGHT_POST:
    fieldLocation = BLUE_GOAL_RIGHT_POST_LOC;
    break;
  case YELLOW_GOAL_LEFT_POST:
    fieldLocation = YELLOW_GOAL_LEFT_POST_LOC;
    break;
  case YELLOW_GOAL_RIGHT_POST:
    fieldLocation = YELLOW_GOAL_RIGHT_POST_LOC;
    break;
  case YELLOW_BLUE_BEACON:
    fieldLocation = YELLOW_BLUE_BEACON_LOC;
    break;
  case BLUE_YELLOW_BEACON:
    fieldLocation = BLUE_YELLOW_BEACON_LOC;
    break;
    
    /*
  case BLUE_YELLOW_ARC:
    //    fieldLocation = BLUE_YELLOW_ARC
    break;
  case YELLOW_BLUE_ARC:
  break;*/

  case UNKNOWN_FIELD_OBJECT:
    fieldLocation = UNKNOWN_LOC;
  default:
    fieldLocation = UNKNOWN_LOC;
    break;
  }
}


string FieldObjects::getStringFromID(fieldObjectID _id) {
  switch (_id) {
  case BLUE_GOAL_LEFT_POST:
    return "Blue goal left post";
  case BLUE_GOAL_RIGHT_POST:
    return "Blue goal right post";
  case YELLOW_GOAL_LEFT_POST:
    return "Yellow goal left post";
  case YELLOW_GOAL_RIGHT_POST:
    return "Yellow goal right post";
  case YELLOW_BLUE_BEACON:
    return "Yellow blue beacon";
  case BLUE_YELLOW_BEACON:
    return "Blue yellow beacon";
  case BLUE_ARC:
    return "Blue arc";
  case YELLOW_ARC:
  return "Yellow arc";
  default:
    return "Invalid ID";
  }
}

const double FieldObjects::getHeightFromGround(const fieldObjectID _id) {
  switch (_id) {
  case BLUE_GOAL_LEFT_POST:
  case BLUE_GOAL_RIGHT_POST:
  case YELLOW_GOAL_LEFT_POST:
  case YELLOW_GOAL_RIGHT_POST:
  case BLUE_ARC:
  case YELLOW_ARC:
    return 0;
    // The beacons only consider the blue and yellow portion, which are
    // elevated off the ground
  case YELLOW_BLUE_BEACON:
  case BLUE_YELLOW_BEACON:
    return WHITE_HEIGHT_ON_BEACON;
  default:
    return 0;
  }
}

const bool FieldObjects::isGoal(const FieldObjects * obj) {
  switch (obj->getID()) {
  case BLUE_GOAL_LEFT_POST: // Intentional fall throughs
  case BLUE_GOAL_RIGHT_POST:
  case YELLOW_GOAL_LEFT_POST:
  case YELLOW_GOAL_RIGHT_POST:
    return true;
  default:
    return false;
  }
}

const bool FieldObjects::isBeacon(const FieldObjects * obj) {
  switch (obj->getID()) {
  case YELLOW_BLUE_BEACON: // Intentional fall through
  case BLUE_YELLOW_BEACON:
    return true;
  default:
    return false;
  }
}

const bool FieldObjects::isArc(const FieldObjects * obj) {
  switch (obj->getID()) {
  case BLUE_ARC: // Intentional fall through
  case YELLOW_ARC:
    return true;
  default:
    return false;
  }
}


void FieldObjects::printDebugInfo(FILE * out) {
  fprintf(out, "(%d, %d)\tdist: %f\tcertainty %d\n",
          x, y, dist, certainty);
}
