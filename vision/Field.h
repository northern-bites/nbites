#ifndef Field_h_DEFINED
#define Field_h_DEFINED

#include <boost/shared_ptr.hpp>

class Field;  // forward reference

#include "Threshold.h"
#ifndef NO_ZLIB
#include "Zlib.h"
#endif
#include "Profiler.h"

class Field
{
    friend class Vision;
public:
    Field(Vision* vis, Threshold* thr);
    virtual ~Field() {}

    // main methods
    int findGreenHorizon(int pH, float sl);
	void findFieldEdges();
	int horizonAt(int x);

    // scan operations
    int yProject(int startx, int starty, int newy);
    int yProject(point <int> point, int newy);
    int xProject(int startx, int starty, int newx);
    int xProject(point <int> point, int newx);

    // shooting
    void setShot(VisualCrossbar * one, int color);
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
};

#endif // Field_h_DEFINED
