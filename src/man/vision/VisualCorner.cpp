#include "VisualCorner.h"
#include <math.h>
using namespace std;
using namespace boost;

//-------------------------------
// Static VisualCorner variables:
const point <int> VisualCorner::
dogLocation(IMAGE_WIDTH/2, IMAGE_HEIGHT - 1);

const float VisualCorner::MIN_EXTEND_DIST = 12.0f;

VisualCorner::VisualCorner(const int _x, const int _y,
                           const float _distance,
                           const float _bearing,
                           shared_ptr<VisualLine> l1, shared_ptr<VisualLine> l2,
                           const float _t1, const float _t2)
    : VisualDetection(_x, _y, _distance, _bearing),
      VisualLandmark<cornerID>(CORNER_NO_IDEA_ID),
      possibleCorners(ConcreteCorner::concreteCorners().begin(),
                      ConcreteCorner::concreteCorners().end()),
      cornerType(UNKNOWN), line1(l1), line2(l2),
      lines(), t1(_t1), t2(_t2),
      // Technically the initialization of tBar and tStem is incorrect here for
      // which we apologize. It's a hack, but the true values of tBar and tStem
      // will get assigned in determineCornerShape which is right here in the
      // constructor.
      tBar(line1), tStem(line2),
      angleBetweenLines(0)
{
    lines.push_back(line1);
    lines.push_back(line2);
    determineCornerShape();

    // Calculate and set the standard deviation of the measurements
    setDistanceSD(cornerDistanceToSD(_distance));
    setBearingSD(cornerBearingToSD(_bearing));
}

VisualCorner::~VisualCorner() {}

VisualCorner::VisualCorner(const VisualCorner& other)
    : VisualDetection(other), VisualLandmark<cornerID>(other),
      possibleCorners(other.possibleCorners),
      cornerType(other.cornerType),
      line1(other.line1), line2(other.line2), lines(other.lines),
      t1(other.t1), t2(other.t2),
      tBar(other.tBar), tStem(other.tStem),
      angleBetweenLines(other.angleBetweenLines)
{
    secondaryShape = UNKNOWN;
}

/* This method will assign a value to the variable cornerType. It assumes
 * that the corner is not a cross corner. If it were it wouldn't have been
 * constructed in FieldLines::interesctLines()
 */
void VisualCorner::determineCornerShape() {
    if (Utility::tValueInMiddleOfLine(t1, line1->getLength(),
                                      max(line2->getAvgWidth(),
                                          MIN_EXTEND_DIST))) {
        cornerType = T;
        tBar = line1;
        tStem = line2;
        setID(T_CORNER);
    } else if(Utility::tValueInMiddleOfLine(t2, line2->getLength(),
                                            max(line1->getAvgWidth(),
                                                MIN_EXTEND_DIST))) {
        cornerType = T;
        tBar = line2;
        tStem = line1;
        setID(T_CORNER);
    } else {
        // Temporary side effect - set angleBetweenLines
        cornerType = getLClassification();
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


    // We draw a line between the endpoints of our lines forming the corner,
    // and another
    const point<int> intersection = Utility::
        getIntersection(line1End, line2End,
                        cornerLocation,
                        dogLocation);
    // Lines are parallel..
    if ((intersection.x == Utility::NO_INTERSECTION) &&
        (intersection.y == Utility::NO_INTERSECTION))
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
    const ConcreteLine* concreteLine = line->getPossibleLines().front();

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
const point<int> VisualCorner::getTStemEndpoint() const
{
    return Utility::getPointFartherFromCorner(*getTStem(), getX(), getY());
}

/**
 * Returns true when the endpoint is below the corner on the screen.
 */
const bool VisualCorner::doesTPointDown() const
{
    point<int> endpoint = getTStemEndpoint();
    return endpoint.y > getY();
}

/**
 * Returns true when the endpoint is below the corner on the screen.
 */
const bool VisualCorner::doesTPointRight() const
{
    point<int> endpoint = getTStemEndpoint();
    return endpoint.x > getX();
}

/**
 * Returns true when the endpoint is below the corner on the screen.
 */
const bool VisualCorner::doesTPointLeft() const
{
    point<int> endpoint = getTStemEndpoint();
    return endpoint.x < getX();
}
