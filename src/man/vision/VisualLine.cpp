
#include <algorithm>    // for sort()
#include <cstdlib>      // git rid of overloaded abs() errors
#include "NBMath.h"

#include "VisualLine.h"
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
    VisualLandmark<lineID>(UNKNOWN_LINE)
{
    findEndpoints(a,b,g);
    findDimensions();
    find3DCoords();
}

/**
 * Find the endpoints of the line from the given HoughLines.
 *
 */
void VisualLine::findEndpoints(const HoughLine& a,
                               const HoughLine& b,
                               const Gradient& g)
{
    // Find the line (of the pair) which is above (in the image) the other one
    const HoughLine& top = (a.getSinT() * a.getRadius() <
                            b.getSinT() * b.getRadius()) ? a : b;
    const HoughLine& bottom = (top == a) ? b : a;

    findLineEdgeEnds(top, g, tr, tl);
    findLineEdgeEnds(bottom, g, br, bl);
}

void VisualLine::findLineEdgeEnds(const HoughLine& line, const Gradient& g,
                                 point<int>& r, point<int>& l)
{
    // Search along line from one end to other keeping track of last
    // runs of edge points. Look for start, then end runs.
    double u1 = 0, u2 = 0;
    HoughLine::findLineImageIntersects(line, u1, u2);

    const double x0 = line.getRadius() * line.getCosT() + IMAGE_WIDTH/2;
    const double y0 = line.getRadius() * line.getSinT() + IMAGE_HEIGHT/2;

    const double cs = -line.getCosT();
    const double sn = line.getSinT();

    int edgeStreak = 0;
    bool foundStart = false, foundEnd = false;

    // Iterate across entire line
    double u;
    int outX, outY;
    for(u=u1; u <= u2; u += 1.0){
        double x = x0 + u * sn;
        double y = y0 + u * cs;

        // Look for first edge points in direction of line
        if (isLineEdge(line, g, x, y, outX, outY)){
            edgeStreak = max(1, edgeStreak+1);

            // Mark first long streak
            if (edgeStreak >= edge_pts_for_line){
                foundEnd = false;

                if (!foundStart){
                    foundStart = true;

                    // Mark it (we'll correct ordering later)
                    // note: we want it to be the first point in the streak
                    r.x = outX; //static_cast<int>((u-2.) * sn + x0);
                    r.y = outY; //static_cast<int>((u-2.) * cs + y0);
                }
            }
        } else {
            // Keep track of negative streaks too
            edgeStreak = min(-1, edgeStreak-1);

            // Look for last edge points in a row
            if (foundStart && !foundEnd &&
                edgeStreak <= -edge_pts_for_line){
                foundEnd = true;

                // Mark end of line segment
                l.x = outX;//static_cast<int>((u-2.) * sn + x0);
                l.y = outY;//static_cast<int>((u-2.) * cs + y0);
            }
        }
    }

    if (!foundEnd){
        // Mark end of line segment
        l.x = outX; // static_cast<int>((u-2.) * sn + x0);
        l.y = outY; static_cast<int>((u-2.) * cs + y0);
    }

    // Put points in the correct order
    if (r.x < l.x){
        swap(r,l);
    }
}

/**
 * Find various line dimensions:
 *                            - Height
 *                            - Width
 *                            - Length
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
      tr(other.tr), tl(other.tl), br(other.br), bl(other.bl),
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
      possibleLines(other.getPossibleLines())
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
