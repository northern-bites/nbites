#ifndef EdgeDetector_h_DEFINED
#define EdgeDetector_h_DEFINED

#include "VisionDef.h"
#include "Gradient.h"
#include "Profiler.h"
#include <stdint.h>

#include "boost/shared_ptr.hpp"

/**
 * Used to find the step edges in a given channel of an image according to a
 * preset, fixed threshold.
 */
class EdgeDetector
{
public:
    EdgeDetector(uint8_t thresh = default_edge_value);
    virtual ~EdgeDetector(){ };

/**
 * Public interface
 */
public:
    void detectEdges(const uint16_t* channel,
                     boost::shared_ptr<Gradient> gradient);

    uint8_t  getThreshold()           { return threshold; }
    void setThreshold(uint8_t thresh) { threshold = thresh; }

private:
    void sobelOperator(const uint16_t* channel,
                       boost::shared_ptr<Gradient> gradient);
    void findPeaks(boost::shared_ptr<Gradient> gradient);

    enum {
        default_edge_value = 30
    };

private:
    uint8_t threshold;
};

#endif /* EdgeDetector_h_DEFINED */
