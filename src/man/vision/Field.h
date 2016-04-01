#ifndef Field_h_DEFINED
#define Field_h_DEFINED

#include <boost/shared_ptr.hpp>
#include "Homography.h"
#include "Point.h"
#include "RansacTypes.h"
#include "Ransac.h"
#include "Vision.h"

namespace man {
namespace vision {
	class Field;  // forward reference
}
}

//#include "VisionModule.h"

//#include "Threshold.h"
//#include "NaoPose.h"

namespace man {
namespace vision {

// NEWVISION
#define IMAGE_WIDTH 320  // needs to be adjustable in NEWVISION
#define IMAGE_HEIGHT 240
#define BLACK 1
#define BLUE 7
#define MAROON 8
#define WHITE 2
#define GREEN 6
#define YELLOW 5
#define RED 3
#define ORANGE 4

// constants for Graham scanning to find convex hull
static const int RUNSIZE = 8;
static const int SCANSIZE = 10;
static const int SCANNOISE = 2;
static const int HULLS = IMAGE_WIDTH / SCANSIZE + 1;


class Field
{
    friend class VisionModule;
public:
    Field(int w, int h, FieldHomography *hom);
    virtual ~Field() {
	}

    // main methods
	void setDebugImage(DebugImage * di);
	void setImages(ImageLiteU8 white, ImageLiteU8 green, ImageLiteU8 orange,
		ImageLiteU16 yImg);
	void getColor(int x, int y);
   	bool isGreen();
	bool isWhite();
	bool isOrange();
	bool isNavy();
	bool isUndefined();
	float getPixDistance(int x);
	void drawPoint(int x, int y, int c);
	void drawDot(int x, int y, int c);
	void drawLine(int x, int y, int x1, int y1, int c);

	void findFieldEdge(int pH);
	void findFieldEdgeLines(unsigned int * seed);
    // Returns true if x is WITHIN SCOPE convex hull and sets y
    //  to height of hull at that x. (True could still be off field)
	bool onField(double x, double & y);
    int findGreenHorizon(int pH, int rH);
    void findConvexHull(int pH);
    void initialScanForTopGreenPoints(int pH);
    void findTopEdges(int M);
    int getInitialHorizonEstimate(int pH);
    int getImprovedEstimate(int pH);
	int horizonAt(int x);
    float horizonDist() { return getPixDistance(horizonAt(IMAGE_WIDTH / 2)); }
	int blockHorizonAt(int x);
	int occludingHorizonAt(int x);
	float distanceToHorizon(int x, int y);
	float fieldEdgeDistanceCenter();
	int ccw(point<int> p1, point<int> p2, point<int> p3);
    int * getTopEdge(){
        return topEdge;
    }
	int getPeak() {return peak;}
	int findSlant();

    // scan operations
    int yProject(int startx, int starty, int newy);
    int yProject(point <int> point, int newy);
    int xProject(int startx, int starty, int newx);
    int xProject(point <int> point, int newx);

    void drawLess(int x, int y, int c);
    void drawMore(int x, int y, int c);

	inline int ROUND2(float x) {
		return static_cast<int>(std::floor(x + 0.5f) );
	}

#ifdef OFFLINE
	void setDebugHorizon(bool debug) {debugHorizon = debug;}
	void setDebugFieldEdge(bool debug) {debugFieldEdge = debug;}
	void setDrawFieldHorizon(bool debug) {debugDrawFieldEdge = debug;}
	void setDrawCameraHorizon(bool debug) {drawCameraHorizon = debug;}
#endif

private:
	// NEWVISION
	bool topCamera;
	DebugImage debugDraw;
	ImageLiteU8 whiteImage, greenImage, orangeImage;
	ImageLiteU16 yImage;
	int currentX, currentY;
	FieldHomography * homography;
	int width, height;

	std::vector<PointI> edgePointsTop;

	// the field horizon
	int horizon;
	int poseHorizon;
	float slope;
	int peak;
	int numberOfHulls;

	int  topEdge[IMAGE_WIDTH+1];
	int topBlock[IMAGE_WIDTH+1];
	int greenTops[HULLS];
    point<int> convex[HULLS];
	point<int> blockages[HULLS];
	point<double> convexWorld[HULLS];
#ifdef OFFLINE
    bool debugHorizon;
    bool debugFieldEdge;
	bool debugDrawFieldEdge;
	bool drawCameraHorizon;
	bool debugRansac;
#else
    static const bool debugHorizon = false;
    static const bool debugFieldEdge = false;
	static const bool debugDrawFieldEdge = false;
	static const bool drawCameraHorizon = false;
	static const bool debugRansac = false;
#endif
};

}
}

#endif // Field_h_DEFINED
