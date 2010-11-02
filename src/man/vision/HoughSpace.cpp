#include "HoughSpace.h"

using namespace std;
using boost::shared_ptr;

HoughSpace::HoughSpace() : acceptThreshold(DEFAULT_ACCEPT_THRESH),
                           angleSpread(DEFAULT_ANGLE_SPREAD)
{

}

/**
 * The main public interface for the HoughSpace class.
 * Finds all the lines in the image using the Hough Transform.
 */
list<HoughLine> HoughSpace::findLines(shared_ptr<Gradient> g)
{
    reset();
    markEdges(g);
    smooth();
    list<HoughLine> lines = peaks();
    suppress(lines);
    return lines;
}

/**
 * Pass through the given Gradient and mark all potential edges
 * in the accumulator.
 */
void HoughSpace::markEdges(shared_ptr<Gradient> g)
{
    const int height = g->rows;
    const int width  = g->cols;
    const int x0     = g->cols/2;
    const int y0     = g->rows/2;

    for (int y = 0; y < height; ++y){
        for (int x = 0; x < width; ++x){
            if (g->peaks[y][x] > 0){
                int t = Gradient::dir(g->y[y][x], g->x[y][x]);
                edge(x - x0, y - y0,
                     t - angleSpread,
                     t + angleSpread);
            }
        }
    }

}

/**
 * Marks an edge point in the source gradient as an edge in the Hough
 * accumulator
 */
void HoughSpace::edge(int x, int y, int t0, int t1)
{
    int r0 = getR(x, y, t0);
    for (int t=t0; t <= t1; ++t){
        int t8 = t & 0xff;
        int r1 = getR(x, y, t8 + 1);
        // cout << "t8: " << t8 << "\tr0: " << r0 << "\tr1: " << r1 << endl;

        for (int r = min(r0, r1); r <= max(r0, r1); ++r){
            int ri = r + R_SPAN / 2;
            // cout <<"\t\tri: " <<ri<<endl;
            if (0 <=ri && ri <R_SPAN){
                ++hs[ri][t8];
            }
        }
        r0 = r1;
    }
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
    // Make a copy of the row t=0 at t=T_SPAN. t=0 and t=T_SPAN-1 are
    // neighbors in the cylindrical Hough Space, but t=0 gets written
    // over before T=T_SPAN - 1 is smoothed so we copy it now.
    for (int r=0; r < R_SPAN; ++r) {
        hs[r][T_SPAN] = hs[r][0];
    }

    // In-place 2x2 boxcar smoothing
    for (int r=0; r < R_SPAN-1; ++r) {
        for (int t=0; t < T_SPAN; ++t) {
            hs[r][t] = hs[r][t] + hs[r + 1][t] +
                hs[r][t + 1] + hs[r + 1][t + 1];
        }
    }
}

/**
 * Find the peaks of the accumulator and create the list of lines in the space.
 */
list<HoughLine> HoughSpace::peaks()
{

}

/**
 * Combine/remove duplicate lines and lines which are not right.
 */
void HoughSpace::suppress(list<HoughLine>& lines)
{

}

/**
 * Reset the accumulator and peak arrays to their initial values.
 */
void HoughSpace::reset()
{

}
