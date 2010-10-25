#include "EdgeDetector.h"
#include <cmath>


/**
 * Find the edges in a channel of an image.
 *
 * @param image      The entire YUV image
 * @param startIndex The starting index of the image to use for edge detection
 * @param imageSize  Number of elements in the given image.
 * @param chanOffset The offset for each value in the given image
 */
void EdgeDetector::detectEdges(const uchar[][] * image,
                               Gradient& gradient)
{
    sobelOperator(image, gradient);
    findPeaks(gradient);
}

/**
 * Apply the Sobel Operator to the given image and fill a given struct
 * with the gradient information (x, y, absolute magnitude)
 *
 * / -1 0 +1 \   / -1 -2 -1 \
 * | -2 0 +2 |   |  0  0  0 |
 * \ -1 0 +1 /   \ +1 +2 +1 /
 *      Gx           Gy
 * @param image     The image with edges to be detected.
 * @param gradient    Gradient struct to be populated.
 */
void EdgeDetector::sobelOperator(const uchar[][] * image,
                                 Gradient& gradient)
{

    for (int i=0; i < gradient.rows; i++){
        for (int j=0; j < gradient.cols; ++j) {

            int xGrad = (
                // Column j+1
                (image[i-1][j+1] +
                 2 * image[i][j+1] +
                 image[i+1][j+1]) -
                // Column j-1
                (image[i-1][j-1] +
                 2 * image[i][j-1] +
                 image[i+1][j-1]));

            int yGrad = (
                // Row i+1
                (image[i+1][j-1] +
                 2 * image[i+1][j] +
                 image[i+1][j+1]) -
                // Row i -1
                (image[i-1][j-1] +
                 2 * image[i-1][j] +
                 image[i-1][j+1])
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
    bool isPeak = false;
    for (int i=0; i < gradient.height; ++i) {
        for (int j=0; j < gradient.width; ++j){

            isPeak = false;
            const int z = gradient.mag[i][j];

            if (z > threshold){
                const int y = gradient.y[i][j];
                const int x = gradient.x[i][j];

                byte a = dir(y,x);

                // Get the highest 3 bits of the direction
                a = a >> 5;

                if (z >  gradient.mag[i + dyTab[a]][j + dxTab[a]] &&
                    z >= gradient.mag[i + dyTab[a]][j + dxTab[a]]){
                    isPeak = true;
                }
            }
            peaks[i][j] = isPeak;
            if (!isPeak){
                gradient.x[i][j] = gradient.y[i][j] = gradient.mag[i][j] = 0;
            }
        }
    }
}

byte EdgeDetector::dir(int y, int x)
{
    return atan2(y, x) / M_PI * 128.0 & 0xff;
}
