#include "VisualCorner.h"
using namespace std;

//-------------------------------
// Static VisualCorner variables:
const point <int> VisualCorner::
dogLocation(IMAGE_WIDTH/2, IMAGE_HEIGHT - 1);

VisualCorner::VisualCorner(const int _x, const int _y,
                           const float _distance,
                           const float _bearing,
                           const VisualLine l1, const VisualLine l2,
                           const float _t1, const float _t2)
    : VisualDetection(_x, _y, _distance, _bearing), VisualLandmark(),
      cornerType(UNKNOWN),
      line1(l1), line2(l2), t1(_t1), t2(_t2),
      // Technically the initialization of tBar and tStem is incorrect here for
      // which we apologize. It's a hack, but the true values of tBar and tStem
      // will get assigned in determineCornerShape which is right here in the
      // constructor.
      tBar(line1), tStem(line2),
      angleBetweenLines(0) {

    determineCornerShape();

    // Calculate and set the standard deviation of the measurements
    setDistanceSD(cornerDistanceToSD(_distance));
    setBearingSD(cornerBearingToSD(_bearing));
}

VisualCorner::~VisualCorner() {}

VisualCorner::VisualCorner(const VisualCorner& other)
    : VisualDetection(other), VisualLandmark(other),
      possibleCorners(other.possibleCorners),
      cornerType(other.cornerType),
      line1(other.line1), line2(other.line2), t1(other.t1), t2(other.t2),
      tBar(other.tBar), tStem(other.tStem),
      angleBetweenLines(other.angleBetweenLines) {

}

/* This method will assign a value to the variable cornerType. It assumes
 * that the corner is not a cross corner. If it were it wouldn't have been
 * constructed in FieldLines::interesctLines()
 */
void VisualCorner::determineCornerShape() {
    if (Utility::tValueInMiddleOfLine(t1, line1.length, MIN_EXTEND_DIST)) {
        cornerType = T;
        tBar = line1;
        tStem = line2;
    } else if(Utility::tValueInMiddleOfLine(t2, line2.length,
                                            MIN_EXTEND_DIST)) {
        cornerType = T;
        tBar = line2;
        tStem = line1;
    } else {
        // Temporary side effect - set angleBetweenLines
        cornerType = getLClassification();
    }
}


// Given an L corner, determines whether it is an inner or outer L.  See the
// diagram on the wiki for more explanation
const shape VisualCorner::getLClassification() {

    const point<int> cornerLocation = getLocation();
    const int cornerX = getX();
    const int cornerY = getY();

    // In order to calculate the angle between the two lines, we determine
    // the vector emanating from the corner to the end of line 1, and the corner
    // to the end of line 2, then take the dot product and go from there.  The
    // tricky part is that the "end" of the line in terms of the end point
    // which is farther from the corner might be the start point of the
    // line.

    // Determine how the lines are represented as a vector.
    // Points from left to right; will flip the sign of both if the vector
    // should point in the opposite direction (the aforementioned end/start
    // issue)
    pair <int, int> line1Basis =
        VisualLine::getLineComponents(line1);
    pair <int, int> line2Basis =
        VisualLine::getLineComponents(line2);

    // Find the line endpoint that's farther from the corner (allows us
    // to direct our line segment away from the corner)
    point <int> line1End, line2End;

    // corner is closer to start point of line 1
    if (Utility::getLength(static_cast<float>(cornerX),
						   static_cast<float>(cornerY),
						   static_cast<float>(line1.start.x),
						   static_cast<float>(line1.start.y) ) <
        Utility::getLength(static_cast<float>(cornerX),
						   static_cast<float>(cornerY),
						   static_cast<float>(line1.end.x),
						   static_cast<float>(line1.end.y) )) {
        line1End = line1.end;
    }
    // Closer to end
    else {
        line1End = line1.start;
        // Swap the signs on the line 1 basis
        line1Basis.first *= -1;
        line1Basis.second *= -1;
    }

    // corner is closer to start point of line 2
    if (Utility::getLength(static_cast<float>(cornerX),
						   static_cast<float>(cornerY),
						   static_cast<float>(line2.start.x),
						   static_cast<float>(line2.start.y) ) <
        Utility::getLength(static_cast<float>(cornerX),
						   static_cast<float>(cornerY),
						   static_cast<float>(line2.end.x),
						   static_cast<float>(line2.end.y) )) {
        line2End = line2.end;
    }
    // Closer to end
    else {
        line2End = line2.start;
        // Swap the signs on the line 2 basis
        line2Basis.first *= -1;
        line2Basis.second *= -1;
    }


    // By this point, we now have the actual vector representations of the lines
    // and can calculate theta
    float dotProduct = static_cast<float>( (line1Basis.first *
											 line2Basis.first) +
										   (line1Basis.second *
											 line2Basis.second) );

    // v dot w = ||v|| ||w|| cos theta -> v dot w / (||v|| ||w||) = cos theta
    // -> ...
    float theta = TO_DEG * acos(dotProduct/(line1.length * line2.length));
    /*
      cout << " first line: " << line1->start << ", " << line1->end << endl;
      cout << " second line: " << line2->start << ", " << line2->end << endl;

      cout << "First basis: " << line1Basis.first << ", "
      << line1Basis.second << " second basis: "
      << line2Basis.first << ", " << line2Basis.second
      << endl;

    */
    //cout << " Theta calculated via dot products is " << theta << endl;
    angleBetweenLines = theta;


    // We draw a line between the endpoints of our lines forming the corner,
    // and another
    const point<int> intersection = Utility::
        getIntersection(line1End, line2End,
                        cornerLocation,
                        dogLocation);
    // Lines are parallel..
    if ((intersection.x == NO_INTERSECTION) &&
        (intersection.y == NO_INTERSECTION))
        return OUTER_L;

    int otherLineMinX = min(line1End.x, line2End.x);
    int otherLineMinY = min(line1End.y, line2End.y);
    int otherLineMaxX = max(line1End.x, line2End.x);
    int otherLineMaxY = max(line1End.y, line2End.y);

    int dogCornerLineMinX = min(getX(), dogLocation.x);
    int dogCornerLineMinY = min(getY(), dogLocation.y);
    int dogCornerLineMaxX = max(getX(), dogLocation.x);
    int dogCornerLineMaxY = max(getY(), dogLocation.y);


    bool onBothLines = (intersection.x >= max(otherLineMinX, dogCornerLineMinX)
                        &&
                        intersection.x <= min(otherLineMaxX, dogCornerLineMaxX)
                        &&
                        intersection.y >= max(otherLineMinY, dogCornerLineMinY)
                        &&
                        intersection.y <= min(otherLineMaxY, dogCornerLineMaxY)
        );
    /*
    // We know we have a crossing somewhere.
    const float cornerToDogLineLength = Utility::getLength(dogLocation, cornerLocation);
    const float tOnCornerToDogLine =
    Utility::findLinePointDistanceFromStart(intersection,
    dogLocation,
    cornerLocation,
    cornerToDogLineLength);

    const float otherLineLength = Utility::getLength(line1End, line2End);

    const float tOnOtherLine =
    Utility::findLinePointDistanceFromStart(intersection,
    line1End,
    line2End,
    otherLineLength);

    const bool intersectionLiesOnCornerToDogLine =
    tOnCornerToDogLine > 0 && tOnCornerToDogLine < cornerToDogLineLength;
    const bool intersectionLiesOnOtherLine =
    tOnOtherLine > 0 && tOnOtherLine < otherLineLength;

    // See 115.FRM.  Add uncertainty if angle between cornerToDog and
    // one of corner.line1 or corner.line2 is very small



    if (onBothLines != (intersectionLiesOnCornerToDogLine &&
    intersectionLiesOnOtherLine)) {
    cout << onBothLines << intersectionLiesOnCornerToDogLine
    << intersectionLiesOnOtherLine << endl;
    cout << "You screwed up nick." << endl;
    }

    */
    //if (intersectionLiesOnCornerToDogLine && intersectionLiesOnOtherLine) {
    if (onBothLines) {
        return INNER_L;
    } else {
        return OUTER_L;
    }
}

/**
 * Calculate and set the standard deviation for the distance measurement.
 * Set the distance measurement.
 *
 * @param _distance the distance estimate to be set
 */
void VisualCorner::setDistanceWithSD(float _distance)
{
    setDistance(_distance);
    setDistanceSD(cornerDistanceToSD(_distance));
}

/**
 * Calculate and set the standard deviation for the bearing measurement.
 * Set the bearing measurement.
 *
 * @param _bearing the bearing estimate to be set
 */
void VisualCorner::setBearingWithSD(float _bearing)
{
    setBearing(_bearing);
    setBearingSD(cornerBearingToSD(_bearing));
}
