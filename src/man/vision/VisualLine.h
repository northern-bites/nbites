#ifndef VisualLine__hpp
#define VisualLine__hpp

#include <list>
#include <vector>
#include <string>
#include <sstream>
#include <utility>
#include <boost/shared_ptr.hpp>

#include "ConcreteLine.h"
#include "geom/HoughLine.h"
#include "FieldLines/Gradient.h"
#include "VisualLandmark.h"

namespace man {
namespace vision {

class VisualLine;

enum ScanDirection {
    HORIZONTAL,
    VERTICAL
};

// structure for a single line point
struct linePoint {
    int x; // x coordinate on the image screen
    int y; // y coordinate on the image screen
    float lineWidth; // the width of the line where the point was found
    float distance; // The distance pose estimates the point to be from robot's
    float bearing;

    // center
    ScanDirection foundWithScan;

    linePoint (int _x = 0, int _y = 0, float _lineWidth = 0.0,
               float _distance = 0.0,
               float _bearing = 0.0,
               ScanDirection _scanFound = VERTICAL) :
    x(_x), y(_y), lineWidth(_lineWidth),
        distance(_distance), bearing(_bearing), foundWithScan(_scanFound) {
    }

linePoint(const linePoint& l) : x(l.x), y(l.y), lineWidth(l.lineWidth),
        distance(l.distance), bearing(l.bearing),
        foundWithScan(l.foundWithScan) {
}

    // define one line point as less than another if its x coord is less than
    // the other.  Otherwise, break the tie by comparing y values
    bool operator< (const linePoint &secondLinePoint) const {
        if (x < secondLinePoint.x)       return true;
        else if (x > secondLinePoint.x)  return false;
        // equals case, look to y
        else                             return y > secondLinePoint.y;
    }
    bool operator== (const linePoint &secondLinePoint) const {
        return
        x == secondLinePoint.x &&
        y == secondLinePoint.y &&
        lineWidth == secondLinePoint.lineWidth &&
        distance == secondLinePoint.distance &&
        bearing == secondLinePoint.bearing &&
        foundWithScan == secondLinePoint.foundWithScan;
    }
    bool operator!= (const linePoint &secondLinePoint) const {
        return !(*this == secondLinePoint);
    }


    //  http://www.daniweb.com/forums/thread69005.html
    friend std::ostream& operator<< (std::ostream &o, const linePoint &c)
    {
        return o << "(" << c.x << "," << c.y << "), width: " << c.lineWidth;
    }
};

class YOrder {
 public:
    const bool operator() (const linePoint& first, const linePoint& second) const;
};

}
}

#include "Structs.h"
#include "Utility.h"

namespace man {
namespace vision {

class VisualLine : public VisualLandmark {
    // Private constants
    enum {
        edge_pts_for_line = 6,
        edge_pt_buffer = 4,
        angle_epsilon = 5
    };
 public: // Constants

    static const unsigned int NUM_POINTS_TO_BE_VALID_LINE = 3;

 public:
    VisualLine(std::list<std::list<linePoint>::iterator> &listOfIterators);
    VisualLine(std::list<linePoint> &listOfPoints);
    VisualLine();
    VisualLine(const HoughLine& a, const HoughLine& b, const Gradient& g);

    VisualLine(float _dist, float _bearing);
    VisualLine(const VisualLine& other);
    ~VisualLine();

    void addPoints(const std::list <linePoint> &additionalPoints);
    void addPoints(const std::vector <linePoint> &additionalPoints);
    void addPoint(const linePoint &point);

    static const linePoint DUMMY_LINEPOINT;

    // Returns true if the line is more vertical on the screen than horizontal
    const bool isVerticallyOriented() const;

    const bool isPerfectlyVertical() const;

    // Given a line, returns (a, b) where the line can be represented by
    // ai + bj (i being the unit vector parallel to the x axis, j being
    // the unit vector parallel to the y axis).  Assumes that the line
    // is oriented left to right.
    // Our j axis will actually increase towards the bottom of the screen
    static std::pair <int, int> getLineComponents(const VisualLine &aLine);

    // By default, sort by lengths of lines.
    bool operator< (const VisualLine &secondLine) const  {
        return length < secondLine.length;
    }
    // Define equality to be if the endpoints of the line are the same
    bool operator== (const VisualLine &secondLine) const {
        return start.x == secondLine.start.x && start.y == secondLine.start.y &&
        end.x == secondLine.end.x && end.y == secondLine.end.y;
    }
    // Prints out the endpoints of the line
    friend std::ostream& operator<< (std::ostream &o, const VisualLine &l)
        {
            return o << "start.x: " << l.start.x << " start.y: " << l.start.y
                     << " end.x: " << l.end.x << " end.y: " << l.end.y;
            /*
              << l.left << " " << l.right << " " << l.top << " " << l.bottom
              << l.angle << " " << l.a << " " << l.b << " " << l.length
              << " " << l.color << endl;
            */
        }

 private: // Member functions
    typedef std::pair<point<int>, point<int> > PointsPair;

    void findEndpoints(const HoughLine& a,
                       const HoughLine& b,
                       const Gradient& g);
    PointsPair findHoughLineEndpoints(const HoughLine& a,
                                      const Gradient& g);
    void setDirectionalEndpoints(const PointsPair& a, const PointsPair& b);
    void trimEndpoints();
    void findDimensions();
    void find3DCoords();
    void findLineEdgeEnds(const HoughLine& line, const Gradient& g,
                          point<int>& r, point<int>& l);

    bool isLineEdge(const HoughLine& line,
                    const Gradient& g,
                    double x0, double y0,
                    int& _x, int& _y);

    void init();
    void calculateWidths();
    const float calculateAngle() const;
    const float calculateLength() const;

    // Use least squares to fit the line to the points
    // from http://www.efunda.com/math/leastsquares/lstsqr1dcurve.cfm
    // y = mx + b
    // returns a pair <m, b>
    static std::pair <float, float> leastSquaresFit(const std::vector <linePoint>
                                                    &thePoints);
    // Using the points in the line, finds the line of best fit that corresponds
    // with those points.
    static std::pair <float, float> leastSquaresFit(const VisualLine& l);

    //list <const ConcreteLine *> possibleLines;
    inline static float lineDistanceToSD(float _distance) {
        return (10.0f + (_distance * _distance)*0.0125f);
    }
    inline static float lineBearingToSD(float _bearing) {
        return static_cast<float>(M_PI) / 8.0f;
    }


 private:                        // Member variables
    point <int> start, end;
    // left, right x values, bottom, top y values
    int leftBound, rightBound, bottomBound, topBound;
    std::vector <linePoint> points;
    float angle;                // Angle from horizontal in degrees
    float length;               // Length on screen

    // y = ax + b (a = slope, b = y-intercept)
    float a, b;
    int color; // Holds the color the line is being drawn as on the screen
    std::string colorStr;
    float avgVerticalWidth, avgHorizontalWidth;
    linePoint thinnestHorPoint, thickestHorPoint;
    linePoint thinnestVertPoint, thickestVertPoint;
    bool parallel;

    float distance;             // Distance to the closest point on the line
    float bearing;              // Bearing to the closest point on the line
    float distanceSD;           // Standard deviation of distance measurement
    float bearingSD;            // Standard deviation of bearing measurement
    bool ccLine;                // Is this line part of the center circle?
    std::list <const ConcreteLine*> possibleLines; // Possible ConcreteLines

    // New VisualLine member variables
    //
    // bearing, sd (3 space)
    // distance, sd (3 space)
    // point<int> start, end
    // point<int>* left, *right
    // length
    HoughLine hLine1, hLine2;
    float radius; // (on screen)
    float theta; // (on screen)
    point<int> tr, tl, br, bl;  // top right/left, bottom right/left

 public:
    // Getters
    inline const bool getCCLine() const;

    inline const float getAngle() const;
    inline const float getAvgHorizontalWidth() const;
    inline const float getAvgVerticalWidth() const;
    inline const float getAvgWidth() const; // Returns width based on orientation
    inline const float getBearing() const;
    inline const float getBearingDeg() const;
    inline const float getBearingSD() const;
    inline const float getDistance() const;
    inline const float getDistanceSD() const;
    inline const float getLength() const;
    inline const float getSlope() const;
    inline const float getYIntercept() const;

    inline const int getColor() const;

    inline const linePoint getLeftLinePoint() const;
    inline const linePoint getRightLinePoint() const;
    inline const linePoint getThickestHorizontalPoint() const;
    inline const linePoint getThickestVerticalPoint() const;
    inline const linePoint getThinnestHorizontalPoint() const;
    inline const linePoint getThinnestVerticalPoint() const;

    inline const point<int> getBottomEndpoint() const;
    inline const point<int> getEndpoint() const;
    inline const point<int> getLeftEndpoint() const;
    inline const point<int> getRightEndpoint() const;
    inline const point<int> getStartpoint() const;
    inline const point<int> getTopEndpoint() const;
    inline const point<int> getTopRightEndpoint() const;
    inline const point<int> getTopLeftEndpoint() const;
    inline const point<int> getBottomRightEndpoint() const;
    inline const point<int> getBottomLeftEndpoint() const;

    inline const bool isParallel() const;

    inline const std::string getColorString() const;
    inline const std::list <const ConcreteLine *> getPossibilities() const;
    const std::vector<lineID> getIDs();
    inline const std::vector<linePoint> getPoints() const;

    virtual const bool hasPositiveID();

    // Setters
    inline void setBearing(float _bearing);
    inline void setBearingSD(float _bearingSD);
    inline void setCCLine(bool _ccLine);
    inline void setColorString(const std::string s);
    inline void setDistance(float _distance);
    inline void setDistanceSD(float _distanceSD);
    inline void setPossibleLines(const ConcreteLine* _possible);

    void setParallel() {parallel = true;}
    void setBearingWithSD(float _bearing);
    void setColor(const int c);
    void setDistanceWithSD(float _distance);
    void setPossibleLines(std::list <const ConcreteLine*> _possibles);
    void setPossibleLines(std::vector <const ConcreteLine*> _possibles);
};

/**
 * Inline "Getter" definitions
 */
inline const bool VisualLine::getCCLine() const
{
    return ccLine;
}

inline const float VisualLine::getDistance() const
{
    return distance;
}
inline const float VisualLine::getBearing() const
{
    return bearing;
}
inline const float VisualLine::getBearingDeg() const
{
  return bearing*TO_DEG;
}
inline const float VisualLine::getDistanceSD() const
{
    return distanceSD;
}
inline const float VisualLine::getBearingSD() const
{
    return bearingSD;
}

inline const float VisualLine::getLength() const
{
    return length;
}

inline const float VisualLine::getAngle() const
{
    return angle;
}

inline const float VisualLine::getSlope() const
{
    return a;
}

inline const float VisualLine::getYIntercept() const
{
    return b;
}

inline const float VisualLine::getAvgVerticalWidth() const
{
    return avgVerticalWidth;
}

inline const float VisualLine::getAvgHorizontalWidth() const
{
    return avgHorizontalWidth;
}

inline const float VisualLine::getAvgWidth() const
{
    return (isVerticallyOriented() ?
            getAvgHorizontalWidth() : getAvgVerticalWidth());
}

inline const point<int> VisualLine::getStartpoint() const
{
    return start;
}
inline const point<int> VisualLine::getEndpoint() const
{
    return end;
}

inline const point<int> VisualLine::getLeftEndpoint() const
{
    return (leftBound == start.x ? start : end);
}

inline const point<int> VisualLine::getRightEndpoint() const
{
    return (rightBound == start.x ? start : end);
}

inline const point<int> VisualLine::getTopEndpoint() const
{
    return (topBound == start.y ? start : end);
}

inline const point<int> VisualLine::getBottomEndpoint() const
{
    return (bottomBound == start.y ? start : end);
}


inline const point<int> VisualLine::getTopRightEndpoint() const
{
    return tr;
}
inline const point<int> VisualLine::getTopLeftEndpoint() const
{
    return tl;
}
inline const point<int> VisualLine::getBottomRightEndpoint() const
{
    return br;
}
inline const point<int> VisualLine::getBottomLeftEndpoint() const
{
    return bl;
}

inline const linePoint VisualLine::getLeftLinePoint() const
{
    return points.front();
}

inline const linePoint VisualLine::getRightLinePoint() const
{
    return points.back();
}

inline const linePoint VisualLine::getThinnestHorizontalPoint() const
{
    return thinnestHorPoint;
}
inline const linePoint VisualLine::getThinnestVerticalPoint() const
{
    return thinnestVertPoint;
}
inline const linePoint VisualLine::getThickestHorizontalPoint() const
{
    return thickestHorPoint;
}
inline const linePoint VisualLine::getThickestVerticalPoint() const
{
    return thickestVertPoint;
}


inline const std::vector<linePoint> VisualLine::getPoints() const
{
    return points;
}

inline const bool VisualLine::isParallel() const
{
    return parallel;
}

inline const int VisualLine::getColor() const
{
    return color;
}

inline const std::string VisualLine::getColorString() const
{
    return colorStr;
}

inline
const std::list <const ConcreteLine *> VisualLine::getPossibilities() const
{
    return possibleLines;
}

/**
 * Inline setters
 */
inline void VisualLine::setColorString(const std::string s)
{
    colorStr = s;
}

inline void VisualLine::setPossibleLines(const ConcreteLine* _possible) {
    if (_possible == NULL)
        return;
    possibleLines.clear();
    possibleLines.push_back(_possible);
}

inline void VisualLine::setBearing(float _bearing)
{
    bearing = _bearing;
}
inline void VisualLine::setBearingSD(float _bearingSD)
{
    bearingSD = _bearingSD;
}
inline void VisualLine::setCCLine(bool _ccLine)
{
    ccLine = _ccLine;
}
inline void VisualLine::setDistance(float _distance)
{
    distance = _distance;
}
inline void VisualLine::setDistanceSD(float _distanceSD)
{
    distanceSD = _distanceSD;
}

}
}

#endif
