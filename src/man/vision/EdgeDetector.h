#ifndef EdgeDetector_h_DEFINED
#define EdgeDetector_h_DEFINED

#include "VisionDef.h"
#include "Gradient.h"

#include "boost/shared_ptr.hpp"


class EdgeDetector
{
public:
    EdgeDetector(int thresh);
    virtual ~EdgeDetector(){ };

/**
 * Public interface
 */
public:
    void detectEdges(const Channel& channel,
                     boost::shared_ptr<Gradient> gradient);

    int  getThreshold()           { return threshold; }
    void setThreshold(int thresh) { threshold = thresh; }

private:
    void sobelOperator(const Channel& channel,
                       boost::shared_ptr<Gradient> gradient);
    void findPeaks(boost::shared_ptr<Gradient> gradient);

private:
    // Tables that specify the + neighbor of a pixel indexed by
    // gradient direction octant (the high 3 bits of direction).
    const static int DIRECTIONS = 8;
    const static int dxTab[DIRECTIONS];
    const static int dyTab[DIRECTIONS];

    int threshold;
};

#endif /* EdgeDetector_h_DEFINED */
