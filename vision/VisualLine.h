#ifndef VisualLine__hpp
#define VisualLine__hpp

#include <list>
#include <vector>
#include <string>
#include <sstream>

#include "ConcreteLine.h"
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

    // center
    ScanDirection foundWithScan;

    linePoint (int _x = 0, int _y = 0, float _lineWidth = 0.0,
               float _distance = 0.0,
               ScanDirection _scanFound = VERTICAL) :
        x(_x), y(_y), lineWidth(_lineWidth),
        distance(_distance), foundWithScan(_scanFound) {
    }

    linePoint(const linePoint& l) : x(l.x), y(l.y), lineWidth(l.lineWidth),
                                    distance(l.distance),
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
            foundWithScan == secondLinePoint.foundWithScan;
    }
    bool operator!= (const linePoint &secondLinePoint) const {
        return !(*this == secondLinePoint);
    }


    //  http://www.daniweb.com/forums/thread69005.html
    friend std::ostream& operator<< (std::ostream &o, const linePoint &c)
        {
            return o << "(" << c.x << "," << c.y << ")";
        }
};

class YOrder {
public:
    const bool operator() (const linePoint& first, const linePoint& second) const;
};


#include "Structs.h"
#include "Utility.h"

class VisualLine {
public: // Constants
    // number of points to be a valid line
    static const unsigned int NUM_POINTS_TO_BE_VALID_LINE = 3;

public:
    VisualLine(std::list<std::list<linePoint>::iterator> &listOfIterators);
    VisualLine(std::list<linePoint> &listOfPoints);
    VisualLine();
    VisualLine(const VisualLine& other);
    ~VisualLine();

    void setColor(const int c) { color = c; }
    void setColorString(const std::string s) { colorStr = s; }
    void addPoints(const std::list <linePoint> &additionalPoints);
    void addPoints(const std::vector <linePoint> &additionalPoints);

    static const linePoint DUMMY_LINEPOINT;
    const float getSlope() const;

    // Returns true if the line is more vertical on the screen than horizontal
    static const bool isVerticallyOriented(const VisualLine& line);

    static const bool isPerfectlyVertical(const VisualLine& line);

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
    void init();
    void calculateWidths();

    static inline const float getLength(const VisualLine& line);
    static inline const float getAngle(const VisualLine& line);



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
    inline float lineDistanceToSD(float _distance) {
        return (10.0f + (_distance * _distance)*0.0125f);
    }
    inline float lineBearingToSD(float _bearing) {
        return static_cast<float>(M_PI) / 8.0f;
    }


public: // Member variables (public just for now)
    point <int> start, end;
    int left,right,bottom,top; // left, right x values, bottom, top y values
    std::vector <linePoint> points;

    float angle; // Angle from horizontal in degrees
    // y = ax + b (a = slope, b = y-intercept)
    float a, b;
    float length;
    int color; // Holds the color the line is being drawn as on the screen
    std::string colorStr;

    float avgVerticalWidth, avgHorizontalWidth;
    linePoint thinnestHorPoint, thickestHorPoint;
    linePoint thinnestVertPoint, thickestVertPoint;

private: // Private member variables
    float distance;
    float bearing;
    float distanceSD;
    float bearingSD;
    std::list <const ConcreteLine*> possibleLines;

public:
    // Getters
    const float getDistance() const { return distance; }
    const float getBearing() const { return bearing; }
    const float getDistanceSD() const { return distanceSD; }
    const float getBearingSD() const { return bearingSD; }
    /**
     * @return a List of all possible lines.  Currently returns all 11 lines
     */
    const std::list <const ConcreteLine *> getPossibleLines() const {
        return possibleLines;
    }


    // Setters
    void setDistance(float _distance) { distance = _distance; }
    void setBearing(float _bearing) { bearing = _bearing; }
    void setDistanceSD(float _distanceSD) { distanceSD = _distanceSD; }
    void setBearingSD(float _bearingSD) { bearingSD = _bearingSD; }
    void setDistanceWithSD(float _distance);
    void setBearingWithSD(float _bearing);
    void setPossibleLines(std::list <const ConcreteLine*> _possibles) {
        possibleLines = _possibles;
    }

};
#endif
