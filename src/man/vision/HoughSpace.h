#ifndef HoughSpace_h_DEFINED
#define HoughSpace_h_DEFINED

#include <list>
#include <boost/shared_ptr.hpp>

#include "Gradient.h"
#include "geom/HoughLine.h"
#include "Profiler.h"
#include "visionconfig.h"
#include "ActiveArray.h"

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
    virtual ~HoughSpace() { };

    std::list<std::pair<HoughLine, HoughLine> >
    findLines(Gradient& g);

    void setAcceptThreshold(int t) { acceptThreshold = t;    }
    void setAngleSpread(int t)     { angleSpread     = t;    }

    int  getAngleSpread()          { return angleSpread;     }
    int  getAcceptThreshold()      { return acceptThreshold; }

    uint16_t getHoughBin(int r, int t);

private:                        // Member functions
    void findHoughLines(Gradient& g);
    std::list<std::pair<HoughLine, HoughLine> > narrowHoughLines();

    void markEdges(Gradient& g);
    void edge(int x, int y, int t0, int t1);
    int  getR(int x, int y, int t);

    void smooth();
    void peaks();
    void createLinesFromPeaks(ActiveArray<HoughLine>& lines);
    void suppress(int x0, int y0, ActiveArray<HoughLine>& lines);
    std::list<std::pair<int, int> > pairLines(ActiveArray<HoughLine>& lines);

    void reset();

    inline void addPeak(uint16_t r, uint16_t t, uint16_t z);

    inline int getPeakR(int i) { return peak[i].r; }
    inline int getPeakT(int i) { return peak[i].t; }
    inline int getPeakZ(int i) { return peak[i].z; }

private:       // Member variables
    friend class HoughLine;     // Hough Line needs the HoughSpace private vars

    enum {
        // Hough Space size parameters
        // 256 for full 8 bit angle, radius is for 320x240 image
        r_span = 320,
        t_span = 256,

        // 5 rows on either side of the hough space to account for
        // angle wrap around in edge marking.
        hs_t_dim = t_span+10,
#ifdef USE_MMX
        first_smoothing_row = 3,
#else
        first_smoothing_row = 0,
#endif
        first_peak_row = first_smoothing_row + 1,

        default_accept_thresh = 43,
        default_angle_spread  = 5,
        peak_points = 4,
        hough_max_peaks = r_span * t_span / 4,
        active_line_buffer = 200,
        opp_line_thresh = 5,
        suppress_r_bound = 4};

    struct HoughPeak {
        uint16_t r;
        uint16_t t;
        uint16_t z;
    };

    int acceptThreshold, angleSpread, numPeaks;
    ActiveArray<HoughLine> activeLines;

#ifdef USE_MMX
    uint16_t hs[hs_t_dim][r_span];
#else
    uint16_t hs[t_span+1][r_span];
#endif
    HoughPeak peak[hough_max_peaks];

    const static int drTab[peak_points];
    const static int dtTab[peak_points];
};

#endif /* HoughSpace_h_DEFINED */
