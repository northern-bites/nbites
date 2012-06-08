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

    /**
     * The main public interface for the HoughSpace class.
     * Finds all the lines in the image using the Hough Transform.
     */
    std::list<std::pair<HoughLine, HoughLine> >
    findLines(Gradient& g);

    void setAcceptThreshold(int t) { acceptThreshold = t;    }
    void setAngleSpread(int t)     { angleSpread     = t;    }

    int  getAngleSpread()          { return angleSpread;     }
    int  getAcceptThreshold()      { return acceptThreshold; }

    uint16_t getHoughBin(int r, int t);

protected:

    /**
     * Locate the lines in the image using a Hough Transform
     */
    void findHoughLines(Gradient& g);

    /**
     * Process hough lines to eliminate duplicate lines and pair up
     * the lines.
     */
    std::list<std::pair<HoughLine, HoughLine> > narrowHoughLines();

    /**
     * Pass through the given Gradient and mark all potential edges in
     * the accumulator.
     */
    void markEdges(Gradient& g);

    /**
     * Marks an edge point in the source gradient as an edge in the
     * Hough accumulator
     */
    void edge(int x, int y, int t0, int t1);

    /**
     * Returns the radius of a line at the given location with the
     * given angle.
     */
    int  getR(int x, int y, int t);

    /**
     * Smooth out irregularities in the Hough accumulator to reduce
     * noisy peaks by using a 2x2 boxcar kernel.
     *
     * Boxcar kernel: (each element becomes the sum of four surrounding pixels)
     *       |1 1|
     *       |1 1|
     */
    void smooth();

    /**
     * Find the peaks of the accumulator and create the list of lines
     * in the space.
     */
    void peaks();

    /**
     * Using the list of peaks found in the Hough Transform, create line objects
     *
     * @param lines List to be filled with lines
     */
    void createLinesFromPeaks(ActiveArray<HoughLine>& lines);

    /**
     * Combine/remove duplicate lines and lines which are not right.
     */
    void suppress(int x0, int y0, ActiveArray<HoughLine>& lines);

    /**
     * Put the Hough lines into pairs of lines with opposite
     * angles. Opposite angles mean that the gradients on the lines are
     * headed into each other, as in the two edges of a field line.
     *
     * Each line should be paired with only one other line, that line
     * being the closest other parallel line to it. That way two parallel field
     * lines will not produce very strange results. If a line has no
     * parallel counterpart, it is discarded.
     *
     * @TODO Make sure lines are headed into or out of the line only. This
     *       would prevent inner parts of two lines from being paired.
     */
    std::list<std::pair<int, int> > pairLines(ActiveArray<HoughLine>& lines);

    /**
     * Reset the accumulator and peak arrays to their initial values.
     */
    void reset();

    inline void addPeak(uint16_t r, uint16_t t, uint16_t z);

    inline int getPeakRadius(int i) { return peak[i].r; }
    inline int getPeakAngle(int i) { return peak[i].t; }
    inline int getPeakCount(int i) { return peak[i].z; }

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

    /** Holds the two indices of a point in the HoughSpace and the
     * count at that location */
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
