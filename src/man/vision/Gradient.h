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
    uint16_t getAnglesXCoord(int n){
        return angles[n*3 + angles_x_offset];
    }

    // Return the nth y coordinate in the angles array
    uint16_t getAnglesYCoord(int n){
        return angles[n*3 + angles_y_offset];
    }

    // Public member variables

    // Values is all the arrays in one, the others get pointers within
    // 'values'
    uint16_t *values;
    enum {
        magnitudes = 0,
        x_grads = IMAGE_WIDTH * IMAGE_HEIGHT,
        y_grads = IMAGE_WIDTH * IMAGE_HEIGHT * 2
    };

    // Each angle has 3 values: angle, x coordinate, and y coordinate
    uint16_t *angles;

    enum {
        num_angles_limit = IMAGE_WIDTH * IMAGE_HEIGHT /2,
        angles_size = num_angles_limit * 3,
        angles_offset = 0,
        angles_x_offset = 1,
        angles_y_offset = 2,
    };

    bool peaks[IMAGE_HEIGHT][IMAGE_WIDTH];

    const static int rows = IMAGE_HEIGHT;
    const static int cols = IMAGE_WIDTH;
    // Tables that specify the + neighbor of a pixel indexed by
    // gradient direction octant (the high 3 bits of direction).
    const static int DIRECTIONS = 8;
    const static int dxTab[DIRECTIONS];
    const static int dyTab[DIRECTIONS];

};

#endif /* _Gradient_h_DEFINED */
