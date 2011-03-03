#ifndef _Gradient_h_DEFINED
#define _Gradient_h_DEFINED

#include <stdlib.h>
#include <stdint.h>

#include <iostream>
#include "VisionDef.h"
#include "Common.h"


/**
 * Image channel gradient information struct
 */
class Gradient
{
    // Public member functions
public:
    Gradient();
    virtual ~Gradient() { };

    static inline int dir(int y, int x) {
        return static_cast<int>(atan2(y, x) / M_PI * 128.0) & 0xff;
    }

    void reset();

    /**
     * Calculate the highest three bits of an angle for the given y and x
     */
    static inline int dir3(int y, int x) {
        unsigned int d = 0x0;
        //http://www-graphics.stanford.edu/~seander/bithacks.html#ConditionalSetOrClearBitsWithoutBranching
        // w = (w & ~m) | (-f & m);

        // if (y <0){
        //     d |= 0x4;
        // }
        d = ( d & ~0x4) | ( -(y<0) & 0x4);

        // if ((x^y) < 0){
        //     d |= 0x2;
        // }
        d = (d & ~0x2) | ( -((x^y) < 0) & 0x2);


        // if (d & 0x2) {
        //     if (abs(x) > abs(y)){
        //         d |= 0x1;
        //     }
        // } else if (abs(x) < abs(y)){
        //     d |= 0x1;
        // }
        d = (d & 0x2) ? ((d & 0x1) | (- (abs(x) > abs(y)) & 0x1)) :
                         (d & 0x1) | (- (abs(x) < abs(y)) & 0x1);

        return (d);
    }

    // Values are all offset by one (see EdgeDetection.s)
    uint16_t getMagnitude(int i, int j){
        return values[i * IMAGE_WIDTH + j + magnitudes + 1];
    }

    int16_t getX(int i, int j){
        return values[i * IMAGE_WIDTH + j + x_grads + 1];
    }

    int16_t getY(int i, int j){
        return values[i * IMAGE_WIDTH + j + y_grads + 1];
    }

    void setMagnitude(uint16_t v, int i, int j){
        values[i * IMAGE_WIDTH + j + magnitudes + 1] = v;
    }

    void setX(int16_t v, int i, int j){
        values[i * IMAGE_WIDTH + j + x_grads + 1] = v;
    }

    void setY(int16_t v, int i, int j){
        values[i * IMAGE_WIDTH + j + y_grads + 1] = v;
    }

    // Return the nth angle in the angles array
    uint8_t getAngle(int n){
        return static_cast<uint8_t>(angles[n*3 + angles_offset]);
    }

    // Return the nth x coordinate in the angles array
    int16_t getAnglesXCoord(int n){
        return static_cast<int16_t>(angles[n*3 + angles_x_offset] + 160);
    }

    // Return the nth y coordinate in the angles array
    int16_t getAnglesYCoord(int n){
        return static_cast<int16_t>(angles[n*3 + angles_y_offset] + 120);
    }

    void addAngle(uint8_t angle, int16_t x, int16_t y){
        angles[numPeaks*3 + angles_offset] = angle;
        angles[numPeaks*3 + angles_x_offset] = static_cast<uint16_t>(x - 160);
        angles[numPeaks*3 + angles_y_offset] = static_cast<uint16_t>(y - 120);

        numPeaks++;

        // Mark as end
        angles[numPeaks*3 + angles_offset] = 0;
        angles[numPeaks*3 + angles_x_offset] = -160;
        angles[numPeaks*3 + angles_y_offset] = -120;
    }

    bool isPeak(int n){
        return (getAnglesXCoord(n) != 0 &&
                getAnglesYCoord(n) != 0);
    }

    int peaks_list_contains(int i, int j);

    // Public member variables

    // Values is all the arrays in one, the others
    // get pointers within 'values'
    uint16_t *values;

    // Each angle has 3 values: angle, x coordinate, and y coordinate
    int16_t *angles;
    int numPeaks;
    bool peaks[IMAGE_HEIGHT][IMAGE_WIDTH];

    enum {
        magnitudes = 0,
        x_grads = IMAGE_WIDTH * IMAGE_HEIGHT,
        y_grads = IMAGE_WIDTH * IMAGE_HEIGHT * 2,

        num_angles_limit = IMAGE_WIDTH * IMAGE_HEIGHT /2,
        angles_size = num_angles_limit * 3,
        angles_offset = 0,
        angles_x_offset = 1,
        angles_y_offset = 2,
    };

    const static int rows = IMAGE_HEIGHT;
    const static int cols = IMAGE_WIDTH;
    // Tables that specify the + neighbor of a pixel indexed by
    // gradient direction octant (the high 3 bits of direction).
    const static int DIRECTIONS = 8;
    const static int dxTab[DIRECTIONS];
    const static int dyTab[DIRECTIONS];

};

#endif /* _Gradient_h_DEFINED */
