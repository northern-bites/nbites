#include "EdgeDetector.h"
#include <cmath>

const int EdgeDetector::dxTab[8] = { 1,  1,  0, -1, -1, -1,  0,  1};
const int EdgeDetector::dyTab[8] = { 0,  1,  1,  1,  0, -1, -1, -1};


EdgeDetector::EdgeDetector(int thresh) : threshold(thresh)
{

}

/**
 * Find the edges in a channel of an image.
 *
 * @param channel      The entire channel (one of Y, U, or V)
 */
void EdgeDetector::detectEdges(const Channel& channel,
                               Gradient& gradient)
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
                                 Gradient& gradient)
{

    for (int i=1; i < gradient.rows-1; i++){
        for (int j=1; j < gradient.cols-1; ++j) {

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
            gradient.x[i][j] = xGrad;
            gradient.y[i][j] = yGrad;
            gradient.mag[i][j] = xGrad * xGrad + yGrad * yGrad;
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
void EdgeDetector::findPeaks(Gradient& gradient)
{
    for (int i=0; i < gradient.rows; ++i) {
        for (int j=0; j < gradient.cols; ++j){

            gradient.peaks[i][j] = false;
            const int z = gradient.mag[i][j];

            if (z > threshold){
                const int y = gradient.y[i][j];
                const int x = gradient.x[i][j];

                int a = static_cast<int>(dir(y,x));

                // Get the highest 3 bits of the direction
                a = a >> 5;

                if (z >  gradient.mag[i + dyTab[a]][j + dxTab[a]] &&
                    z >= gradient.mag[i + dyTab[a]][j + dxTab[a]]){
                    gradient.peaks[i][j] = true;
                }
            }
            if (!gradient.peaks[i][j]){
                gradient.x[i][j] = gradient.y[i][j] = gradient.mag[i][j] = 0;
            }
        }
    }
}

int EdgeDetector::dir(int y, int x)
{
    return static_cast<int>(atan2(y, x) / M_PI * 128.0) & 0xff;
}

