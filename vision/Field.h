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
    int findGreenHorizon(int pH);
	void findFieldEdges();

private:

    // class pointers
    Vision* vision;
	Threshold* thresh;

	// the field horizon
	int horizon;
	bool debugHorizon;
	bool debugFieldEdge;
};

#endif // Field_h_DEFINED
