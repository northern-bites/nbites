#ifndef _Gradient_h_DEFINED
#define _Gradient_h_DEFINED

#include <iostream>
#include "VisionDef.h"


typedef struct channel_t {
    int val[IMAGE_HEIGHT][IMAGE_WIDTH];
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


    static int dir(int y, int x) {
        return static_cast<int>(atan2(y, x) / M_PI * 128.0) & 0xff;
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
