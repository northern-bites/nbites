
#include <algorithm>    // for sort()
#include <cstdlib>      // git rid of overloaded abs() errors

#include "VisualLine.h"
using namespace std;

// Return true if y is higher in the image (towards 0)
const bool YOrder::operator() (const linePoint& first, const linePoint& second)
 const
{
    return first.y < second.y;
}


VisualLine::VisualLine(list<list<linePoint>::iterator> &nodes)
    : possibleLines(ConcreteLine::concreteLines)
{
    for (list<list<linePoint>::iterator>::iterator i = nodes.begin();
         i != nodes.end(); i++) {
        // We need to dereference twice to get to the actual linePoint object.
        points.push_back(**i);
    }
    init();
}

VisualLine::VisualLine() : possibleLines(ConcreteLine::concreteLines)
{

}




VisualLine::VisualLine(list<linePoint> &linePoints)
    : possibleLines(ConcreteLine::concreteLines)
{
    for (list<linePoint>::iterator i = linePoints.begin();
         i != linePoints.end(); i++) {
        // We need to dereference twice to get to the actual linePoint object.
        points.push_back(*i);
    }
    init();
}


VisualLine::VisualLine(const VisualLine& other)
    : start(other.start), end(other.end), left(other.left), right(other.right),
      bottom(other.bottom), top(other.top), points(other.points),
      angle(other.angle), a(other.a), b(other.b), length(other.length),
      color(other.color), colorStr(other.colorStr),
      avgVerticalWidth(other.avgVerticalWidth),
      avgHorizontalWidth(other.avgHorizontalWidth),
      thinnestHorPoint(other.thinnestHorPoint),
      thickestHorPoint(other.thickestHorPoint),
      thinnestVertPoint(other.thinnestVertPoint),
      thickestVertPoint(other.thickestVertPoint),
      distance(other.getDistance()), bearing(other.getBearing()),
      distanceSD(other.getDistanceSD()), bearingSD(other.getBearingSD()),
      possibleLines(ConcreteLine::concreteLines)
{
}

VisualLine::~VisualLine() {

}

const linePoint VisualLine::DUMMY_LINEPOINT(-1,-1,-1,VERTICAL);


void VisualLine::addPoints(const list <linePoint> &additionalPoints) {
    for (list<linePoint>::const_iterator i = additionalPoints.begin();
         i != additionalPoints.end(); i++) {
        points.push_back(*i);
    }
    sort(points.begin(), points.end());
    init();
}

void VisualLine::addPoints(const vector <linePoint> &additionalPoints) {
    for (vector<linePoint>::const_iterator i = additionalPoints.begin();
         i != additionalPoints.end(); i++) {
        points.push_back(*i);
    }

    sort(points.begin(), points.end());
    init();
}


void VisualLine::init() {
    // Points are sorted by x
    left = points[0].x;
    right = points[points.size()-1].x;

    // Unfortunately we can't get the top and bottom so easily
    top = min_element(points.begin(), points.end(), YOrder())->y;
    bottom = max_element(points.begin(), points.end(), YOrder())->y;

    pair <float, float> equation = leastSquaresFit(points);
    a = equation.first;
    b = equation.second;

    // We are calling this a vertical line
    if (abs(right - left) < 2) {
        right = left;

        start.x = left;
        end.x = left;
        start.y = bottom;
        end.y = top;
    }
    // Line is more horizontally oriented, use left and right and calculate the
    // endpoints
    else if (right-left > bottom-top){
        start.x = left;
        end.x = right;
        start.y = Utility::getLineY(start.x, b, a);
        end.y = Utility::getLineY(end.x, b, a);
        // Since we are basing our end point's y values on the least squares
        // regression values, need to recalculate the top and bottom points
        if (start.y < end.y) {
            top = start.y;
            bottom = end.y;
        }
        else {
            top = end.y;
            bottom = start.y;
        }
    }
    // Line is more vertically oriented, use top and bottom to calculate the
    // other endpoints
    else {
        start.y = bottom;
        end.y = top;
        start.x = Utility::getLineX(start.y, b, a);
        end.x = Utility::getLineX(end.y, b, a);
        if (start.x < end.x) {
            left = start.x;
            right = end.x;
        }
        else {
            left = end.x;
            right = start.x;
        }
    }

    angle = getAngle(*this);
    length = getLength(*this);

    calculateWidths();
}

// Loops through all of our line points and calculates the average widths in
// the x and y direction, and assigns these widths to the avgVerticalWidth
// and avgHorizontalWidth fields.  Also assigns the min and max for the widths
void VisualLine::calculateWidths() {
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


const float VisualLine::getLength(const VisualLine& line) {
    return Utility::getLength( static_cast<float>(line.start.x),
							   static_cast<float>(line.start.y),
                               static_cast<float>(line.end.x),
							   static_cast<float>(line.end.y) );
}

// Get the angle from horizontal (in degrees) the line makes on the screen
const float VisualLine::getAngle(const VisualLine& line) {
    return Utility::getAngle(line.start.x, line.start.y,
                             line.end.x, line.end.y);
}

// We define a line to be vertically oriented if the change in y is bigger
// than the change in x
const bool VisualLine::isVerticallyOriented(const VisualLine& line) {
    return line.right - line.left < line.bottom - line.top;
}


const bool VisualLine::isPerfectlyVertical(const VisualLine& line) {
    return line.right == line.left;
}

// Return the slope of the line (returns NAN if vertical)
const float VisualLine::getSlope() const {
    return Utility::getSlope(*this);
}

// Given a line, returns (a, b) where the line can be represented by
// ai + bj (i being the unit vector parallel to the x axis, j being
// the unit vector parallel to the y axis).  Assumes that the line
// is oriented left to right.
// Our j axis will actually increase towards the bottom of the screen
pair<int, int> VisualLine::
getLineComponents(const VisualLine &aLine) {
    int i = aLine.right - aLine.left;
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
pair <float, float> VisualLine::
	leastSquaresFit(const vector <linePoint> &thePoints) {
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

    float b = ((ySum * xSquaredSum) - (xSum * xYSum)) /
		((numPoints * xSquaredSum) - (xSum * xSum)) ;

    float m =  ( (numPoints * xYSum) - (xSum * ySum) ) /
		((numPoints * xSquaredSum) - (xSum * xSum)) ;

    return pair<float, float>(m, b);
}

pair <float, float> VisualLine::leastSquaresFit(const VisualLine& l) {
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

