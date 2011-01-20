#ifndef _Gradient_h_DEFINED
#define _Gradient_h_DEFINED

#include <stdlib.h>

#include <iostream>
#include "VisionDef.h"
#include "Common.h"


typedef struct channel_t {
    int val[IMAGE_HEIGHT * IMAGE_WIDTH];
} Channel;

/**
 * Image channel gradient information struct
 */
class Gradient
{
    // Public member functions
public:
    Gradient() { };
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

    // Public member variables
public:
    int x[IMAGE_HEIGHT][IMAGE_WIDTH];
    int y[IMAGE_HEIGHT][IMAGE_WIDTH];
    int mag[IMAGE_HEIGHT][IMAGE_WIDTH];
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
