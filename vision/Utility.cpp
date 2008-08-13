
#include <cstdlib>

#include "Utility.h"

 // We often deal with an estimated value in comparison with a known value and
// its necessity of being within a certain amount in order to be acceptable.
bool CLOSE_ENOUGH(double arg1, double arg2, double errorAllowed) {
  return (fabs(arg2-arg1) < errorAllowed);
}

/*const double hypot(const double a, const double b) {
  return sqrt((a * a) + (b * b));
  }*/


/* From Joseph O'Rourke's Computational Geometry in C book, published 1998, p.27
 * Calculates twice the *signed* area of the triangle specified by three points
 */
const int Utility::area2(const point<int> &a, const point <int> &b,
                         const point <int> &c) {
  // Since our Y axis is backwards, we have to swap what we subtract for y values
  return (b.x - a.x) * (a.y - c.y) -//(c.y - a.y) -
    (c.x - a.x) * (a.y - b.y); //(b.y - a.y);
}

/* From Joseph O'Rourke's Computational Geometry in C book, published 1998, p.29
 * "A directed line is determined by two points given in a particular order 
 * (a,b).  If a point c is to the left of the line determined by (a,b), 
 * then the triple (a,b,c) forms a counterclockwise circuit... c is to the 
 * left of (a,b) iff the area of the counterclockwise triangle A(a,b,c) is
 * positive"
 */
// Returns true if point c is to the left of the line directed from a to b
const bool Utility::left(const point<int> &a, const point <int> &b,
                         const point <int> &c) {
  return area2(a, b, c) > 0;
}
// p.29
// Returns true if point c is to the left of the line or colinear with the
// line directed from a to b
const bool Utility::leftOn(const point<int> &a, const point <int> &b, 
                           const point <int> &c) {
  return area2(a, b, c) >= 0;
}
// p.29
// Returns true if point c is collinear with the line directed form a to b
const bool Utility::collinear(const point<int> &a, const point <int> &b,
                              const point <int> &c) {
  return area2(a, b, c) == 0;
}

// Returns true if the point is in the rectangle formed from the VisualLine's
// endpoints
const bool Utility::between(const VisualLine& line,
                            const point <int>& p) {
  return 
    // Point is between the endpoints in x direction
    (p.x >= line.left && p.x <= line.right) &&
    // Point is between the endpoints in y direction (note that the the higher
    // in the image it is, the lower the y coordinate)
    (p.y >= line.top && p.y <= line.bottom);
}

const bool Utility::between(const VisualLine& line,
                            const linePoint& p) {
  const point <int> point(p.x, p.y);
  return between(line, point);
}


const point<int> Utility::getPointFartherFromCorner(const VisualLine &l,
                                                    int cornerX,
                                                    int cornerY) {
  double startPointDistance = getLength(l.start.x, l.start.y, cornerX, cornerY);
  double endPointDistance = getLength(l.end.x, l.end.y, cornerX, cornerY);
  if (startPointDistance < endPointDistance) {
    return l.end;
  }
  else {
    return l.start;
  }
}

const point<int> Utility::getCloserEndpoint(const VisualLine& l, int x, int y) {
  double startPointDistance = getLength(l.start.x, l.start.y, x, y);
  double endPointDistance = getLength(l.end.x, l.end.y, x, y);
  if (startPointDistance < endPointDistance) {
    return l.start;
  }
  else {
    return l.end;
  }

}
const point<int> Utility::getCloserEndpoint(const VisualLine& l, const point<int>& p) {
  return getCloserEndpoint(l, p.x, p.y);
}



// Returns true if the line segments (a,b) and (c,d) intersect "at a point
// interior to both" 
// p.30
const bool Utility::intersectProp(const point<int> &a, const point<int> &b,
                                  const point<int> &c, const point<int> &d) {
    // Eliminate improper cases
  if (collinear(a,b,c) ||
      collinear(a,b,d) ||
      collinear(c,d,a) ||
      collinear(c,d,b)) {
    return false;
  }
  
  // ^ = exclusive or.  returns 1 if both operands are different.
  // If you draw a diagram, it's obvious that if c and d are on opposite sides
  // of (a,b) and a and b are on opposite sides of (c,d), the lines must 
  // intersect
  return (left(a,b,c) ^ left(a,b,d)) 
    && (left(c,d,a) ^ left(c,d,b));
}



// get slope given x1,y1 and x2,y2
double Utility::getSlope(int x1, int y1, int x2, int y2) {
  return (double) ((double)(y2-y1)/(double)(x2-x1));
}

double Utility::getSlope(const VisualLine& line) {
  return getSlope(line.start.x,line.start.y,line.end.x,line.end.y);
}


// :TODO: This is broken.  -1/slope is perpendicular slope; plus it doesn't
// deal with horizontal lines correctly.
double Utility::getPerpenSlope(double slope) {
  if (slope != 0)
    return 1/slope;
  return 0;
}

// See http://mathworld.wolfram.com/LeastSquaresFitting.html for illustration;
// we calculate vertical (or horizontal) offsets from the line rather than
// perpendicular offsets
double Utility::getPointDeviation(const VisualLine &aLine, const int x, const int y) {
  // Bigger change in x than in y, nearer to horizontal than vertical
  if ( abs(aLine.end.x - aLine.start.x) > abs(aLine.end.y - aLine.start.y) ) {
    double lineY = getLineY(x, aLine);
    return fabs(y - lineY);
  }
  // Perfectly vertical; no need to find where the point would hit.
  // Hack.
  else if (aLine.left == aLine.right) {
    return abs(aLine.left - x);
  }
  // A line that is not perfectly vertical but is more vertical than horizontal
  else {
    double lineX = getLineX(y, aLine);
    return fabs(x - lineX);
  }
}

// Calculate how well the point fits to the line.  
// Return distance between point and line evaluated at same x or y (depending
// on orientation of the line)
double Utility::getPointDeviation(const VisualLine &aLine, const linePoint &point) {
  return getPointDeviation(aLine, point.x, point.y);  
}

// get length of line segment specified by (x1, y1), (x2, y2)
const double Utility::getLength(const double x1, const double y1, 
                                const double x2, const double y2) {
  return sqrt(pow(y2-y1,2)+pow(x2-x1,2));
}

// get length of line segment specified by (x1, y1), (x2, y2)
const double Utility::getLength(const point <const double> &p1, 
                                const point <const double> &p2) {
  return getLength(p1.x, p1.y, p2.x, p2.y);
}


const double Utility::getLength(const point <const int> &p1,
                                const point <const int> &p2) {
  return getLength(p1.x, p1.y, p2.x, p2.y);
}



// get angle between two lines 
// http://www.tpub.com/math2/5.htm
double Utility::getAngle(const VisualLine& line1, const VisualLine& line2) {
  return DEG_OVER_RAD * atan((line2.a - line1.a) / (1 + line1.a * line2.a));
}

double Utility::getAngle(int x1, int y1, int x2, int y2) {
  if (x2 == x1) {
    if (y2 > y1) return -90.0;
    else         return 90.0;
  }
  // y1-y2 due to flipped coordinate system
  return DEG_OVER_RAD * atan( static_cast<double>(y1-y2) / static_cast<double>(x2-x1) );
}


// get y-coord with given x-coord, slopoe, and y-intercept
int Utility::getLineY(int x, double y_intercept, double slope) {
  return ROUND(y_intercept+slope*x);
}

int Utility::getLineY(int x, const VisualLine &aLine) {
  return getLineY(x, aLine.b, aLine.a);
}

// VERY IMPORTANT: Ensure that the slope is not infinity! 
// get x-coord with given y-coord, slope, and y-intercept
int Utility::getLineX(int y, double y_intercept, double slope) {
  if (slope != 0) {
    return ROUND((double)(y - y_intercept)/(double)slope);
  }
  return 0;
}

int Utility::getLineX(int y, const VisualLine &aLine) {
  return getLineX(y, aLine.b, aLine.a);
}


// get y-intercept given line slope plus a point the line goes through
double Utility::getInterceptY(int x1, int y1, double slope){
  return (-slope*x1 + y1);
}

// Returns the (x, y) coordinate where the two lines intersect, or (NO_INTERSECTION, NO_INTERSECTION)
// if they do not intersect.  (since we are only concerned  about 
// intersections that appear on the screen, this will not be a problem)
const point<int> Utility::getIntersection(const VisualLine& line1,
					  const VisualLine& line2) {
  return getIntersection(line1.start, line1.end,
                         line2.start, line2.end);
}

const point <int> Utility::getIntersection(const point<int> line1Start,
                                           const point<int> line1End,
                                           const point<int> line2Start,
                                           const point<int> line2End) {
  int I_A1 = line1End.y-line1Start.y;
  int I_B1 = line1Start.x-line1End.x;
  int I_C1 = I_A1*line1Start.x+I_B1*line1Start.y;
  
  int I_A2 = line2End.y-line2Start.y;
  int I_B2 = line2Start.x-line2End.x;
  int I_C2 = I_A2*line2Start.x+I_B2*line2Start.y;
  

  int I_det = I_A1*I_B2 - I_A2*I_B1;/*
  cout << line1Start << "\t" <<  line1End << endl;
  cout << line2Start << "\t" << line2End << endl;

  cout << "I_A1 I_B1 I_C1 I_A2 I_B2 I_C2 I_DET" << endl;
  cout << I_A1 << " " <<  I_B1 << " " << I_C1 << " " << I_A2 
       << " " << I_B2 << " " << I_C2 << " " << I_det << endl;
                                    */
  
  point <int> intersection;
  if (I_det != 0) {
    intersection.x = static_cast<int>((I_B2*I_C1 - I_B1*I_C2)/I_det);
    intersection.y = static_cast<int>((I_A1*I_C2 - I_A2*I_C1)/I_det);
  }
  else {
    intersection.x = NO_INTERSECTION;
    intersection.y = NO_INTERSECTION;
  }
  
  return intersection;

}
                                           





template <typename T>
bool Utility::isElementInArray(const T elem, const T list[], const int numElems) {
  for (int i = 0; i < numElems; i++) {
    if (elem == list[i])
      return true;
  }
  return false;
}

bool Utility::isElementInArray(const int elem, const int list[], const int numElems) {
  for (int i = 0; i < numElems; i++) {
    if (elem == list[i])
      return true;
  }
  return false;
}




// Return the four corners of the bounding box surrounding a line 
// Uses trig to determine how to shift things orthogonally and parallel
// to the line to determine the endpoints
// Future: Rewrite using parameterizations of the line?
// Keyword parameters:
// x1                x coordinate of an endpoint of the line
// y1                y coordinate of an endpoint of the line
// x2                x coordinate of second endpoint of the line
// y2                y coordinate of second endpoint of line
// orthoBuff         the number of pixels to extend the box perpendicularly
//                   to the line, i.e. the width of the box / 2.
//                   Must be positive
// paraBuff          the number of pixels to extend the box parallel
//                   to the line, i.e. extending the length of the box by
//                   this amount on either side
//                   Must be positive
//
// Return:
// thePoints        4 point <int> structs (having x and y values) representing
//                  the location of the corners of the bounding box.
//                  NOTE:  The method is set up in such a way that 
//                  connecting thePoints[0] to [1], [1] to [2], [2] to [3],
//                  and [3] to [0] draws the correct shape on the screen.
//                  This allows the drawing to be easily done using a for
//                  loop and modular arithmetic, e.g.
//                  for (int i = 0; i < 4; i ++) {
//                           vision -> drawLine(thePoints[i].x, thePoints[i].y 
//                           thePoints[(i + 1) % 4].x, thePoints[(i + 1) % 4].y
//                           COLOR)
//                  }

BoundingBox Utility::getBoundingBox(int x1, int y1, int x2, int y2, 
                                             int orthoBuff, 
                                             int paraBuff) {

  BoundingBox box;
  
  // Check legitimacy of arguments
  if (orthoBuff < 0 || paraBuff < 0) {
    cout << "Error: invalid arguments; both orthoBuff and paraBuff must be "
	 << "positive ints" << endl;
    return box;
  }

  // calculations require x1 to be left most point, swap x1, y1 with x2, y2
  // if it's not
  if (x1 > x2) {
    int temp = x2;
    x2 = x1;
    x1 = temp;

    temp = y2;
    y2 = y1;
    y1 = temp;
  }

  // Use ternary operators to determine top and bottom y value.
  int top = (y2 > y1) ? y1: y2;
  int bottom = (y2 > y1) ? y2: y1;


  // vertical line
  if (x1 - x2 == 0) {
    // Easy case, just add or subtract orthoBuff to x values, since
    // 90 degrees from a vertical line is simply up and down.  The paraBuff
    // serves to lengthen the y values.  Note:  since our coordinate system
    // uses 0, 0 as top left corner, to increase the length of the line to the
    // top, you *subtract*, whereas to extend on the bottom you *add*
    box.corners[0].x = x1 - orthoBuff;
    box.corners[0].y = top - paraBuff;
   
    box.corners[1].x = x1 + orthoBuff;
    box.corners[1].y = top - paraBuff;
    
    box.corners[2].x = x2 + orthoBuff;
    box.corners[2].y = bottom + paraBuff;
    
    box.corners[3].x = x2 - orthoBuff;
    box.corners[3].y = bottom + paraBuff;

    return box;

  }

  // Can divide with impunity now that we've removed chance of division by 0
  double slope = (y2 - y1)/(double)(x2 - x1);

  // Horizontal line
  if (slope == 0) {

    // Now the paraBuff adds to the x values rather than y values because
    // the line is oriented horizontally

    box.corners[0].x = x1 - paraBuff;
    box.corners[0].y = y1 + orthoBuff;

    box.corners[1].x = x1 - paraBuff;
    box.corners[1].y = y1 - orthoBuff;

    box.corners[2].x = x2 + paraBuff;
    box.corners[2].y = y2 - orthoBuff;
    
    box.corners[3].x = x2 + paraBuff;
    box.corners[3].y = y2 + orthoBuff;

    return box;

  }

  // This is the most complicated part.  Basically, we need to break down
  // the slanted line into its x and y components in order to figure out
  // how much to add in x and y direction.  It's fairly simple to do that
  // with just paraBuff (i.e., to extend a line into space), but in order
  // to extend in the orthogonal direction we need to use the orthogonal
  // slope.
      
  double alpha = atan(slope);
  double betaX = paraBuff * cos(alpha);
  double betaY = paraBuff * sin(alpha);

  double orthoSlope = -1.0 / slope;

  double theta = atan(orthoSlope);
  double deltaX = orthoBuff * cos(theta);
  double deltaY = orthoBuff * sin(theta);

  box.corners[0].x = (int)(x1 + deltaX - betaX);
  box.corners[0].y = (int)(y1 + deltaY - betaY);
  
  box.corners[1].x = (int)(x1 - deltaX - betaX);
  box.corners[1].y = (int)(y1 - deltaY - betaY);
  
  box.corners[2].x = (int)(x2 - deltaX + betaX);
  box.corners[2].y = (int)(y2 - deltaY + betaY);
  
  box.corners[3].x = (int)(x2 + deltaX + betaX);
  box.corners[3].y = (int)(y2 + deltaY + betaY);
  
  return box;

}


// See explanation above as to what the bounding box method does in general.
// This fits a bounding box to the least squares equation of the line passed
// in.
BoundingBox Utility::getBoundingBox(const VisualLine& aLine, 
                                    int orthogonalRadius,
                                    int parallelRadius) {
  return getBoundingBox(aLine.start.x, aLine.start.y, aLine.end.x, aLine.end.y,
                        orthogonalRadius, parallelRadius);
}


// Determine whether a vertical line segment intersects a line segment
// Keyword parameters:
// plumbTop           the (x,y) coordinate of the top of the plumb line
// plumbBottom        the (x,y) coordinate of the bottom of the plumb line; 
//                    for our purposes this will always be IMAGE_HEIGHT
// line1start         the (x,y) coordinate of one of the endpoints of the line
// line1end           the (x,y) coordinate of the second of the endpoints of 
//                    the line.
// Returns (NO_INTERSECTION, NO_INTERSECTION) if no intersection, else (x, y) where x is x coord of
// intersection point, y is y coord.
pair<int, int> Utility::plumbIntersection(point <int> plumbTop, 
                                          point <int> plumbBottom, 
                                          point <int> line1start, 
                                          point <int> line1end) {

  // Use ternary operation to determine the left, right, and bottom value
  int line1Left = (line1start.x < line1end.x) ? line1start.x : line1end.x;
  int line1Right = (line1start.x > line1end.x) ? line1start.x : line1end.x;
  int line1Bottom = (line1start.y > line1end.y) ? line1start.y : line1end.y;

  pair <int, int> intersection;
  intersection.first = intersection.second = NO_INTERSECTION;
  
  // plumbline lies to right or left of endpoints of line; no intersection
  if (plumbTop.x > line1Right || plumbTop.x < line1Left) {
    return intersection;
  }
  // plumbline starts below the lowest part of line
  else if (plumbTop.y>line1Bottom) {
    return intersection;
  }

  // vertical line
  if (line1start.x - line1end.x == 0) {
    // We'll count it as an intersection only if they have the same x value 
    // (i.e. they run into each other going down the screen)
    if (plumbTop.x == line1start.x) {
      intersection.first = plumbTop.x;
      intersection.second = line1Bottom;
    }
    return intersection;
  }

  // easy cases out of way, have to calculate the y value of where the 
  // intersection point would be
  // Will not divide by 0 here since we checked for vertical line already.
  double slope = (double) (line1end.y - line1start.y) / 
    (double) (line1end.x - line1start.x);

  double intercept = line1end.y - slope * line1end.x;
  // Determine the y value of line at x value of plumbline
  double y = slope * plumbTop.x + intercept;
 
  if (y >= plumbTop.y && y <= plumbBottom.y) {
    intersection.first = plumbTop.x;
    intersection.second = (int) y;
  }
  // if y weren't in the range of the plumb line, then return NO_INTERSECTION, NO_INTERSECTION.
  return intersection;
}

  



// This method is meant to take a point on a line, and find what distance
// that point is away from the start (x1, x2) of the line.
// We parameterize the line such that x and y are functions of one
// variable t. Then we figure out what t gives us the corner's
// x coordinate. When t = 0, x = x1; when t = lineLength, x = x2;
// Since this relies on parametrization, we could end up with infinite slope.
// For better accuracy, we use x to find t when the line is less than 45 degs
// from horizontal and y otherwise.
double Utility::findLinePointDistanceFromStart(const point <int> &p,
                                               const VisualLine &aLine) {
  
  return findLinePointDistanceFromStart(p, aLine.start, aLine.end,
                                        aLine.length);
}


double Utility::findLinePointDistanceFromStart(const point <int> &p,
                                               const point<int> &lineStart,
                                               const point<int> &lineEnd,
                                               const double length) {
  if ( abs(lineEnd.x - lineStart.x) > abs(lineEnd.y - lineStart.y) )
    return length * static_cast<double>(p.x - lineStart.x) /
            static_cast<double>(lineEnd.x - lineStart.x);
  else
    return length * static_cast<double>(p.y - lineStart.y) /
            static_cast<double>(lineEnd.y - lineStart.y);
}

/*
 * Any point on a line can be represented by a single value, t1, when the
 * line has been parameterized in such a way that x and y depend on a new 
 * variable t.  Given such a t1 we can calculate where along the line segment
 * the point lies.  We parameterize our line such that when t = 0, x = x1 and
 * y = y1, and when t = length of line, x = x2 and y = y2.  Thus if t < 0 the
 * point lies "before" the first end point and if t > length of line, the
 * point lies "after" the second end point.  
 *
 * This method determines whether a given t value lies in the "middle" of the
 * line (not too close to either end point)
 * 
 * @param t1 the value for which the line reaches the point in question
 * @param length the length of the line
 */
// TODO: think of a better name for this
const bool Utility::tValueInMiddleOfLine(const double t1, const double length,
				const double minExtendDistance) {
  return t1 > minExtendDistance && t1 < length - minExtendDistance;
}

/*
License to Use

Copyright (c) 1970-2003, Wm. Randolph Franklin

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

   1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimers.
   2. Redistributions in binary form must reproduce the above copyright notice in the documentation and/or other materials provided with the distribution.
   3. The name of W. Randolph Franklin may not be used to endorse or promote products derived from this Software without specific prior written permission. 

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE. 
http://www.ecse.rpi.edu/Homepages/wrf/Research/Short_Notes/pnpoly.html
*/
int pnpoly(int nvert, float *vertx, float *verty, float testx, float testy)
{
  int i, j, c = 0;
  for (i = 0, j = nvert-1; i < nvert; j = i++) {
    if ( ((verty[i]>testy) != (verty[j]>testy)) &&
	 (testx < (vertx[j]-vertx[i]) * (testy-verty[i]) / (verty[j]-verty[i]) + vertx[i]) )
       c = !c;
  }
  return c;
}

bool Utility::boxContainsPoint(BoundingBox box, int x, int y) {
  float xCoords[] = {box.corners[0].x,
                 box.corners[1].x,
                 box.corners[2].x,
                 box.corners[3].x};
  float yCoords[] = {box.corners[0].y,
                     box.corners[1].y,
                     box.corners[2].y,
                     box.corners[3].y};
  // pnpoly returns 1 iff point is within the polygon described
  return static_cast<bool>(pnpoly(4, xCoords, yCoords, x, y));
}


// Returns whether or not both entries in the pair are legitimate, i.e.
// not our NO_INTERSECTION value
bool Utility::isLegitIntersection(int x, int y) {
  return x != NO_INTERSECTION && y != NO_INTERSECTION;
}

bool Utility::isPointOnScreen(int x, int y) {
  if (x >= 0 && x < IMAGE_WIDTH && y >= 0 && y < IMAGE_HEIGHT) {
    return true;
  }
  return false;
}

bool Utility::isPointOnScreen(point<int> p) {
  return isPointOnScreen(p.x, p.y);
}


const string Utility::getColorString(int _color) {
  switch (_color) {
  case GREY:
    return "GREY";
  case WHITE:
    return "WHITE";
  case GREEN:
    return "GREEN";
  case BLUE:
    return "BLUE";
  case YELLOW:
    return "YELLOW";
  case ORANGE:
    return "ORANGE";
  case YELLOWWHITE:
    return "YELLOWWHITE";
  case BLUEGREEN:
    return "BLUEGREEN";
  case ORANGERED:
    return "ORANGERED";
  case ORANGEYELLOW:
    return "ORANGEYELLOW";
  case RED:
    return "RED";
  case NAVY:
    return "NAVY";
  case BLACK:
    return "BLACK";
  case PINK:
    return "PINK";
  case SHADOW:
    return "SHADOW";
  case CYAN:
    return "CYAN";
  default:
    return "Don't know what color";
  }
}

const string Utility::getCertaintyString(int _cert) {
  switch (_cert) {
  case SURE: return "SURE";
  case MILDLYSURE: return "MILDLYSURE";
  case NOTSURE: return "NOTSURE";
  default: return "Don't know what certainty you're talking about, mate";
  }  
}

const string Utility::getDistCertaintyString(int _cert) {
  switch (_cert) {
  case BOTH_SURE: return "BOTH_SURE";
  case HEIGHT_UNSURE: return "HEIGHT_UNSURE";
  case WIDTH_UNSURE: return "WIDTH_UNSURE";  
  case BOTH_UNSURE: return "BOTH_UNSURE";
  default: return "Don't know what certainty you're talking about, mate";
  }
}  








