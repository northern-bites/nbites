/**
 * Observation.cpp - The landmark observation class. Here we house all those
 * things needed to describe a single landmark sighting.  Observations can be of
 * field objects, corners (line intersections), lines, and possibly other
 * things in the future (i.e. carpet edges)
 *
 * @author Tucker Hermans
 */

#include "Observation.h"

/**
 * @param fo FieldObject that was seen and reported.
 */
Observation::Observation(FieldObject &_object)
{
    // We aren't a line
    line_truth = false;
    visDist = _object.getDist();
    visBearing = _object.getBearing();
    // sigma_d = _object.getDistanceSD();
    // sigma_b = _object.getBearingSD();
    sigma_d = visDist * 4.0f;
    sigma_b = visBearing * 4.0f;

    // Figure out which possible landmarks we have...
    // This should be cleaner like in corners, once field objects is in line...
    if (_object.getCertainty() == SURE) {
        PointLandmark objectLandmark;
        if ( _object.getID() == BLUE_GOAL_LEFT_POST) {
            objectLandark.x = ConcreteFieldObject::blue_goal_left_post.getX();
            objectLandark.y = ConcreteFieldObject::blue_goal_left_post.getY();
        } else if( _object.getID() == BLUE_GOAL_RIGHT_POST) {
            objectLandark.x = ConcreteFieldObject::blue_goal_right_post.getX();
            objectLandark.y = ConcreteFieldObject::blue_goal_right_post.getY();
        } else if ( _object.getID() == YELLOW_GOAL_LEFT_POST) {
            objectLandark.x = ConcreteFieldObject::yellow_goal_left_post.getX();
            objectLandark.y = ConcreteFieldObject::yellow_goal_left_post.getY();
        } else if( _object.getID() == YELLOW_GOAL_RIGHT_POST) {
            objectLandark.x =ConcreteFieldObject::yellow_goal_right_post.getX();
            objectLandark.y =ConcreteFieldObject::yellow_goal_right_post.getY();
        }
        pointPossibilities.push_back(objectLandmark);
        numPossibilities = 1;
        return;
    }

    list <const ConcreteFieldObject *> objList;
    if ( _object.getID() == BLUE_GOAL_LEFT_POST ||
         _object.getID() == BLUE_GOAL_RIGHT_POST) {
            objList = ConcreteFieldObject::blue_goal_posts;
    } else {
            objList = ConcreteFieldObject::yellow_goal_posts;
    }
    // Initialize to 0 possibilities
    numPossibilites = 0;

    list <const ConcreteFieldObject *>::iterator theIterator;
    //list <const ConcreteFieldObject *> objList = _object.getPossibleFieldObjects();
    for( theIterator = objList.begin(); theIterator != objList.end();
         ++theIterator) {
        PointLandmark objectLandmark;
        objectLandmark.x = (*theIterator).getFieldX();
        objectLandmark.y = (*theIterator).getFieldY();
        pointPossibilities.push_back(objectLandmark);
        ++numPossibilities;
    }
}

/**
 * @param c Corner that was seen and reported.
 */
Observation::Observation(VisualCorner &_corner)
{
    // We aren't a line
    line_truth = false;

    // Get basic vision information
    visDist = _corner.getDistance();
    visBearing = _corner.getBearing();
    //sigma_d = _corner.getDistanceSD();
    //sigma_b = _corner.getBearingSD();
    sigma_d = visDist * 4.0f;
    sigma_b = visBearing * 4.0f;

    // Build our possibilitiy list
    numPossibilities = 0;

    list <const ConcreteCorner *>::iterator theIterator;
    list <const ConcreteCorner *> cornerList = _corner.getPossibleCorners();
    for( theIterator = cornerList.begin(); theIterator != cornerList.end();
         ++theIterator) {
        PointLandmark cornerLandmark;
        cornerLandmark.x = (*theIterator).getFieldX();
        cornerLandmark.y = (*theIterator).getFieldY();
        pointPossibilities.push_back(cornerLandmark);
        ++numPossibilities;
    }
}

/**
 * @param l Line that was seen and reported.
 */
Observation::Observation(VisualLine &_line)
{
    // We're a line
    line_truth = true;

    // Get basic vision information
    visDist = _line.getDistance();
    visBearing = _line.getBearing();
    // sigma_d = _line.getDistanceSD();
    // sigma_b = _line.getBearingSD();
    sigma_d = visDist * 4.0f;
    sigma_b = visBearing * 4.0f;

    // Build our possibilitiy list
    numPossibilities = 0;

    list <const ConcreteLine *>::iterator theIterator;
    list <const ConcreteLine *> lineList = _line.getPossibleLines();
    for( theIterator = lineList.begin(); theIterator != lineList.end();
         ++theIterator) {
        LineLandmark addLine;
        addLine.x1 = (*theIterator).getFieldX1();
        addLine.y1 = (*theIterator).getFieldY1();
        addLine.x2 = (*theIterator).getFieldX1();
        addLine.y2 = (*theIterator).getFieldY1();
        linePossibilities.push_back(addLine);
        ++numPossibilities;
    }

}


/**
 * Deconstructor for our observation
 */
virtual Observation::~Observation()
{
    if (line_truth) {
    } else {
    }
}
