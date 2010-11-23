#include "EdgeDetector.h"
#include <cmath>
#include <iostream>


using boost::shared_ptr;
using namespace std;

EdgeDetector::EdgeDetector(int thresh) : threshold(thresh)
{

}

/**
 * Find the edges in a channel of an image.
 *
 * @param channel      The entire channel (one of Y, U, or V)
 */
void EdgeDetector::detectEdges(const Channel& channel,
                               shared_ptr<Gradient> gradient)
{
    sobelOperator(channel, gradient);
    findPeaks(gradient);
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
void EdgeDetector::sobelOperator(const Channel& channel,
                                 shared_ptr<Gradient> gradient)
{
    for (int i=1; i < Gradient::rows-1; ++i){
        for (int j=1; j < Gradient::cols-1; ++j) {

            int xGrad = (
                // Column j+1
                (channel.val[i-1][j+1] +
                 2 * channel.val[i][j+1] +
                 channel.val[i+1][j+1]) -
                // Column j-1
                (channel.val[i-1][j-1] +
                 2 * channel.val[i][j-1] +
                 channel.val[i+1][j-1]));

            int yGrad = (
                // Row i+1
                (channel.val[i+1][j-1] +
                 2 * channel.val[i+1][j] +
                 channel.val[i+1][j+1]) -
                // Row i -1
                (channel.val[i-1][j-1] +
                 2 * channel.val[i-1][j] +
                 channel.val[i-1][j+1])
                );
            gradient->x[i][j] = xGrad;
            gradient->y[i][j] = yGrad;
            gradient->mag[i][j] = xGrad * xGrad + yGrad * yGrad;
        }
    }
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
void EdgeDetector::findPeaks(shared_ptr<Gradient> gradient)
{
    // The magnitudes were not square rooted and are rather large
    const int edgeThreshold = (threshold * threshold) << 4;

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
            const int z = gradient->mag[i][j];

            if (z > edgeThreshold){
                const int y = gradient->y[i][j];
                const int x = gradient->x[i][j];

                // Get the highest 3 bits of the direction
                const int a = (gradient->dir3(y,x));;

                if (z > gradient->mag
                    [i + Gradient::dyTab[a]] [j + Gradient::dxTab[a]] &&
                    z >= gradient->mag
                    [i - Gradient::dyTab[a]][j - Gradient::dxTab[a]]){
                    gradient->peaks[i][j] = true;
                }
            }
            if (!gradient->peaks[i][j]){
                gradient->x[i][j] = gradient->y[i][j] = gradient->mag[i][j] = 0;
            }
        }
    }
}

