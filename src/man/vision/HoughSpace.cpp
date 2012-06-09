#include "HoughSpace.h"
#include <stdio.h>
#include <climits>

using namespace std;
using boost::shared_ptr;

namespace HC = HoughConstants;

const int HoughSpace::drTab[HC::peak_points] = {  1,  1,  0, -1 };
const int HoughSpace::dtTab[HC::peak_points] = {  0,  1,  1,  1 };

extern "C" void _mark_edges(int numPeaks, int angleSpread,
                            AnglePeak *peaks, uint16_t *houghSpace);
extern "C" void _smooth_hough(uint16_t *hs, uint32_t threshold);

extern "C" void _houghMain(uint16_t* hs,
                           AnglePeak* edges, int numEdges);

HoughSpace::HoughSpace() :
    acceptThreshold(HC::default_accept_thresh),
    angleSpread(HC::default_angle_spread),
    numPeaks(0), activeLines(HC::active_line_buffer)
{

}

list<pair<HoughLine, HoughLine> >
HoughSpace::findLines(Gradient& g)
{
    PROF_ENTER(P_HOUGH);
    reset();
    findHoughLines(g);
    list<pair<HoughLine, HoughLine> > lines = narrowHoughLines();

    PROF_EXIT(P_HOUGH);
    return lines;
}

void HoughSpace::findHoughLines(Gradient& g)
{
    markEdges(g);
    smooth();
    peaks();
    createLinesFromPeaks(activeLines);
}

list<pair<HoughLine, HoughLine> > HoughSpace::narrowHoughLines()
{
    int x0 = static_cast<int>(Gradient::cols/2);
    int y0 = static_cast<int>(Gradient::rows/2);

    suppress(x0, y0, activeLines);
    list<pair<int, int> > pairs = pairLines(activeLines);

    list<pair<HoughLine, HoughLine> > lines;
    list<pair<int, int> >::iterator i;
    for (i = pairs.begin(); i != pairs.end(); ++i){
        lines.push_back(pair<HoughLine, HoughLine>(activeLines[(*i).first],
                                                   activeLines[(*i).second]));
    }
    PROF_EXIT(P_HOUGH);
    return lines;
}

void HoughSpace::markEdges(Gradient& g)
{
    PROF_ENTER(P_MARK_EDGES);
#ifdef USE_MMX
    if (g.numPeaks > 0){
        // _mark_edges(g.numPeaks, angleSpread, g.angles, &hs[0][0]);

        _houghMain(&hs[0][0], g.angles, g.numPeaks);
    }
#else
    // See comment in FindPeaks re: why this is shrunk in by 2
    // rows/columns on each side
    for (int i = 0; g.isPeak(i); ++i) {
        edge(g.getAnglesXCoord(i),
             g.getAnglesYCoord(i),
             g.getAngle(i) - angleSpread,
             g.getAngle(i) + angleSpread);
    }
#endif /* USE_MMX */
    PROF_EXIT(P_MARK_EDGES);
}

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

int HoughSpace::getR(int x, int y, int t)
{
    float a = static_cast<float>(t & 0xff) * M_PI_FLOAT / 128.0f;
    return static_cast<int>(floor(static_cast<float>(x) * cos(a) +
                                  static_cast<float>(y) * sin(a)));
}

void HoughSpace::smooth()
{
    PROF_ENTER(P_SMOOTH);

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

    PROF_EXIT(P_SMOOTH);
}

void HoughSpace::peaks()
{
    PROF_ENTER(P_HOUGH_PEAKS);
    for (uint16_t t = HC::first_peak_row;
         t < HC::t_span + HC::first_peak_row;
         ++t) {

        // First and last columns are not accurate, so they shouldn't
        // be queried, so we skip to third row as first possible peak
        for (uint16_t r=2; r < HC::r_span-2; ++r) {

            const uint16_t z = getHoughBin(r,t);
            if (z){
                for (int i=0; i < HC::peak_points; ++i) {

                    if ( ! ( z >  getHoughBin(r + drTab[i],
                                              (t + dtTab[i])) &&
                             z >= getHoughBin(r - drTab[i],
                                              (t - dtTab[i])))) {
                        goto notALine;
                    }
                }
#ifdef USE_MMX
                addPeak(r, static_cast<uint16_t>(t - HC::first_peak_row), z);
#else
                addPeak(r, t, z);
#endif
            }
        notALine:
            continue;
        }
    }
    PROF_EXIT(P_HOUGH_PEAKS);
}

void HoughSpace::createLinesFromPeaks(ActiveArray<HoughLine>& lines)
{
    for (int i=0; i < numPeaks; ++i){
        HoughLine line(getPeakRadius(i),
                       getPeakAngle(i),
                       getPeakCount(i) );

        lines.add(line);
    }
}

void HoughSpace::suppress(int x0, int y0, ActiveArray<HoughLine>& lines)
{
    PROF_ENTER(P_SUPPRESS);
    const int size = lines.size();
    bool toDelete[size];

    for (int i = 0; i < size; ++i) {
        toDelete[i] = false;
    }

    for(int i=0; i < size; i++){

        // We don't need to keep checking if this one is already
        // going to be deleted
        if (toDelete[i]){
            continue;
        }

        for (int j = i+1; j < size; j++){

            if (toDelete[j]){
                continue;
            }
            const int tDiff = abs(((lines[i].getTIndex() -
                                    lines[j].getTIndex()) & 0xff)
                                  << 24 >> 24);

            // Since the lines are ordered by T value, if the tDiff is
            // too great, we should stop going any further with the
            // current line. This keeps us from looking at every pair
            // of lines, every time.
            if (tDiff > angleSpread+1){
                break;
            } else if (tDiff > angleSpread+1){
                continue;
            }

            const int rDiff = abs(lines[i].getRIndex() -
                                  lines[j].getRIndex());

            if ( (rDiff <= HC::suppress_r_bound ||
                  HoughLine::intersect(x0, y0, lines[i], lines[j]))) {

                if (lines[i].getScore() < lines[j].getScore()){
                    toDelete[i] = true;
                } else {
                    toDelete[j] = true;
                }
            }
        }
    }

    for(int i=0; i < size; i++){
        if (toDelete[i]){
            lines.deactivate(i);
        }
    }
    PROF_EXIT(P_SUPPRESS);
}

list<pair<int, int> > HoughSpace::pairLines(ActiveArray<HoughLine>& lines)
{
    PROF_ENTER(P_PAIR_LINES);
    list<pair<int, int> > pairs;
    const int size = lines.size();

    // The array which holds the partner line for each HoughLine
    int pair_array[size];

    // The minimum r distance found for a partner line for each HoughLine
    int min_pair_r[size];

    const static int NO_PARTNER = -1;

    // Init arrays
    for(int i=0; i < size; ++i){
        pair_array[i] = NO_PARTNER;
        min_pair_r[i] = INT_MAX;
    }

    for(int i=0; i < size; ++i){
        if(!lines.active(i)){
            continue;
        }

        // This inner loop look awkward, yes. I'm avoiding
        // computation of tDiff and rSum by exiting early.
        for(int j=i+1; j < size; ++j){

            if (!lines.active(j)){
                continue;
            }

            const int tDiff = abs(abs(lines[i].getTIndex() -
                                      lines[j].getTIndex()) - HC::t_span/2);

            // The lines are in order by T, so they won't be any
            // closer after this
            if (tDiff >= HC::opp_line_thresh){
                continue;
            }

            const int rSum = abs(lines[i].getRIndex() +
                                 lines[j].getRIndex() - HC::r_span);
            if (rSum < min_pair_r[i] &&
                rSum < min_pair_r[j]){

                // Unset previous line pairs
                if (pair_array[i] != NO_PARTNER){
                    pair_array[pair_array[i]] = NO_PARTNER;
                }

                if (pair_array[j] != NO_PARTNER){
                    pair_array[pair_array[j]] = NO_PARTNER;
                }

                pair_array[i] = j;
                pair_array[j] = i;

                min_pair_r[i] = min_pair_r[j] = rSum;
            }
        }
    }

    for(int i=0; i < size; ++i){
        // If this line hasn't been paired up with a line after it,
        // this keeps us from duplicating line pairs.
        if (pair_array[i] < i){ // pair_array == -1 when no partner was found
            continue;
        }

        pairs.push_back(pair<int,int>(i, pair_array[i]));
    }
    PROF_EXIT(P_PAIR_LINES);
    return pairs;
}

void HoughSpace::reset()
{
    numPeaks = 0;
    activeLines.clear();

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

uint16_t HoughSpace::getHoughBin(int r, int t)
{
    return hs[t][r];
}

void HoughSpace::addPeak(uint16_t r, uint16_t t, uint16_t z)
{
    HoughPeak& p = peak[numPeaks];
    numPeaks++;

    p.r = r;
    p.t = t;
    p.z = z;
}
