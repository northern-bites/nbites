#include "HoughSpace.h"
#include <stdio.h>


using namespace std;
using boost::shared_ptr;

const int HoughSpace::drTab[peak_points] = {  1,  1,  0, -1 };
const int HoughSpace::dtTab[peak_points] = {  0,  1,  1,  1 };

extern "C" void _mark_edges(int numPeaks, int angleSpread,
                            int16_t *peaks, uint16_t *houghSpace);
extern "C" void _smooth_hough(uint16_t *hs, uint32_t threshold);

extern "C" void _houghMain(uint16_t* hs, int16_t* edges, int numEdges);

HoughSpace::HoughSpace(shared_ptr<Profiler> p) :
    profiler(p),
    acceptThreshold(default_accept_thresh),
    angleSpread(default_angle_spread),
    numPeaks(0)
{

}

/**
 * The main public interface for the HoughSpace class.
 * Finds all the lines in the image using the Hough Transform.
 */
list<HoughLine> HoughSpace::findLines(shared_ptr<Gradient> g)
{
    PROF_ENTER(profiler, P_HOUGH);
    reset();

    markEdges(g);
    smooth();
    peaks();

    ActiveArray<HoughLine> lines = ActiveArray<HoughLine>(10);
    createLinesFromPeaks(lines);

    int x0 = static_cast<int>(Gradient::cols/2);
    int y0 = static_cast<int>(Gradient::rows/2);

    suppress(x0, y0, lines);

    PROF_EXIT(profiler, P_HOUGH);
    list<HoughLine> lines_list;
    for (int i=0; i < lines.size(); ++i){
        if (lines.active(i)){
            lines_list.push_back(lines[i]);
        }
    }
    return lines_list;
}

/**
 * Pass through the given Gradient and mark all potential edges
 * in the accumulator.
 */
void HoughSpace::markEdges(shared_ptr<Gradient> g)
{
    PROF_ENTER(profiler, P_MARK_EDGES);
#ifdef USE_MMX
    if (g->numPeaks > 0){
        _houghMain(&hs[0][0], g->angles, g->numPeaks);
    }
#else
    int x0 = Gradient::cols/2;
    int y0  = Gradient::rows/2;

    // See comment in FindPeaks re: why this is shrunk in by 2
    // rows/columns on each side
    for (int i = 0; g->isPeak(i); ++i) {
        edge(g->getAnglesXCoord(i),
             g->getAnglesYCoord(i),
             g->getAngle(i) - angleSpread,
             g->getAngle(i) + angleSpread);
    }
#endif /* USE_MMX */
    PROF_EXIT(profiler, P_MARK_EDGES);
}

/**
 * Marks an edge point in the source gradient as an edge in the Hough
 * accumulator
 */
void HoughSpace::edge(int x, int y, int t0, int t1)
{
#ifndef USE_MMX
    int r0 = getR(x, y, t0);
    for (int t=t0; t <= t1; ++t){
        int t8 = t & 0xff;
        int r1 = getR(x, y, t8 + 1);

        for (int r = min(r0, r1); r <= max(r0, r1); ++r){
            int ri = r + r_span / 2;
            if (0 <=ri && ri <r_span){
                ++hs[t8][ri];
            }
        }
        r0 = r1;
    }
#endif
}

/**
 * Returns the radius of a line at the given location with the given
 * angle.
 */
int HoughSpace::getR(int x, int y, int t)
{
    float a = static_cast<float>(t & 0xff) * M_PI_FLOAT / 128.0f;
    return static_cast<int>(floor(static_cast<float>(x) * cos(a) +
                                  static_cast<float>(y) * sin(a)));
}

/**
 * Smooth out irregularities in the Hough accumulator to reduce noisy
 * peaks by using a 2x2 boxcar kernel.
 *
 * Boxcar kernel: (each element becomes the sum of four surrounding pixels)
 *       |1 1|
 *       |1 1|
 */
void HoughSpace::smooth()
{
    PROF_ENTER(profiler, P_SMOOTH);

#ifdef USE_MMX
    _smooth_hough(&hs[0][0], acceptThreshold);
#else

    // In-place 2x2 boxcar smoothing
    for (int t=first_smoothing_row; t < t_span+first_smoothing_row; ++t) {
        for (int r=0; r < r_span-1; ++r) {

            hs[t][r] = static_cast<int16_t>(
                max((hs[t][r] + hs[t][r+1] + hs[t+1][r] + hs[t+1][r+1] -
                     getAcceptThreshold() * 4),
                    0)
                );
        }
    }
#endif

    PROF_EXIT(profiler, P_SMOOTH);
}

/**
 * Find the peaks of the accumulator and create the list of lines in the space.
 */
void HoughSpace::peaks()
{
    PROF_ENTER(profiler, P_HOUGH_PEAKS);

    for (int t=first_peak_row; t < t_span+first_peak_row; ++t) {

        // First and last columns are not accurate, so they shouldn't
        // be queried
        for (int r=2; r < r_span-2; ++r) {

            const uint16_t z = getHoughBin(r,t);
            if (z){
                for (int i=0; i < peak_points; ++i) {

                    if ( ! ( z >  getHoughBin(r + drTab[i],
                                              (t + dtTab[i])) &&
                             z >= getHoughBin(r - drTab[i],
                                              (t - dtTab[i])))) {
                        goto notALine;
                    }
                }
                addPeak(r, t - first_peak_row, z);
            }
        notALine:
            continue;
        }
    }

    PROF_EXIT(profiler, P_HOUGH_PEAKS);
}

/**
 * Using the list of peaks found in the Hough Transform, create line objects
 *
 * @param lines List to be filled with lines
 */
void HoughSpace::createLinesFromPeaks(ActiveArray<HoughLine>& lines)
{
    for (int i=0; i < numPeaks; ++i){
        HoughLine line = createLine(getPeakR(i),
                                    getPeakT(i),
                                    getPeakZ(i) );
        lines.add(line);
    }
}
/**
 * Combine/remove duplicate lines and lines which are not right.
 */
void HoughSpace::suppress(int x0, int y0, ActiveArray<HoughLine>& lines)
{
    PROF_ENTER(profiler, P_SUPPRESS);
    bool toDelete[lines.size()];

    for (int i = 0; i < lines.size(); ++i) {
        toDelete[i] = false;
    }

    int index = 0;
    while (index < lines.size()){

        int index2 = index;
        index2++;

        while (index2 < lines.size()){

            const int tDiff = abs(((lines[index].getTIndex() -
                                    lines[index2].getTIndex()) & 0xff)
                                  << 24 >> 24);
            const int rDiff = abs(lines[index].getRIndex() -
                                  lines[index2].getRIndex());

            if ( 0 < tDiff && tDiff <= angleSpread &&
                 (rDiff <= 4 ||
                  HoughLine::intersect(x0, y0, lines[index], lines[index2]))) {

                if (lines[index].getScore() < lines[index2].getScore()){
                    toDelete[index] = true;
                } else {
                    toDelete[index2] = true;
                }
            }
            index2++;
        }
        index++;
    }

    index = 0;
    while (index < lines.size()){
        if (toDelete[index]){
            lines.deactivate(index);
        }
        index++;
    }
    PROF_EXIT(profiler, P_SUPPRESS);
}

/**
 * Reset the accumulator and peak arrays to their initial values.
 */
void HoughSpace::reset()
{
    numPeaks = 0;
#ifdef USE_MMX
    // Array resetting done in edge marking
#else
    for (int t=0; t < t_span; ++t) {
        for (int r=0; r < r_span; ++r) {
            hs[t][r] = 0;
        }
    }
#endif
}

HoughLine HoughSpace::createLine(int r, int t, int z)
{
    return HoughLine(r, t,
                     static_cast<float>(r) -
                     r_span / 2.0f + 0.5f,
                     (static_cast<float>(t+.5)) *
                     M_PI_FLOAT / 128.0f, z >> 2);
}

// Check to see if the HoughSpace value at the given radius and theta
// is a peak.
bool HoughSpace::isPeak(int r, int t)
{
    return peak[t * r_span + r];
}

uint16_t HoughSpace::getHoughBin(int r, int t)
{
    return hs[t][r];
}


void HoughSpace::addPeak(int r, int t, int z)
{
    peak[numPeaks * peak_values + r_peak_offset] = static_cast<uint16_t>(r);
    peak[numPeaks * peak_values + t_peak_offset] = static_cast<uint16_t>(t);
    peak[numPeaks * peak_values + z_peak_offset] = static_cast<uint16_t>(z);
    numPeaks++;
}
