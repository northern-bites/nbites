#ifndef Utility_hpp_DEFINED
#define Utility_hpp_DEFINED

#include <cmath>          // Uses fabs, trig functions like atan

class Utility;
#include "Common.h"
#include "Structs.h"
#include "VisualLine.h"
#include "VisionDef.h"
#include "VisualLandmark.h"
#include "VisualLine.h"
#include "NBMath.h"

// This class contains static utility functions which are used in many different
// parts of our code.
//

class Utility {



public:

    static const int NO_INTERSECTION = -13591295;

    // The following 5 methods come from Joseph O'Rourke's Computational
    // Geometry in C book, published in 1998.
    // Calculates twice the area of the triangle specified by (a,b,c)
    static const int area2(const point<int> &a, const point <int> &b,
                           const point <int> &c);
    // Returns true if point c is to the left of the line directed from a to b
    static const bool left(const point<int> &a, const point <int> &b,
                           const point <int> &c);
    // Returns true if point c is to the left of the line or colinear with the
    // line directed from a to b
    static const bool leftOn(const point<int> &a, const point <int> &b,
                             const point <int> &c);
    // Returns true if point c is collinear with the line directed form a to b
    static const bool collinear(const point<int> &a, const point <int> &b,
                                const point <int> &c);

    // Returns true if the point is in the rectangle formed from the
	// VisualLine's endpoints
    static const bool between(const VisualLine& line,
                              const point <int>& p);


    static const bool between(const VisualLine& line,
                              const linePoint& p);

    // @return True when a is between b and c (in the bounding box
    // formed by them)
    static bool between(const point<int>& a,
                        const point<int>& b,
                        const point<int>& c){
        return (a.x < std::max(b.x, c.x) && a.x > std::min(b.x, c.x) &&
                a.y < std::max(b.y, c.y) && a.y > std::min(b.y, c.y));
    }

    // Returns true if the line segments (a,b) and (c,d) intersect "at a point
    // interior to both"
    // p.30.
    static const bool intersectProp(const point<int> &a, const point<int> &b,
                                    const point<int> &c, const point<int> &d);

    static const bool intersectProp(const VisualLine& line1,
									const VisualLine& line2);

    static const point<int> getPointFartherFromCorner(const VisualLine &l,
													  int cornerX, int cornerY);


    static const point<int> getCloserEndpoint(const VisualLine& l,
											  int x, int y);
    static const point<int> getCloserEndpoint(const VisualLine& l,
											  const point<int>& p);

    //static const float hypot(const float a, const float b);

    // get slope given x1,y1 and x2,y2
    static float getSlope(int x1, int y1, int x2, int y2);
    static float getSlope(const VisualLine& line);


    static float getPerpenSlope(float slope);

    // get length of line segment specified by (x1, y1), (x2, y2)
    static const float getLength(const int x1, const int y1,
                          const int x2, const int y2);
    static const float getLength(const float x1, const float y1,
                                 const float x2, const float y2);
    static const double getLength(const double x1, const double y1,
                                 const double x2, const double y2);
    static const float getLength(const point <float> &p1,
                                 const point <float> &p2);
    static const float getLength(const point <int> &p1,
                                 const point < int> &p2);

    // Returns the square of the length for efficiency purposes
    static const float getLength2(const float x1, const float y1,
                                  const float x2, const float y2);
    static const float getLength2(const int x1, const int y1,
                                  const int x2, const int y2);

    // get angle between two lines
    // http://www.tpub.com/math2/5.htm
    static float getAngle(const VisualLine& line1, const VisualLine& line2);

    // Get the angle between the horizontal axis and the line specified by
    // (x1,y1),(x2,y2)
    static float getAngle(int x1, int y1, int x2, int y2);

    static float getAngle(const VisualLine& line1);

    static float getAbsoluteAngle(const point<int>& intersection,
                                  const VisualLine& line1,
                                  const VisualLine& line2);

    // get y-coord with given x-coord, slope, and y-intercept
    static int getLineY(int x, float y_intercept, float slope);
    // get y-coord with given x-coord given a line
    static int getLineY(int x, const VisualLine &aLine);

    // get x-coord with given y-coord, slope, and y-intercept
    static int getLineX(int y, float y_intercept, float slope);
    // get x-coord with given y-coord and a line
    static int getLineX(int y, const VisualLine &aLine);


    // get y-intercept given line slope plus a point the line goes through
    static float getInterceptY(int x1, int y1, float slope);

    template <typename T>
    static bool isElementInArray(const T elem, const T list[],
                                 const int numElems);
    static bool isElementInArray(const int elem, const int list[],
                                 const int numElems);


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
    // thePoints      4 point <int> structs (having x and y values) representing
    //                  the location of the corners of the bounding box.
    //                  NOTE:  The method is set up in such a way that
    //                  connecting thePoints[0] to [1], [1] to [2], [2] to [3],
    //                  and [3] to [0] draws the correct shape on the screen.
    //                  This allows the drawing to be easily done using a for
    //                  loop and modular arithmetic, e.g.
    //                  for (int i = 0; i < 4; i ++) {
    //                       vision -> drawLine(thePoints[i].x, thePoints[i].y
    //                       thePoints[(i + 1) % 4].x, thePoints[(i + 1) % 4].y
    //                       COLOR)
    //                  }

    static BoundingBox getBoundingBox(int x1, int y1, int x2, int y2,
                                      int orthoBuff,
                                      int paraBuff);


    static BoundingBox getBoundingBox(const VisualLine& l, int orthogonalRadius,
                                      int parallelRadius);



    // Determine whether a vertical line segment intersects a line segment
    // Keyword parameters:
    // plumbTop           the (x,y) coordinate of the top of the plumb line
    // plumbBottom        the (x,y) coordinate of the bottom of the plumb line;
    //                    for our purposes this will always be IMAGE_HEIGHT
    // line1start         the (x,y) coordinate of one of the endpoints
    // line1end           the (x,y) coordinate of the second of the endpoints of
    //                    the line.
    // Returns (NO_INTERSECTION, NO_INTERSECTION)
    // if no intersection, else (x, y) where x is x coord of
    // intersection point, y is y coord.
    static std::pair<int, int> plumbIntersection(point <int> plumbTop,
                                                 point <int> plumbBottom,
                                                 point <int> line1start,
                                                 point <int> line1end);



    // Returns the (x, y) coordinate where the two lines intersect, or
    // (NO_INTERSECTION, NO_INTERSECTION) otherwise.
    // if they do not intersect.  (since we are only concerned  about
    // intersections that appear on the screen, this will not be a problem)
    static const point<int> getIntersection(const VisualLine& line1,
                                            const VisualLine& line2);


    static const point <int> getIntersection(const point<int> line1Start,
                                             const point<int> line1End,
                                             const point<int> line2Start,
                                             const point<int> line2End);

    static float findLinePointDistanceFromStart(const point <int> &p,
                                                const VisualLine &aLine);

    static float findLinePointDistanceFromStart(const point <int> &p,
                                                const point<int> &lineStart,
                                                const point<int> &lineEnd,
                                                const float length);

    static const bool tValueInMiddleOfLine(const float t1, const float length,
                                           const float minExtendDistance);

    // Calculate how well the point fits to the line.
    // Return distance between point and line evaluated at same x or y
    // (depending on orientation of the line)
    static float getPointDeviation(const VisualLine &aLine,
								   const linePoint &point);
    static float getPointDeviation(const VisualLine &aLine, const int x,
								   const int y);


    // A closed polygon contains a point if and only if a plumb line
    // dropped down from the point passes through (intersects) an odd number of
    // sides.  See http://www.fandm.edu/x7736.xml
    // We check for intersection at all 4 sides of the box; if an odd number is
    // hit, then we return true, else false
    static bool boxContainsPoint(BoundingBox box, int x, int y);

    static bool isLegitIntersection(int x, int y);

    static bool isPointOnScreen(int x, int y);

    static bool isPointOnScreen(point<int> p);

    static const std::string getColorString(int _color);

    static const std::string getCertaintyString(int _cert);

    static const std::string getDistCertaintyString(int _cert);

    static const std::string getCornerIDString(int _id);

	static const point<int> findCloserEndpoint(const VisualLine&,
											   const point<int>& intersection);

    // Returns the angle between two lines using their bearing from the robot
    static float getGroundAngle(const VisualLine& line1,
                                const VisualLine& line2);

    /**
     * Returns true if pt2 is on the opposite side of the line from the pt1.
     * If a point is on the line, the other point is not considered "across"
     * from it.
     */
    static bool areAcrossLine(const VisualLine& line,
                             const point<int>& p1,
                             const point<int>& p2);

    static float distToLine(const VisualLine& line, const point<int>& point);

    static point<int> getClosestPointOnLine(const VisualLine& line,
                                            const point<int>& pt);

    // Get closest point to point a which is on line from b->c
    static inline point<int> getClosestPointOnLine(const point<int>& a,
                                                   const point<int>& b,
                                                   const point<int>& c){

        /*
         *        *a
         *       /|
         *      / |
         *     /  |
         *    b*--*-------------*c
         *        ^find this point
         *
         * Finds the ^ point by computing the (b,a) dot (b,c)
         *       and extending that result in the direction from
         *       b to c.
         */

        float a_to_b_x = static_cast<float>(a.x - b.x);
        float a_to_b_y = static_cast<float>(a.y - b.y);

        float lx = static_cast<float>(c.x - b.x);
        float ly = static_cast<float>(c.y - b.y);

        // Unit vector
        float len = hypotf(lx,
                           ly);
        float lx_unit = lx/len;
        float ly_unit = ly/len;

        float dot = (a_to_b_x * lx_unit) + (a_to_b_y * ly_unit);

        int perp_x = b.x + static_cast<int>(lx_unit*dot);
        int perp_y = b.y + static_cast<int>(ly_unit*dot);

        return point<int>(perp_x, perp_y);
    }

    static point<int> getClosestLinePoint(const VisualLine& line,
                                          const point<int>& pt);


    // Helper method that just returns whether the thresholded color is a
    // green color
    static inline const bool isGreen(unsigned char threshColor)
        {
			return threshColor & GREEN_BIT;
        }

    // Helper method that just returns whether the thresholded color is a
    // white color
    static inline const bool isWhite(unsigned char threshColor)
        {
			return threshColor & WHITE_BIT;
        }

    // Helper method that just returns whether the thresholded color is a
    // blue color
    static inline const bool isBlue(unsigned char threshColor)
        {
			return threshColor & BLUE_BIT;
        }

    // Helper method that just returns whether the thresholded color is a
    // yellow color
    static inline const bool isYellow(unsigned char threshColor)
        {
			return threshColor & YELLOW_BIT;
        }

    // Helper method that just returns whether the thresholded color is a
    // orange color
    static inline const bool isOrange(unsigned char threshColor)
        {
			return threshColor & ORANGE_BIT;
        }

    // Helper method that just returns whether the thresholded color is a
    // navy color
    static inline const bool isNavy(unsigned char threshColor)
        {
			return threshColor & NAVY_BIT;
        }

    // Helper method that just returns whether the thresholded color is a
    // Red color
    static inline const bool isRed(unsigned char threshColor)
        {
			return threshColor & RED_BIT;
        }

    // Helper method that just returns whether the thresholded color is
    // undefined
    static inline const bool isUndefined(unsigned char threshColor)
        {
			return threshColor == 0x00;
        }

	// are two colors equal?
	static inline const bool colorsEqual(unsigned char x, unsigned char y) {
		return !((x & y) == 0x00);
	}




};


#endif
