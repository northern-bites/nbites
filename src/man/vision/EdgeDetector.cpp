#include "EdgeDetector.h"
#include "visionconfig.h"

#include <cmath>
#include <iostream>
#include <stdio.h>

extern "C" void _sobel_operator(const uint8_t thresh,
                                const uint16_t *input,
                                uint16_t *out);
extern "C" void _find_edge_peaks(const uint16_t *gradients,
                                 uint16_t *angles);
using namespace std;

EdgeDetector::EdgeDetector(uint8_t thresh):
    threshold(thresh)
{

}

/**
 * Find the edges in a channel of an image.
 *
 * @param channel      The entire channel (one of Y, U, or V)
 */
void EdgeDetector::detectEdges(const uint16_t* channel,
                               boost::shared_ptr<Gradient> gradient)
{
    PROF_ENTER(P_EDGES);
    sobelOperator(channel, gradient);
    findPeaks(gradient);
    PROF_EXIT(P_EDGES);
}

/**
 * Apply the Sobel Operator to the given channel and fill a given struct
 * with the gradient information (x, y, absolute magnitude)
 *
 * / -1 0 +1 \   / -1 -2 -1 \
 * | -2 0 +2 |   |  0  0  0 |
 * \ -1 0 +1 /   \ +1 +2 +1 /
 *      Gx           Gy
 * @param channel     The channel with edges to be detected.
 * @param gradient    Gradient struct to be populated.
 */
void EdgeDetector::sobelOperator(const uint16_t* channel,
                                 boost::shared_ptr<Gradient> gradient)
{
    PROF_ENTER(P_SOBEL);
#ifdef USE_MMX
    _sobel_operator(threshold, &channel[0], gradient->values);
#else
    for (int i=1; i < Gradient::rows-1; ++i){
        for (int j=1; j < Gradient::cols-1; ++j) {

            int xGrad = (
                // Column j+1
                (channel[(i-1) * IMAGE_WIDTH + (j+1)] +
                 2 * channel[(i) * IMAGE_WIDTH + (j+1)] +
                 channel[(i+1) * IMAGE_WIDTH + (j+1)]) -
                // Column j-1
                (channel[(i-1) * IMAGE_WIDTH + (j-1)] +
                 2 * channel[(i) * IMAGE_WIDTH + (j-1)] +
                 channel[(i+1) * IMAGE_WIDTH + (j-1)]));

            int yGrad = (
                // Row i+1
                (channel[(i+1) * IMAGE_WIDTH + (j-1)] +
                 2 * channel[(i+1) * IMAGE_WIDTH + (j)] +
                 channel[(i+1) * IMAGE_WIDTH + (j+1)]) -
                // Row i -1
                (channel[(i-1) * IMAGE_WIDTH + (j-1)] +
                 2 * channel[(i-1) * IMAGE_WIDTH + (j)] +
                 channel[(i-1) * IMAGE_WIDTH + (j+1)])
                );

            xGrad = -xGrad;
            yGrad = -yGrad;

            gradient->setX(static_cast<int16_t>(xGrad), i, j);
            gradient->setY(static_cast<int16_t>(yGrad), i, j);

            xGrad = xGrad << 3;
            yGrad = yGrad << 3;

            xGrad = xGrad * xGrad;
            yGrad = yGrad * yGrad;

            xGrad = xGrad >> 16;
            yGrad = yGrad >> 16;

            int mag = xGrad + yGrad;

            // All non above threshold points are zero
            mag = max(0, mag-((threshold*threshold) >> 10));

            gradient->setMagnitude(static_cast<uint16_t>(mag), i, j);
        }
    }
#endif /* USE_MMX */
    PROF_EXIT(P_SOBEL);
}


/**
 * Run an asymmetric peak test over the given gradient array. Based on
 * the direction of the magnitude at a pixel, ensure that its gradient
 * is greater (>) than the magnitude of the point opposite its
 * gradient and greater than or equal to (>=) the magnitude of the
 * point in the direction of its gradient.
 *
 * Example:
 *     Magnitude Direction: to the right
 *     Test:
 *     - - -
 *     > - >=
 *     - - -
 *
 * @param gradient Gradient to check for points.
 */
void EdgeDetector::findPeaks(boost::shared_ptr<Gradient> gradient)
{
    PROF_ENTER(P_EDGE_PEAKS);
#ifdef USE_MMX
    _find_edge_peaks(gradient->values, gradient->angles);
#else
    /**************** IMPORTANT NOTE: **********************
     *
     * These checks exclude the last 2 rows and columns because
     * 1. the gradient in the last column/row is undefined (it's on the edge)
     * 2. the value next to the last column/row cannot be a peak since
     *    you cannot compare its gradient to the gradient of the last row/column
     *
     * This has the effect of shrinking the image in by 4 rows and
     * columns, but oh well.
     */
    for (int i=2; i < Gradient::rows-2; ++i) {
        for (int j=2; j < Gradient::cols-2; ++j){

            gradient->peaks[i][j] = false; // Not a peak yet
            const int z = gradient->getMagnitude(i,j);
            if (z > 0){
                const int y = gradient->getY(i,j);
                const int x = gradient->getX(i,j);

                // Get the highest 3 bits of the direction
                const int a = (gradient->dir3(y,x));;

                if (z > gradient->getMagnitude(i + Gradient::dyTab[a],
                                               j + Gradient::dxTab[a]) &&
                    z >= gradient->getMagnitude(i - Gradient::dyTab[a],
                                                j - Gradient::dxTab[a])){
                    gradient->peaks[i][j] = true;
                }
            }
            if (!gradient->peaks[i][j]){
                gradient->setX(0,i,j);
                gradient->setY(0,i,j);
                gradient->setMagnitude(0,i,j);
            }
        }
    }
#endif
    PROF_EXIT(P_EDGE_PEAKS);
}

