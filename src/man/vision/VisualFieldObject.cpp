#include "VisualFieldObject.h"
using namespace std;

const float VisualFieldObject::BOTH_UNSURE_DISTANCE_SD = 2500;

VisualFieldObject::VisualFieldObject(const fieldObjectID _id)
    : VisualObject(_id)
{
    init();
    framesOn = 0;
    framesOff = 0;
}


VisualFieldObject::VisualFieldObject()
    : VisualObject(UNKNOWN_FIELD_OBJECT)
 {
    init();
    framesOn = 0;
    framesOff = 0;
}

VisualFieldObject::VisualFieldObject(const int _x, const int _y,
                                     const float _distance,
                                     const float _bearing)
    : VisualObject(UNKNOWN_FIELD_OBJECT,
                                  _x, _y, _distance, _bearing)
{
}
VisualFieldObject::VisualFieldObject(const VisualFieldObject& other) :
    VisualObject(other)
{
}

// Initialization, happens every frame.
void VisualFieldObject::init(){
    width = 0;
    height = 0;
    setX(0);
    setY(0);
    centerX = 0;
    centerY = 0;
    angleX = 0;
    angleY = 0;
    setDistance(0);
    setBearing(0);
    elevation = 0;
    idCertainty = NOT_SURE;
    on = false;

    switch (id) {
    case BLUE_GOAL_LEFT_POST:
        fieldLocation = BLUE_GOAL_TOP_POST_LOC;
        fieldLocation2 = BLUE_GOAL_BOTTOM_POST_LOC;
        setPossibleFieldObjects(&ConcreteFieldObject::blueGoalPosts);
        break;
    case BLUE_GOAL_RIGHT_POST:
        fieldLocation = BLUE_GOAL_BOTTOM_POST_LOC;
        fieldLocation2 = BLUE_GOAL_TOP_POST_LOC;
        setPossibleFieldObjects(&ConcreteFieldObject::blueGoalPosts);
        break;
    case YELLOW_GOAL_LEFT_POST:
        fieldLocation = YELLOW_GOAL_BOTTOM_POST_LOC;
        fieldLocation2 = YELLOW_GOAL_TOP_POST_LOC;
        setPossibleFieldObjects(&ConcreteFieldObject::yellowGoalPosts);
        break;
    case YELLOW_GOAL_RIGHT_POST:
        fieldLocation = YELLOW_GOAL_TOP_POST_LOC;
        fieldLocation2 = YELLOW_GOAL_BOTTOM_POST_LOC;
        setPossibleFieldObjects(&ConcreteFieldObject::yellowGoalPosts);
        break;
    case UNKNOWN_FIELD_OBJECT:
        fieldLocation = UNKNOWN_LOC;
    default:
        fieldLocation = UNKNOWN_LOC;
        break;
    }
}

/**
 * Calculate and set the standard deviation for the distance measurement.
 * Set the distance measurement.
 *
 * @param _distance the distance estimate to be set
 */
void VisualFieldObject::setDistanceWithSD(float _distance)
{
    setDistance(_distance);

    if (getDistanceCertainty() == BOTH_UNSURE) {
        setDistanceSD(BOTH_UNSURE_DISTANCE_SD);
    } else {
        // Calculate the SD based on the object type
        switch (getID()) {
        case BLUE_GOAL_LEFT_POST:
        case BLUE_GOAL_RIGHT_POST:
        case YELLOW_GOAL_LEFT_POST:
        case YELLOW_GOAL_RIGHT_POST:
            setDistanceSD(postDistanceToSD(_distance));
            break;
        default:
            setDistanceSD(postDistanceToSD(_distance));
            break;
        }
    }
}

/**
 * Calculate and set the standard deviation for the bearing measurement.
 * Set the bearing measurement.
 *
 * @param _bearing the distance estimate to be set
 */
void VisualFieldObject::setBearingWithSD(float _bearing)
{
    setBearing(_bearing);
    // Calculate the SD based on the object type
    switch (getID()) {
    case BLUE_GOAL_LEFT_POST:
    case BLUE_GOAL_RIGHT_POST:
    case YELLOW_GOAL_LEFT_POST:
    case YELLOW_GOAL_RIGHT_POST:
        setBearingSD(postBearingToSD(_bearing));
        break;
    default:
        setDistanceSD(postBearingToSD(_bearing));
        break;
    }
}

void VisualFieldObject::printDebugInfo(FILE * out) {
    fprintf(out, "(%d, %d)\tdist: %f\tIDcertainty %d\n",
            getX(), getY(), getDistance(), getIDCertainty());
}

/*
 * When setting certainty, set list of possible objects
 */
void VisualFieldObject::setIDCertainty(certainty _cert)
{
    idCertainty = _cert;
    if (_cert == NOT_SURE) {
        switch(getID()) {
        case BLUE_GOAL_LEFT_POST:
        case BLUE_GOAL_RIGHT_POST:
        case BLUE_GOAL_POST:
            setPossibleFieldObjects(&ConcreteFieldObject::blueGoalPosts);
            break;
        case YELLOW_GOAL_LEFT_POST:
        case YELLOW_GOAL_RIGHT_POST:
        case YELLOW_GOAL_POST:
            setPossibleFieldObjects(&ConcreteFieldObject::yellowGoalPosts);
            break;
        default:
            // We don't actually care about the possible field objects
            break;
        }
    } else { // We know what the field object is
        switch(getID()) {
        case BLUE_GOAL_LEFT_POST:
            setPossibleFieldObjects(&ConcreteFieldObject::blueGoalLeftPostList);
            break;
        case BLUE_GOAL_RIGHT_POST:
            setPossibleFieldObjects(&ConcreteFieldObject::blueGoalRightPostList);
            break;
        case BLUE_GOAL_POST: // This should probably never be reached
            setPossibleFieldObjects(&ConcreteFieldObject::blueGoalPosts);
            break;
        case YELLOW_GOAL_LEFT_POST:
            setPossibleFieldObjects(
                &ConcreteFieldObject::yellowGoalLeftPostList);
            break;
        case YELLOW_GOAL_RIGHT_POST:
            setPossibleFieldObjects(
                &ConcreteFieldObject::yellowGoalRightPostList);
            break;
        case YELLOW_GOAL_POST: // This should probably never be reached
            setPossibleFieldObjects(&ConcreteFieldObject::yellowGoalPosts);
            break;
        default:
            // We don't actually care about the possible field objects
            break;
        }
    }
}

void VisualFieldObject::updateObject(Blob * b, certainty _certainty,
                                     distanceCertainty _distCertainty)
{
    // before we do this let's make sure that the object is really our color
    setLeftTopX(b->getLeftTopX());
    setLeftTopY(b->getLeftTopY());
    setLeftBottomX(b->getLeftBottomX());
    setLeftBottomY(b->getLeftBottomY());
    setRightTopX(b->getRightTopX());
    setRightTopY(b->getRightTopY());
    setRightBottomX(b->getRightBottomX());
    setRightBottomY(b->getRightBottomY());
    setX(b->getLeftTopX());
    setY(b->getLeftTopY());
    setWidth(dist(b->getRightTopX(), b->getRightTopY(), b->getLeftTopX(),
                  b->getLeftTopY()));
    setHeight(dist(b->getLeftTopX(), b->getLeftTopY(), b->getLeftBottomX(),
                   b->getLeftBottomY()));
    setCenterX(getLeftTopX() + ROUND2(getWidth() / 2));
    setCenterY(getRightTopY() + ROUND2(getHeight() / 2));
    setIDCertainty(_certainty);
    setDistanceCertainty(_distCertainty);
    setDistance(1);
}

const bool VisualFieldObject::hasPositiveID()
{
	return possibleFieldObjects->size() == 1;
}
