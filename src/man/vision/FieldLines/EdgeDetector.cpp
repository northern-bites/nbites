#include "EdgeDetector.h"


#include <cmath>
#include <iostream>
#include <stdio.h>

namespace man {
namespace vision {

enum {
    default_edge_value = 50
};

extern "C" void _sobel_operator(int bound,
                                const uint8_t thresh,
                                const uint16_t *input,
                                uint16_t *out);
extern "C" int _find_edge_peaks(int bound,
                                const uint16_t *gradients,
                                AnglePeak *angles,
                                int* field_edge);
using boost::shared_ptr;
using namespace std;

EdgeDetector::EdgeDetector():
    threshold(default_edge_value)
{

}

void EdgeDetector::detectEdges(int upperBound,
                               int* field_edge,
                               const uint16_t* channel,
                               Gradient& gradient)
{
    sobelOperator(upperBound, channel, gradient);
    findPeaks(upperBound, field_edge, gradient);
}

void EdgeDetector::sobelOperator(int upperBound,
                                 const uint16_t* channel,
                                 Gradient& gradient)
{

#ifdef USE_MMX
    _sobel_operator(upperBound, threshold,
                    &channel[0], &gradient.values[0][0]);
#else
    for (int i=1+upperBound; i < Gradient::rows-1; ++i){
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

            gradient.setX(static_cast<int16_t>(xGrad), i, j);
            gradient.setY(static_cast<int16_t>(yGrad), i, j);

            xGrad = xGrad << 3;
            yGrad = yGrad << 3;

            xGrad = xGrad * xGrad;
            yGrad = yGrad * yGrad;

            xGrad = xGrad >> 16;
            yGrad = yGrad >> 16;

            int mag = xGrad + yGrad;

            // All non above threshold points are zero
            mag = max(0, mag-((threshold*threshold) >> 10));
            gradient.setMagnitude(static_cast<uint16_t>(mag), i, j);
        }
    }
#endif /* USE_MMX */

}

void EdgeDetector::findPeaks(int upperBound,
                             int * field_edge,
                             Gradient& gradient)
{

#ifdef USE_MMX
    gradient.numPeaks = _find_edge_peaks(upperBound,
                                         &gradient.values[0][0],
                                         gradient.angles,
                                         field_edge);
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
    for (int16_t i=int16_t(2+upperBound); i < Gradient::rows-2; ++i) {
        for (int16_t j=2; j < Gradient::cols-2; ++j){

            const int z = gradient.getMagnitude(i,j);
            if (z > 0){
                const int y = gradient.getY(i,j);
                const int x = gradient.getX(i,j);

                // Get the highest 3 bits of the direction
                const int a = (Gradient::dir3(y,x));;

                if (z > gradient.getMagnitude(i + Gradient::dyTab[a],
                                               j + Gradient::dxTab[a]) &&
                    z >= gradient.getMagnitude(i - Gradient::dyTab[a],
                                               j - Gradient::dxTab[a])){
                    gradient.addAngle(Gradient::dir(y,x),
                                      static_cast<int16_t>(j - IMAGE_WIDTH/2),
                                      static_cast<int16_t>(i - IMAGE_HEIGHT/2));
                }
            }
        }
    }
#endif
    gradient.updatePeakGrid();

}

}
}
