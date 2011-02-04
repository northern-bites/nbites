#ifndef EdgeDetector_h_DEFINED
#define EdgeDetector_h_DEFINED

#include "VisionDef.h"
#include "Gradient.h"
#include "Profiler.h"
#include <stdint.h>

#include "boost/shared_ptr.hpp"

extern "C" void _sobel_operator(const uint8_t thresh,
                                const uint16_t *input,
                                uint16_t *out);

/**
 * Used to find the step edges in a given channel of an image according to a
 * preset, fixed threshold.
 */
class EdgeDetector
{
public:
    EdgeDetector(boost::shared_ptr<Profiler> p, int thresh);
    virtual ~EdgeDetector(){ };

/**
 * Public interface
 */
public:
    void detectEdges(const uint16_t* channel,
                     boost::shared_ptr<Gradient> gradient);

    int  getThreshold()           { return threshold; }
    void setThreshold(int thresh) { threshold = thresh; }

private:
    void sobelOperator(const uint16_t* channel,
                       boost::shared_ptr<Gradient> gradient);
    void findPeaks(boost::shared_ptr<Gradient> gradient);

private:
    boost::shared_ptr<Profiler> profiler;
    int threshold;
};

#endif /* EdgeDetector_h_DEFINED */
