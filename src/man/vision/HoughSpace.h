#ifndef HoughSpace_h_DEFINED
#define HoughSpace_h_DEFINED

#include <list>
#include <boost/shared_ptr.hpp>

#include "Gradient.h"
#include "geom/HoughLine.h"
#include "Profiler.h"
#include "visionconfig.h"

/**
 * The accumulator space and associated functions for performing
 * the Hough Transform on a given set of gradient edge points.
 *
 * Returns the lines found in the image.
 */
class HoughSpace
{
public:
    HoughSpace(boost::shared_ptr<Profiler> p);
    virtual ~HoughSpace() { };

    std::list<HoughLine> findLines  (boost::shared_ptr<Gradient> g);
    static HoughLine     createLine (int r, int t, int z);


    void setAcceptThreshold(int t) { acceptThreshold = t;    }
    void setAngleSpread(int t)     { angleSpread     = t;    }

    int  getAngleSpread()          { return angleSpread;     }
    int  getAcceptThreshold()      { return acceptThreshold; }

    bool isPeak(int r, int t);
    int  getHoughBin(int r, int t);

private:                        // Member functions
    void markEdges(boost::shared_ptr<Gradient> g);
    void edge(int x, int y, int t0, int t1);
    int getR(int x, int y, int t);

    void smooth();
    std::list<HoughLine> peaks();
    void suppress(int x0, int y0, std::list<HoughLine>& lines);

    void reset();


private:       // Member variables
    // Hough Space size parameters
    // 256 for full 8 bit angle, width is for 320x240 image
    enum { r_span = 400,
           t_span = 256,
           default_accept_thresh = 43,
           default_angle_spread  = 5,
           peak_points = 4 };

    boost::shared_ptr<Profiler> profiler;
    int acceptThreshold, angleSpread;

    // allocate an extra T for the smoothing neighborhood
#ifdef USE_MMX
    // arranged opposite the non-asm version, rows are theta, cols are radius
    uint16_t hs[(r_span+1) * t_span];
    // bool peak[r_span * t_span];
#else
    int hs[r_span][t_span + 1];
#endif
    bool peak[r_span][t_span];

    const static int drTab[peak_points];
    const static int dtTab[peak_points];

};

#endif /* HoughSpace_h_DEFINED */
