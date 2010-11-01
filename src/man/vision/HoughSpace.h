#ifndef HoughSpace_h_DEFINED
#define HoughSpace_h_DEFINED

#include <list>
#include <boost/shared_ptr.hpp>

#include "Gradient.h"
#include "geom/HoughLine.h"


/**
 * The accumulator space and associated functions for performing
 * the Hough Transform on a given set of gradient edge points.
 *
 * Returns the lines found in the image.
 */
class HoughSpace
{
public:
    HoughSpace();
    virtual ~HoughSpace();

    std::list<HoughLine> findLines(boost::shared_ptr<Gradient> g);

private:
    // Hough Space size parameters
    // 256 for full 8 bit angle, width is for 320x240 image
    const static int R_SPAN = 400, T_SPAN  = 256;
    const static int DEFAULT_ACCEPT_THRESH = 20;
    const static int DEFAULT_ANGLE_SPREAD  = 5;

    int acceptThreshold, angleSpread;

    // allocate an extra T for the smoothing neighborhood
    int hs[R_SPAN][T_SPAN + 1];
    bool peak[R_SPAN][T_SPAN];

    void markEdges(boost::shared_ptr<Gradient> g);
    void smooth();
    void peaks();
    void suppress();

    void reset();
};

#endif /* HoughSpace_h_DEFINED */
