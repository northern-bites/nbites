#include "VisualFieldObject.h"

VisualFieldObject::VisualFieldObject(const fieldObjectID _id)
    : id(_id)
{
    init();
}


VisualFieldObject::VisualFieldObject() : id(UNKNOWN_FIELD_OBJECT) {
    init();
}

VisualFieldObject::VisualFieldObject(const int _x, const int _y,
                                     const float _distance,
                                     const float _bearing)
    : VisualLandmark(_x, _y, _distance, _bearing) {
}
VisualFieldObject::VisualFieldObject(const VisualFieldObject&) {
}

// Initialization, happens every frame.
void VisualFieldObject::init(){
    width = 0;
    height = 0;
    x = 0;
    y = 0;
    centerX = 0;
    centerY = 0;
    angleX = 0;
    angleY = 0;
    focDist = 0;
    distance = 0;
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

void VisualFieldObject::printDebugInfo(FILE * out) {
    fprintf(out, "(%d, %d)\tdist: %f\tIDcertainty %d\n",
            x, y, distance, IDcertainty);
}
