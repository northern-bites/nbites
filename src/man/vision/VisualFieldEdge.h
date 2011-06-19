#ifndef VisualFieldEdge__hpp
#define VisualFieldEdge__hpp

#include <stdlib.h>
#include <math.h>
#include <boost/shared_ptr.hpp>

using namespace std;

#define UNKNOWN_DISTANCE  -1.0f

class VisualFieldEdge;

enum Basic_Shape {
    RISING_RIGHT,
	RISING_LEFT,
	FLAT,
	WEDGE
};

struct fieldEdgeDistances {
	float left;
	float center;
	float right;
};


#include "Structs.h"
#include "Utility.h"

class VisualFieldEdge  {

public:
    VisualFieldEdge();
    ~VisualFieldEdge(){}

private: // Private member variables
	fieldEdgeDistances distances;

public:
    // Getters
    const fieldEdgeDistances getDistances() const { return distances; }
	const float getDistanceLeft() const { return distances.left; }
	const float getDistanceCenter() const { return distances.center; }
	const float getDistanceRight() const { return distances.right; }
	float getMinimumDistance() { return min(distances.right, min(distances.center,
															   distances.left)); }
	float getMaxDistance() {return max(distances.right, max(distances.center,
															   distances.left)); }

	Basic_Shape getShape();

    // Setters
    void setDistances(float d1, float d2, float d3) { distances.left = d1;
		distances.center = d2; distances.right = d3;}
};
#endif
