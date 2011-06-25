#include "VisualCorner.h"

#include <math.h>

#include "NaoPose.h"


using namespace std;
using namespace boost;

//-------------------------------
// Static VisualCorner variables:
const point <int> VisualCorner::
naoLocation(IMAGE_WIDTH/2, IMAGE_HEIGHT - 1);

const float VisualCorner::MIN_EXTEND_DIST = 12.0f;

VisualCorner::VisualCorner(const int _x, const int _y,
                           const float _distance,
                           const float _bearing,
                           shared_ptr<VisualLine> l1, shared_ptr<VisualLine> l2,
                           const float _t1, const float _t2,
                           shared_ptr<NaoPose> _pose)
    : VisualObject(CORNER_NO_IDEA_ID,_x, _y, _distance, _bearing),
      pose(_pose),
      possibleCorners(ConcreteCorner::concreteCorners().begin(),
                      ConcreteCorner::concreteCorners().end()),
      cornerType(UNKNOWN), secondaryShape(UNKNOWN), line1(l1), line2(l2),
      lines(), t1(_t1), t2(_t2),
      // Technically the initialization of tBar and tStem is incorrect here for
      // which we apologize. It's a hack, but the true values of tBar and tStem
      // will get assigned in determineCornerShape which is right here in the
      // constructor.
      tBar(line1), tStem(line2),
      angleBetweenLines(0), orientation(0), physicalOrientation(0)
{
    lines.push_back(line1);
    lines.push_back(line2);
    determineCornerShape();

    // Calculate and set the standard deviation of the measurements
    setDistanceSD(cornerDistanceToSD(_distance));
    setBearingSD(cornerBearingToSD(_bearing));
	setAngleX( static_cast<float>(HALF_IMAGE_WIDTH - _x) /
			   static_cast<float>(HALF_IMAGE_WIDTH) *
			   MAX_BEARING_RAD);
	setAngleY(static_cast<float>(HALF_IMAGE_HEIGHT - _y) /
			  static_cast<float>(HALF_IMAGE_HEIGHT) *
			  MAX_ELEVATION_RAD);
}

VisualCorner::~VisualCorner() {}

VisualCorner::VisualCorner(const VisualCorner& other)
    : VisualObject(other),
      pose(other.pose),
      possibleCorners(other.possibleCorners),
      cornerType(other.cornerType), secondaryShape(other.secondaryShape),
      line1(other.line1), line2(other.line2), lines(other.lines),
      t1(other.t1), t2(other.t2),
      tBar(other.tBar), tStem(other.tStem),
      angleBetweenLines(other.angleBetweenLines),
      orientation(other.orientation),
      physicalOrientation(other.physicalOrientation)
{
}

/*
 */
void VisualCorner::changeToT(boost::shared_ptr<VisualLine> stem) {
	cornerType = T;
	tStem = stem;
	if (line1 == stem) {
		tBar = line2;
	} else {
		tBar = line1;
	}
	setID(T_CORNER);
	setTOrientation();
}

/* This method will assign a value to the variable cornerType. It assumes
 * that the corner is not a cross corner. If it were it wouldn't have been
 * constructed in FieldLines::interesctLines()
 */
void VisualCorner::determineCornerShape() {

    // Decide the T stem and bar lines
    if (Utility::tValueInMiddleOfLine(t1, line1->getLength(),
                                      max(line2->getAvgWidth(),
                                          MIN_EXTEND_DIST))) {
        cornerType = T;
        tBar = line1;
        tStem = line2;
    } else if(Utility::tValueInMiddleOfLine(t2, line2->getLength(),
                                            max(line1->getAvgWidth(),
                                                MIN_EXTEND_DIST))) {
        cornerType = T;
        tBar = line2;
        tStem = line1;
    } else {          // It's a L Corner
        // Temporary side effect - set angleBetweenLines
        cornerType = getLClassification();
        physicalOrientation = getLPhysicalOrientation();
    }

    // Set T Attributes
    if (cornerType == T){
        setTOrientation();
        physicalOrientation = getTPhysicalOrientation();
    }

    secondaryShape = UNKNOWN;
    determineCornerIDFromShape();
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
        VisualLine::getLineComponents(*line1);
    pair <int, int> line2Basis =
        VisualLine::getLineComponents(*line2);

    // Find the line endpoint that's farther from the corner (allows us
    // to direct our line segment away from the corner)
    point <int> line1End, line2End;

    const point<int> corner(cornerX, cornerY);

    const point<int> end1 = line1->getEndpoint();
    const point<int> start1 = line1->getStartpoint();
    // corner is closer to start point of line 1
    if (Utility::getLength(corner, start1) < Utility::getLength(corner, end1)) {
        line1End = line1->getEndpoint();
    }
    // Closer to end
    else {
        line1End = line1->getStartpoint();
        // Swap the signs on the line 1 basis
        line1Basis.first *= -1;
        line1Basis.second *= -1;
    }

    const point<int> start2 = line2->getStartpoint();
    const point<int> end2 = line2->getEndpoint();
    // corner is closer to start point of line 2
    if (Utility::getLength(corner, start2) < Utility::getLength(corner, end2)) {
        line2End = line2->getEndpoint();
    }
    // Closer to end
    else {
        line2End = line2->getStartpoint();
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
    float theta = TO_DEG * NBMath::safe_acos(dotProduct/
                                             (line1->getLength() * line2->getLength()));
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

    // in theory we should be able to get the angle's orientation
    // step 1:  normalize the vectors
    // step 2:  average them to get a new vector between them
    // step 3:  take the angle between that vector and the one pointing "north"
    // This is all greatly simplified by two things: 1) we don't care about
    // magnitude, and 2) using a "north" vector simplifies the equations

    // we have the length of the lines already
    // step 1:  we don't have to "normalize" in the traditional sense, just
    // get equivalent length vectors
    float normalize = line1->getLength() / line2->getLength();
    pair <float, float> line3Basis;
    // we don't need to average since we don't care about the magnitude of the vector
    line3Basis.first = static_cast<float>(line1Basis.first) +
        normalize * static_cast<float>(line2Basis.first);
    line3Basis.second = static_cast<float>(line1Basis.second) +
        normalize * static_cast<float>(line2Basis.second);
    orientation = TO_DEG * atan2(line3Basis.first, -line3Basis.second);

    // We draw a line between the endpoints of our lines forming the corner,
    // and another
    const point<int> intersection = Utility::
        getIntersection(line1End, line2End,
                        cornerLocation,
                        naoLocation);
    // Lines are parallel..
    if ((intersection.x == Utility::NO_INTERSECTION) &&
        (intersection.y == Utility::NO_INTERSECTION))
        return OUTER_L;

    int otherLineMinX = min(line1End.x, line2End.x);
    int otherLineMinY = min(line1End.y, line2End.y);
    int otherLineMaxX = max(line1End.x, line2End.x);
    int otherLineMaxY = max(line1End.y, line2End.y);

    int naoCornerLineMinX = min(getX(), naoLocation.x);
    int naoCornerLineMinY = min(getY(), naoLocation.y);
    int naoCornerLineMaxX = max(getX(), naoLocation.x);
    int naoCornerLineMaxY = max(getY(), naoLocation.y);
    /*cout << "Inters " << intersection.x << " " << intersection.y << " " <<
        otherLineMinX << " " << naoCornerLineMinX << " " <<
        otherLineMaxX << " " << naoCornerLineMaxX << " " <<
        otherLineMinY << " " << naoCornerLineMinY << " " <<
        otherLineMaxY << " " << naoCornerLineMaxY << endl; */


    bool onBothLines = (intersection.x >= max(otherLineMinX, naoCornerLineMinX)
                        &&
                        intersection.x <= min(otherLineMaxX, naoCornerLineMaxX)
                        &&
                        intersection.y >= max(otherLineMinY, naoCornerLineMinY)
                        &&
                        intersection.y <= min(otherLineMaxY, naoCornerLineMaxY)
        );
    /*
    // We know we have a crossing somewhere.
    const float cornerToNaoLineLength = Utility::getLength(naoLocation, cornerLocation);
    const float tOnCornerToNaoLine =
    Utility::findLinePointDistanceFromStart(intersection,
    naoLocation,
    cornerLocation,
    cornerToNaoLineLength);

    const float otherLineLength = Utility::getLength(line1End, line2End);

    const float tOnOtherLine =
    Utility::findLinePointDistanceFromStart(intersection,
    line1End,
    line2End,
    otherLineLength);

    const bool intersectionLiesOnCornerToNaoLine =
    tOnCornerToNaoLine > 0 && tOnCornerToNaoLine < cornerToNaoLineLength;
    const bool intersectionLiesOnOtherLine =
    tOnOtherLine > 0 && tOnOtherLine < otherLineLength;

    // See 115.FRM.  Add uncertainty if angle between cornerToNao and
    // one of corner.line1 or corner.line2 is very small



    if (onBothLines != (intersectionLiesOnCornerToNaoLine &&
    intersectionLiesOnOtherLine)) {
    cout << onBothLines << intersectionLiesOnCornerToNaoLine
    << intersectionLiesOnOtherLine << endl;
    cout << "You screwed up nick." << endl;
    }

    */
    //if (intersectionLiesOnCornerToNaoLine && intersectionLiesOnOtherLine) {
    if (onBothLines) {
        return INNER_L;
    } else {
        return OUTER_L;
    }
}

/**
 * See documentation for VisualCorner::physicalOrientation
 * variable. Determines the value for L corners.
 */
float VisualCorner::getLPhysicalOrientation()
{
    // Get line endpoints
    point<int> line1End = Utility::getPointFartherFromCorner(*line1, x, y);
    point<int> line2End = Utility::getPointFartherFromCorner(*line2, x, y);

    // Find which line is on the left:
    // Is line2End to the left of the line intersect->line1End
    point<int> leftEnd = Utility::left(point<int>(x,y),
                                       line1End,
                                       line2End) ? line2End : line1End;

    return getLinePhysicalOrientation(leftEnd);
}

float VisualCorner::getTPhysicalOrientation()
{
    point<int> stemEnd = getTStemEndpoint();
    return getLinePhysicalOrientation(stemEnd);
}

/**
 * @return Orientation of line on the ground from 0-2pi radians (right
 *         hand rule)
 */
float VisualCorner::getLinePhysicalOrientation(point<int> end)
{
    estimate endPt = pose->pixEstimate(end.x, end.y, 0);

    // Use angles to determine orientation
    //
    // Make the coordinate frame point at the corner by subtracting
    // the bearings. This makes the operation independent of the
    // robot's direction.
    float x = distance - endPt.dist * cosf(endPt.bearing - bearing);
    float y = - endPt.dist * sinf(endPt.bearing - bearing);

    float legLength = hypotf(x,y);

    // Adjacent over hypotenuse
    // "That's just the definition of cosine" - Yoni
    float orientationCos = x/legLength;

    // -sign(j) gives us the signed orientation depending on direction
    // of vector
    return -copysignf(1,y) * acosf(orientationCos);
}

/**
 * Use the ID and shape of the corner to help narrow down IDs for
 * lines within the corner.
 */
void VisualCorner::identifyLinesInCorner()
{
    // Check lines in positively identified corners
    if (hasPositiveID()) {
        const ConcreteCorner * corner = possibleCorners.front();
        if (cornerType == T) {
            tBar->setPossibleLines(corner->getTBar());
            tStem->setPossibleLines(corner->getTStem());
        } else {
            // The best we can do is say that we know it's one of
            // the two lines in this corner
            line1->setPossibleLines(corner->getLines());
            line2->setPossibleLines(corner->getLines());
        }
        return;
    } else if (possibleCorners.size() < ConcreteCorner::NUM_CORNERS){
        list<const ConcreteLine*> possibilites;
        list<const ConcreteCorner*>::const_iterator c;
        // for (c = possibleCorners.begin();
        //   c != possibleCorners.end();
        //   ++c){
        //  possibilites.insert(possibilites.end(),
        //                      (*c)->getLines().begin(),
        //                      (*c)->getLines().end());

        // }
        // line1->setPossibleLines(possibilites);
        // line2->setPossibleLines(possibilites);
    }

    // Use the shape of the corner to identify the lines
    if (cornerType == T){
        tBar->setPossibleLines(ConcreteLine::tBarLines());
        tStem->setPossibleLines(ConcreteLine::tStemLines());
    } else if (cornerType == INNER_L || cornerType == OUTER_L) {
        line1->setPossibleLines(ConcreteLine::lCornerLines());
        line2->setPossibleLines(ConcreteLine::lCornerLines());
    }
}

// See if any corners have a line that was positively identified,
// and use that to limit the number of possible corners that it be
void VisualCorner::identifyFromLines()
{
    if (line1->hasPositiveID())
        IDFromLine(line1);
    if (line2->hasPositiveID())
        IDFromLine(line2);
}

/**
 * Uses a positively identified line to reduce the
 * number of possible corners
 *
 * @param line A line with only one possible ID
 */
void VisualCorner::IDFromLine(const shared_ptr<VisualLine> line)
{
    // We don't want to use an ambiguous line, or
    // change the ID an a corner that has already been identified
    if (!line->hasPositiveID() || hasPositiveID())
        return;
    const ConcreteLine* concreteLine = line->getPossibilities().front();

    const vector <const ConcreteCorner*> concretes =
        ConcreteCorner::concreteCorners();

    list<const ConcreteCorner*> possibles;
    vector<const ConcreteCorner*>::const_iterator i = concretes.begin();
    for ( ; i != concretes.end() ; ++i){
        if ((*i)->isLineInCorner(concreteLine))
            possibles.push_back(*i);
    }
    setPossibleCorners(possibles);
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

/**
 * Set the corner ID dependent on the corner shape
 */
void VisualCorner::determineCornerIDFromShape()
{
    switch(cornerType) {
    case INNER_L:
        setID(L_INNER_CORNER);
        break;
    case OUTER_L:
        setID(L_OUTER_CORNER);
        break;
    case T:
        setID(T_CORNER);
        break;
    case CIRCLE:
        setID(CENTER_CIRCLE);
        break;
    case UNKNOWN:
        setID(CORNER_NO_IDEA_ID);
        break;
    default:
        setID(CORNER_NO_IDEA_ID);
        break;
    }

}

const bool VisualCorner::hasPositiveID()
{
    return possibleCorners.size() == 1;
}


/* The secondary shape represents a refining of what the corner
   might be.  Since there are several possible steps in this process
   we do not want to go backwards at any step (e.g. we already know
   what corner it is, don't try and classify it again)
 */
void VisualCorner::setSecondaryShape(const shape s) {
    if (secondaryShape == UNKNOWN || s > secondaryShape) {
        secondaryShape = s;
    }
}

/*
 * Once we have identified a corner as a T we can set its orientation
 * according to the line that forms the stem.
 */
void VisualCorner::setTOrientation() {
    point<int> end = getTStemEndpoint();
    orientation = TO_DEG * static_cast<float>(atan2(end.x - getX(),
                                                    getY() - end.y));
}

void VisualCorner::setPossibleCorners(
    std::list <const ConcreteCorner *> _possibleCorners)
{
    list<const ConcreteCorner*> updated(0);

    for (list<const ConcreteCorner*>::const_iterator
             currCorner = possibleCorners.begin();
         currCorner != possibleCorners.end(); currCorner++) {

        for ( list<const ConcreteCorner*>::iterator
                  newCorner = _possibleCorners.begin();
              newCorner != _possibleCorners.end() ; ) {

            // If the line is in both sets, then it's still a
            // possible corner
            if (**newCorner == **currCorner) {
                updated.push_back(*newCorner);
                newCorner = _possibleCorners.erase(newCorner);
                break;
            } else {
                // Increment the iterator if we don't erase a corner
                newCorner++;
            }
        }
    }
    if (updated.size() > 0)
        possibleCorners = updated;
}

/**
 * Another way of setting the possible corners
 *
 * @TODO Unify setPossibleCorners so we don't copy the vector.
 */
void VisualCorner::
setPossibleCorners( vector <const ConcreteCorner*> _possibleCorners)
{
    list<const ConcreteCorner*> updated(0);

    for (list<const ConcreteCorner*>::const_iterator
             currCorner = possibleCorners.begin();
         currCorner != possibleCorners.end(); currCorner++) {

        for ( vector<const ConcreteCorner*>::iterator
                  newCorner = _possibleCorners.begin();
              newCorner != _possibleCorners.end() ; ) {

            // If the line is in both sets, then it's still a
            // possible corner
            if (**newCorner == **currCorner) {
                updated.push_back(*newCorner);
                newCorner = _possibleCorners.erase(newCorner);
                break;
            } else {
                // Increment the iterator if we don't erase a corner
                newCorner++;
            }
        }
    }
    if (updated.size() > 0)
        possibleCorners = updated;
}

/**
 * Returns the endpoint of the TStem which is farther from
 * the corner. So the base of the stem, basically.
 */
point<int> VisualCorner::getTStemEndpoint() const
{
    return Utility::getPointFartherFromCorner(*getTStem(), getX(), getY());
}

/**
 * Returns true when the endpoint is below the corner on the screen.
 */
bool VisualCorner::doesItPointDown()
{
    return abs(orientation) > 90.0;
}

bool VisualCorner::doesItPointUp()
{
    return abs(orientation) < 90.0;
}

/**
 * Returns true when the endpoint is below the corner on the screen.
 */
bool VisualCorner::doesItPointRight()
{
    return orientation >= 0.0;
}

/**
 * Returns true when the endpoint is below the corner on the screen.
 */
bool VisualCorner::doesItPointLeft()
{
    return orientation < 0.0;
}

const std::vector<cornerID> VisualCorner::getIDs() {
  std::vector<cornerID> poss;

  for (list<const ConcreteCorner*>::const_iterator
	 currCorner = possibleCorners.begin();
       currCorner != possibleCorners.end(); currCorner++) {
    poss.push_back((**currCorner).getID());
  }

  return poss;
}
