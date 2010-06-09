#ifndef Field_h_DEFINED
#define Field_h_DEFINED

#include <boost/shared_ptr.hpp>

class Field;  // forward reference

#include "Threshold.h"
#ifndef NO_ZLIB
#include "Zlib.h"
#endif
#include "Profiler.h"
#include "NaoPose.h"
class Field
{
    friend class Vision;
public:
    Field(Vision* vis, Threshold* thr);
    virtual ~Field() {}

    // main methods
    int findGreenHorizon(int pH, float sl);
	void findConvexHull(int pH);
	int horizonAt(int x);
	int ccw(point<int> p1, point<int> p2, point<int> p3);

    // scan operations
    int yProject(int startx, int starty, int newy);
    int yProject(point <int> point, int newy);
    int xProject(int startx, int starty, int newx);
    int xProject(point <int> point, int newx);

    void bestShot(VisualFieldObject * left, VisualFieldObject * right,
                  VisualCrossbar * mid, int color);
    void openDirection(int h, NaoPose *p);
    void drawLess(int x, int y, int c);
    void drawMore(int x, int y, int c);

private:

    // class pointers
    Vision* vision;
	Threshold* thresh;

	// the field horizon
	int horizon;
	bool debugHorizon;
	bool debugFieldEdge;
	bool debugShot;
	bool openField;

	float slope;

    bool shoot[IMAGE_WIDTH];
	int  topEdge[IMAGE_WIDTH+1];
};

#endif // Field_h_DEFINED
