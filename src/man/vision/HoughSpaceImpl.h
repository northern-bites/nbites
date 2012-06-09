#include "HoughSpace.h"

class HoughSpaceImpl : public HoughSpace
{
public:
    HoughSpaceImpl();
    virtual ~HoughSpaceImpl() {};

    virtual std::list<std::pair<HoughLine, HoughLine> >
    findLines(Gradient& g);
    virtual uint16_t getHoughBin(int r, int t);

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

    /** Holds the two indices of a point in the HoughSpace and the
     * count at that location */
    struct HoughPeak {
        uint16_t r;
        uint16_t t;
        uint16_t z;
    };

    ActiveArray<HoughLine> activeLines;

#ifdef USE_MMX
    uint16_t hs[HoughConstants::hs_t_dim][HoughConstants::r_span];
#else
    uint16_t hs[HoughConstants::t_span+1][HoughConstants::r_span];
#endif
    HoughPeak peak[HoughConstants::hough_max_peaks];

    const static int drTab[HoughConstants::peak_points];
    const static int dtTab[HoughConstants::peak_points];

    int numPeaks;
};
