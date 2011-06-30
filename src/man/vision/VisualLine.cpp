#include "VisualLine.h"

#include <algorithm>    // for sort()
#include <cstdlib>      // git rid of overloaded abs() errors
#include "NBMath.h"
#include "ActiveArray.h"

// #define DEBUG_VISUAL_LINE

using namespace std;

// Return true if y is higher in the image (towards 0)
const bool YOrder::operator() (const linePoint& first, const linePoint& second)
 const
{
    return first.y < second.y;
}


VisualLine::VisualLine(const HoughLine& a,
                       const HoughLine& b,
                       const Gradient& g) :
    VisualLandmark<lineID>(UNKNOWN_LINE), hLine1(a), hLine2(b)
{
    findEndpoints(a,b,g);
    // findDimensions();
    // find3DCoords();
}

/**
 * Find the endpoints of the line from the gradient
 */
void VisualLine::findEndpoints(const HoughLine& a,
                               const HoughLine& b,
                               const Gradient& g)
{
    PointsPair aEnds = findHoughLineEndpoints(a,g);
    PointsPair bEnds = findHoughLineEndpoints(b,g);

    setDirectionalEndpoints(aEnds, bEnds);
    trimEndpoints();
}

VisualLine::PointsPair
VisualLine::findHoughLineEndpoints(const HoughLine& l,
                                   const Gradient& g)
{
    const float sn = l.getSinT();
    const float cs = l.getCosT();

    const float x0 = l.getRadius() * cs;
    const float y0 = l.getRadius() * sn;

    // vector along line is (sin, -cos)
    float uMean = 0, uMeanSq = 0;
    int numPts = 0;

    // Scan through the gradient's edges
    for (int i = 0; g.isPeak(i); ++i) {
        // Find points which lie on the HoughLine
        // Add them to the ActiveArray
        if ( l.isOnLine(g.angles[i]) ){

            float u = (sn * (static_cast<float>(g.angles[i].x) - x0) +
                       -cs * (static_cast<float>(g.angles[i].y) - y0));

            uMean   += u;
            uMeanSq += u*u;
            numPts++;
        }
    }

    const float numPtsF = static_cast<float>(numPts);
    uMean   /= numPtsF;
    uMeanSq /= numPtsF;

    float uVar = uMeanSq - uMean*uMean;
    float uSd = sqrtf(uVar);

    point<int> meanPt(static_cast<int>(uMean *  sn + x0) + IMAGE_WIDTH/2,
                      static_cast<int>(uMean * -cs + y0) + IMAGE_HEIGHT/2);

    float xOff = sn * uSd * 1.5;
    float yOff = -cs * uSd * 1.5;

    // Package and return endpoints
    point<int> endA(meanPt.x + xOff,
                    meanPt.y + yOff);

    point<int> endB(meanPt.x - xOff,
                    meanPt.y - yOff);

    return PointsPair(endA, endB);
}

/**
 * Assign the various endpoints of the visual line
 */
void VisualLine::setDirectionalEndpoints(const PointsPair& a,
                                         const PointsPair& b )
{
    const PointsPair* top, *bottom;

    // Y is inverted (top is actually the lesser of the two for human
    // viewing purposes)
    if (a.first.y < b.first.y){
        top    = &a;
        bottom = &b;
    } else {
        top    = &b;
        bottom = &a;
    }

    if (top->first.x < top->second.x){
        tl = top->first;
        tr = top->second;
    } else {
        tl = top->second;
        tr = top->first;
    }

    if (bottom->first.x < bottom->second.x){
        bl = bottom->first;
        br = bottom->second;
    } else {
        bl = bottom->second;
        br = bottom->first;
    }
}

/**
 * Trim the two lines outlining the VisualLine to be of roughly the same length
 */
void VisualLine::trimEndpoints()
{
    // We find the closest point on the bottom line to the top right
    // point. If that point is between the bottom endpoints, it means
    // the bottom line is longer and should be truncated. We truncate
    // it to the intersection point.


    // Truncate the right side
    point<int> perp = Utility::getClosestPointOnLine(tr, bl, br);
    if (Utility::between(perp, bl, br)){
        br = perp;
    } else {
        perp = Utility::getClosestPointOnLine(br, tl, tr);

        if (!Utility::between(perp, tl, tr)){

#ifdef DEBUG_VISUAL_LINE
            cout << "The sides of this line don't overlap. Throwing away."
                 << endl;
#endif
                // Set line to bad.

                return;
        }

        tr = perp;
    }

    // Truncate the left side
    perp = Utility::getClosestPointOnLine(tl, bl, br);
    if (Utility::between(perp, bl, br)){
        bl = perp;
    } else {
        tl = Utility::getClosestPointOnLine(bl, tl, tr);
    }
}

/**
 * Find various line dimensions:
 *                            - Angle
 */
void VisualLine::findDimensions()
{

}

/**
 * Calculate the position of the line relative to the robot rather
 * than just in the image frame
 */
void VisualLine::find3DCoords()
{

}

bool VisualLine::isLineEdge(const HoughLine& line,
                            const Gradient& g,
                            double x0, double y0,
                            int& _x, int& _y)
{
    // Look 3 points on either side of line for an edge in the same
    // direction
    for(double u = -edge_pt_buffer; u <= edge_pt_buffer; u += 1.0){

        int x = static_cast<int>(line.getCosT() * u + x0);
        int y = static_cast<int>(line.getSinT() * u + y0);

        if (y < 0 || x < 0 || y > IMAGE_WIDTH || x > IMAGE_WIDTH){
            continue;
        }

        if (g.peaks[y][x] != -1){
            int tDiff = abs(((g.peaks[y][x] - line.getTIndex()) & 0xff)
                            << 24 >> 24);
            if(tDiff < angle_epsilon){
                _x = x;
                _y = y;
                return true;
            }
        }
    }
    return false;
}

VisualLine::VisualLine(list<list<linePoint>::iterator> &nodes)
    : VisualLandmark<lineID>(UNKNOWN_LINE),ccLine(false),
      possibleLines(ConcreteLine::concreteLines().begin(),
                    ConcreteLine::concreteLines().end())
{
    for (list<list<linePoint>::iterator>::iterator i = nodes.begin();
         i != nodes.end(); i++) {
        // We need to dereference twice to get to the actual linePoint object.
        points.push_back(**i);
    }
    init();
}

VisualLine::VisualLine() : VisualLandmark<lineID>(UNKNOWN_LINE),ccLine(false),
      possibleLines(ConcreteLine::concreteLines().begin(),
                    ConcreteLine::concreteLines().end())
{

}

VisualLine::VisualLine(float _dist, float _bearing) :
    VisualLandmark<lineID>(UNKNOWN_LINE),ccLine(false),
      possibleLines(ConcreteLine::concreteLines().begin(),
                    ConcreteLine::concreteLines().end())
{
    setDistanceWithSD(_dist);
    setBearingWithSD(_bearing);
}





VisualLine::VisualLine(list<linePoint> &linePoints)
    : VisualLandmark<lineID>(UNKNOWN_LINE),ccLine(false),
      possibleLines(ConcreteLine::concreteLines().begin(),
                    ConcreteLine::concreteLines().end())
{
    for (list<linePoint>::iterator i = linePoints.begin();
         i != linePoints.end(); i++) {
        // We need to dereference twice to get to the actual linePoint object.
        points.push_back(*i);
    }
    init();
}


VisualLine::VisualLine(const VisualLine& other)
    : VisualLandmark<lineID>(other),
      start(other.start), end(other.end), leftBound(other.leftBound),
      rightBound(other.rightBound),
      bottomBound(other.bottomBound),
      topBound(other.topBound),
      points(other.points),
      angle(other.angle), length(other.length),
      a(other.a), b(other.b),
      color(other.color), colorStr(other.colorStr),
      avgVerticalWidth(other.avgVerticalWidth),
      avgHorizontalWidth(other.avgHorizontalWidth),
      thinnestHorPoint(other.getThinnestHorizontalPoint()),
      thickestHorPoint(other.getThickestHorizontalPoint()),
      thinnestVertPoint(other.getThinnestVerticalPoint()),
      thickestVertPoint(other.getThickestVerticalPoint()),
      distance(other.getDistance()), bearing(other.getBearing()),
      distanceSD(other.getDistanceSD()), bearingSD(other.getBearingSD()),
      ccLine(other.getCCLine()),
      possibleLines(other.getPossibleLines()),
      tr(other.tr), tl(other.tl), br(other.br), bl(other.bl)
{
}

VisualLine::~VisualLine() {}

const linePoint VisualLine::DUMMY_LINEPOINT(-1,-1,-1,-1,VERTICAL);


void VisualLine::addPoints(const list <linePoint> &additionalPoints)
{
    for (list<linePoint>::const_iterator i = additionalPoints.begin();
         i != additionalPoints.end(); i++) {
        points.push_back(*i);
    }
    sort(points.begin(), points.end());
    init();
}

// Do not use often, forces us to re-sort all the points!
void VisualLine::addPoint(const linePoint& point)
{
    points.push_back(point);
    sort(points.begin(), points.end());
    init();
}

void VisualLine::addPoints(const vector <linePoint> &additionalPoints)
{
    for (vector<linePoint>::const_iterator i = additionalPoints.begin();
         i != additionalPoints.end(); i++) {
        points.push_back(*i);
    }

    sort(points.begin(), points.end());
    init();
}

/**
 * Computes and sets the basic parameters of a VisualLine from its linePoints
 */
void VisualLine::init()
{
    parallel = false;
    // Points are sorted by x
    leftBound = points[0].x;
    rightBound = points[points.size()-1].x;

    // Unfortunately we can't get the top and bottom so easily
    topBound = min_element(points.begin(), points.end(), YOrder())->y;
    bottomBound = max_element(points.begin(), points.end(), YOrder())->y;

    pair <float, float> equation = leastSquaresFit(points);
    a = equation.first;
    b = equation.second;

    // We are calling this a vertical line
    if (abs(rightBound - leftBound) < 2) {
        rightBound = leftBound;

        start.x = leftBound;
        end.x = leftBound;
        start.y = bottomBound;
        end.y = topBound;
    }
    // Line is more horizontally oriented, use leftBound and rightBound and calculate the
    // endpoints
    else if (rightBound-leftBound > bottomBound-topBound){
        start.x = leftBound;
        end.x = rightBound;
        start.y = Utility::getLineY(start.x, b, a);
        end.y = Utility::getLineY(end.x, b, a);
        // Since we are basing our end point's y values on the least squares
        // regression values, need to recalculate the top and bottom points
        if (start.y < end.y) {
            topBound = start.y;
            bottomBound = end.y;
        }
        else {
            topBound = end.y;
            bottomBound = start.y;
        }
    }
    // Line is more vertically oriented, use top and bottom to calculate the
    // other endpoints
    else {
        start.y = bottomBound;
        end.y = topBound;
        start.x = Utility::getLineX(start.y, b, a);
        end.x = Utility::getLineX(end.y, b, a);
        if (start.x < end.x) {
            leftBound = start.x;
            rightBound = end.x;
        }
        else {
            leftBound = end.x;
            rightBound = start.x;
        }
    }

    angle = calculateAngle();
    length = calculateLength();

    calculateWidths();
}

// Loops through all of our line points and calculates the average widths in
// the x and y direction, and assigns these widths to the avgVerticalWidth
// and avgHorizontalWidth fields.  Also assigns the min and max for the widths
void VisualLine::calculateWidths()
{
    float verticalWidthTotal = 0;
    float horizontalWidthTotal = 0;
    int numVertPoints = 0;
    int numHorPoints = 0;

    static const float INITIAL_WIDTH = 0.0;

    float minVertWidth = INITIAL_WIDTH;
    float maxVertWidth = INITIAL_WIDTH;
    float minHorWidth = INITIAL_WIDTH;
    float maxHorWidth = INITIAL_WIDTH;


    for (vector<linePoint>::const_iterator i = points.begin();
         i != points.end(); i++) {

        if (i->foundWithScan == VERTICAL) {
            numVertPoints++;
            verticalWidthTotal += i->lineWidth;
            // See if the line point width is min or max

            if (minVertWidth == INITIAL_WIDTH || i->lineWidth < minVertWidth) {
                minVertWidth = i->lineWidth;
                thinnestVertPoint = *i;
            }
            if (maxVertWidth == INITIAL_WIDTH || i->lineWidth > maxVertWidth) {
                maxVertWidth = i->lineWidth;
                thickestVertPoint = *i;
            }
        }
        else {
            numHorPoints++;
            horizontalWidthTotal += i->lineWidth;
            // See if the line point width is min or max

            if (minHorWidth == INITIAL_WIDTH || i->lineWidth < minHorWidth) {
                minHorWidth = i->lineWidth;
                thinnestHorPoint = *i;
            }
            if (maxHorWidth == INITIAL_WIDTH || i->lineWidth > maxHorWidth) {
                maxHorWidth = i->lineWidth;
                thickestHorPoint = *i;
            }
        }
    }
    if (numVertPoints > 0) {
        avgVerticalWidth = verticalWidthTotal /
            static_cast<float>(numVertPoints);
    }
    else {
        avgVerticalWidth = 0;
    }
    if (numHorPoints > 0) {
        avgHorizontalWidth = horizontalWidthTotal /
            static_cast<float>(numHorPoints);
    }
    else {
        avgHorizontalWidth = 0;
    }
}

/**
 * Set the color value of the line. Also sets the string value of the color.
 */
void VisualLine::setColor(const int c)
{
    color = c;
    setColorString(Utility::getColorString(c));
}

/**
 * Calculate the length of the line on screen from its endpoints.
 */
const float VisualLine::calculateLength() const
{
    return Utility::getLength( static_cast<float>(start.x),
                               static_cast<float>(start.y),
                               static_cast<float>(end.x),
                               static_cast<float>(end.y) );
}

/**
 * Calculate the angle from horizontal (in degrees) the line makes on the screen
 */
const float VisualLine::calculateAngle() const
{
    return Utility::getAngle(start.x, start.y,
                             end.x, end.y);
}

// We define a line to be vertically oriented if the change in y is bigger
// than the change in x
const bool VisualLine::isVerticallyOriented() const
{
    return rightBound - leftBound < bottomBound - topBound;
}

const bool VisualLine::isPerfectlyVertical() const
{
    return rightBound == leftBound;
}

// Given a line, returns (a, b) where the line can be represented by
// ai + bj (i being the unit vector parallel to the x axis, j being
// the unit vector parallel to the y axis).  Assumes that the line
// is oriented left to right.
// Our j axis will actually increase towards the bottom of the screen
pair<int, int> VisualLine::
getLineComponents(const VisualLine &aLine)
{
    int i = aLine.rightBound - aLine.leftBound;
    // If y2 is above y1 in the image, y2 is smaller in image coordinates than
    // y1, so this would be a negative j value (as per our axis oriented towards
    // bottom of screen)
    int j = aLine.end.y - aLine.start.y;
    return pair<int, int>(i,j);
}




// Use least squares to fit the line to the points
// from http://www.efunda.com/math/leastsquares/lstsqr1dcurve.cfm
// y = mx + b
// we need to find m and b
pair<float,float>
VisualLine::leastSquaresFit(const vector<linePoint> &thePoints)
{
    // Use least squares to fit the line to the points
    // from http://www.efunda.com/math/leastsquares/lstsqr1dcurve.cfm
    // y = mx + b
    // we need to find m and b
    float numPoints = static_cast<float>(thePoints.size());
    float ySum = 0.0;
    float xSum = 0.0;
    float xYSum = 0.0;
    float xSquaredSum = 0.0;

    for (vector <linePoint>::const_iterator i = thePoints.begin();
         i != thePoints.end(); i++) {
        xSum += static_cast<float>(i->x);
        ySum += static_cast<float>(i->y);
        xYSum += static_cast<float>(i->x * i->y);
        xSquaredSum += static_cast<float>(i->x * i->x);
    }
    /*
      for (int i = 0; i < numPoints; i++) {
      xSum += thePoints[i].x;
      ySum += thePoints[i].y;
      xYSum += thePoints[i].x * thePoints[i].y;
      xSquaredSum += (thePoints[i].x * thePoints[i].x);
      }*/

    const float b = ((ySum * xSquaredSum) - (xSum * xYSum)) /
        ((numPoints * xSquaredSum) - (xSum * xSum)) ;

    const float m =  ( (numPoints * xYSum) - (xSum * ySum) ) /
        ((numPoints * xSquaredSum) - (xSum * xSum)) ;

    return pair<float, float>(m, b);
}

pair <float, float> VisualLine::leastSquaresFit(const VisualLine& l)
{
    return leastSquaresFit(l.points);
}

/**
 * Calculate and set the standard deviation for the distance measurement.
 * Set the distance measurement.
 *
 * @param _distance the distance estimate to be set
 */
void VisualLine::setDistanceWithSD(float _distance)
{
    setDistance(_distance);
    setDistanceSD(lineDistanceToSD(_distance));
}

/**
 * Calculate and set the standard deviation for the bearing measurement.
 * Set the bearing measurement.
 *
 * @param _bearing the bearing estimate to be set
 */
void VisualLine::setBearingWithSD(float _bearing)
{
    setBearing(_bearing);
    setBearingSD(lineBearingToSD(_bearing));
}

/**
 * Takes the intersection of the current possible line list
 * and the given possibilities to narrow down the choices.
 * The idea is that every new list of possible lines cuts out
 * more impossible lines and shrinks the available set.
 */
void VisualLine::
setPossibleLines( list <const ConcreteLine*> _possibleLines)
{
    if (_possibleLines.empty()){
        return;
    }
    list<const ConcreteLine*> updated(0);

    for (list<const ConcreteLine*>::iterator
             currLine = possibleLines.begin();
         currLine != possibleLines.end(); currLine++) {

        for ( list<const ConcreteLine*>::iterator
                  newLine = _possibleLines.begin();
              newLine != _possibleLines.end(); ) {

            // If the line is in both sets
            if (**newLine == **currLine) {
                updated.push_back(*newLine);
                newLine = _possibleLines.erase(newLine);
            } else {
                // Increment the iterator if we don't erase a line
                newLine++;
            }
        }
    }
    if (!updated.empty()){
        possibleLines = updated;
    }
}

/**
 * Another way of setting the possible lines
 *
 * @TODO Unify setPossibleLines so we don't copy the vector.
 */
void VisualLine::
setPossibleLines( vector <const ConcreteLine*> _possibleLines)
{
    if (_possibleLines.empty()){
        return;
    }

    list<const ConcreteLine*> updated(0);

    for (list<const ConcreteLine*>::iterator
             currLine = possibleLines.begin();
         currLine != possibleLines.end(); currLine++) {

        for ( vector<const ConcreteLine*>::iterator
                  newLine = _possibleLines.begin();
              newLine != _possibleLines.end(); ) {

            // If the line is in both sets
            if (**newLine == **currLine) {
                updated.push_back(*newLine);
                newLine = _possibleLines.erase(newLine);
            } else {
                // Increment the iterator if we don't erase a line
                newLine++;
            }
        }
    }
    if (!updated.empty()){
        possibleLines = updated;
    }
}

const bool VisualLine::hasPositiveID()
{
    return possibleLines.size() == 1;
}

const std::vector<lineID> VisualLine::getIDs() {
  std::vector<lineID> poss;

  for (list<const ConcreteLine*>::const_iterator 
	 currLine = possibleLines.begin();
       currLine != possibleLines.end(); currLine++) {
    poss.push_back((**currLine).getID());
  }

  return poss;
}

