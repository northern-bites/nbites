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
    HoughSpace(boost::shared_ptr<Profiler> p);
    virtual ~HoughSpace() { };

    std::list<HoughLine> findLines  (boost::shared_ptr<Gradient> g);
    static HoughLine     createLine (int r, int t, int z);


    void setAcceptThreshold(int t) { acceptThreshold = t;    }
    void setAngleSpread(int t)     { angleSpread     = t;    }

    int  getAngleSpread()          { return angleSpread;     }
    int  getAcceptThreshold()      { return acceptThreshold; }

    bool isPeak(int r, int t);
    uint16_t getHoughBin(int r, int t);

private:                        // Member functions
    void markEdges(boost::shared_ptr<Gradient> g);
    void edge(int x, int y, int t0, int t1);
    int getR(int x, int y, int t);

    void smooth();
    void peaks();
    void createLinesFromPeaks(ActiveArray<HoughLine>& lines);
    void suppress(int x0, int y0, ActiveArray<HoughLine>& lines);

    void reset();

    inline void addPeak(int r, int t, int z);

    inline int getPeakR(int i) { return peak[i * peak_values + r_peak_offset]; }
    inline int getPeakT(int i) { return peak[i * peak_values + t_peak_offset]; }
    inline int getPeakZ(int i) { return peak[i * peak_values + z_peak_offset]; }

private:       // Member variables
    // Hough Space size parameters
    // 256 for full 8 bit angle, radius is for 320x240 image
    enum { r_span = 320,
           t_span = 256,

           // 5 rows on either side of the hough space to account for
           // angle wrap around in edge marking.
           hs_t_dim = t_span+10,
           first_smoothing_row = 3,
           first_peak_row = first_smoothing_row + 1,

           default_accept_thresh = 43,
           default_angle_spread  = 5,
           peak_points = 4 };

    enum { r_peak_offset = 0,
           t_peak_offset = 1,
           z_peak_offset = 2,
           peak_values = 3,
           hough_max_peaks = r_span * t_span / 4 };

    boost::shared_ptr<Profiler> profiler;
    int acceptThreshold, angleSpread, numPeaks;

    uint16_t hs[hs_t_dim][r_span];
    uint16_t peak[hough_max_peaks*peak_values];

    const static int drTab[peak_points];
    const static int dtTab[peak_points];

};

#endif /* HoughSpace_h_DEFINED */
