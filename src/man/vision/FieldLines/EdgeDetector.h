#ifndef EdgeDetector_h_DEFINED
#define EdgeDetector_h_DEFINED

#include "VisionDef.h"
#include "Gradient.h"
#include <stdint.h>

#include "boost/shared_ptr.hpp"

namespace man {
namespace vision {

/**
 * Used to find the step edges in a given channel of an image according to a
 * preset, fixed threshold.
 */
class EdgeDetector
{
public:
    EdgeDetector();
    virtual ~EdgeDetector(){ };

    /**
     * Find the edges in a channel of an image.
     *
     * @param channel      The entire channel (one of Y, U, or V)
     */
    void detectEdges(int upperBound,
                     int* field_edge,
                     const uint16_t* channel,
                     Gradient& gradient);

    void    setThreshold(uint8_t thresh) { threshold = thresh; }
    uint8_t getThreshold()               { return threshold;   }

protected:

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
    void sobelOperator(int upperBound,
                       const uint16_t* channel,
                       Gradient& gradient);

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
    void findPeaks(int upperBound,
                   int* field_edge,
                   Gradient& gradient);
private:
    uint8_t threshold;
};

}
}

#endif /* EdgeDetector_h_DEFINED */
