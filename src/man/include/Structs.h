/* Contains the Most basic common information/types/defs */


#ifndef Structs_h_DEFINED
#define Structs_h_DEFINED

#include "Common.h"

#include <iostream>

//////////// VISION STRUCTURES ////////////


template <typename T>
struct point {
    T x, y;
    point() : x(0), y(0) { }
    point(const T _x, const T _y)
        : x(_x), y(_y) { }

    bool operator== (const point& secondPt) const {
        return (x == secondPt.x &&
                y == secondPt.y);
    }

    friend std::ostream& operator<< (std::ostream &o, const point &c) {
        return o << "(" << c.x << "," << c.y << ")";
    }
};

template <typename T>
struct point3 {
    T x, y, z;
    point3() : x(0), y(0), z(0) { }
    point3(const T _x, const T _y, const T _z)
        : x(_x), y(_y), z(_z) { }
    //  http://www.daniweb.com/forums/thread69005.html
    friend std::ostream& operator<< (std::ostream &o, const point3 &c)
        {
            return o << "(" << c.x << "," << c.y << "," << c.z << ")";
        }
};

// Structure for the bounding box we draw around lines
static const int NUM_LINES_IN_BOUNDING_BOX = 4;
struct BoundingBox {
    point <int> corners[NUM_LINES_IN_BOUNDING_BOX];
};

struct rectangle {
    int left;
    int right;
    int top;
    int bottom;
};

struct plane {
    int y_intercept;
    point <int> pnt;
    double slope;
};

struct segment {
    double y_intercept;
    point <int> start;
    point <int> end;
    double slope;
};

struct scanRun {
    int scanline;
    point <int> start;
    point <int> end;
    int h;
};

struct scanBlob {
    int start_scan_line;
    int end_scan_line;
    point <int> leftBottom;
    point <int> leftTop;
    point <int> rightBottom;
    point <int> rightTop;
};

struct scanline {
    int h;
    double y_intercept;
    int bottom_x;
    int bottom_y;
    int top_x;
    int top_y;
    double slope;
};

struct post {
    int topLeftX;
    int topLeftY;
    int topRightX;
    int topRightY;
    int bottomLeftX;
    int bottomLeftY;
    int bottomRightX;
    int bottomRightY;
    int width;
    int height;
};

struct goalBlob {
    int leftX;
    int rightX;
    int maxH;
    int maxHx;
    int area;
    bool bad;
};

struct segmentBlob {
    point <int> leftBottom;
    point <int> leftTop;
    point <int> rightBottom;
    point <int> rightTop;
    segment left; // always going to be perpen slope
    segment top; // horizon slope
    segment right; // perpen slope
    segment bottom; // horizon slope
    int start_scan_line;
    int end_scan_line;
    int density;
};

struct estimate {
    // Distance in cm from point to robot's center
    cms dist;
    angle::radians elevation;
    angle::radians bearing;
    // Field coordinate X, relative to robot's body
    cms x;
    // Field coordinate Y, relative to robot's body
    cms y;
    // Distance variance
    float distance_variance;
    // Bearing variance
    float bearing_variance;

    estimate() :
        dist(0.0f), elevation(0.0f), bearing(0.0f), x(0.0f), y(0.0f),
        distance_variance(0.0f), bearing_variance(0.0f)
        {}

    friend std::ostream& operator<< (std::ostream &o, const estimate &e)
        {
            return o << "Distance: " << e.dist << ", Elevation: " << e.elevation 
                     << ", Bearing: " << e.bearing << "(" << e.x << ", " << e.y << ")";
        }
};

const estimate NULL_ESTIMATE;

struct fieldOpening {
    int soft;
    int hard;
    int horizonDiffSoft;
    int horizonDiffHard;
    float dist;
    float elevation;
    float bearing;
};
#endif // Structs_h_DEFINED

