#ifndef _Gradient_h_DEFINED
#define _Gradient_h_DEFINED

#include <iostream>


typedef struct channel_t {
    int val[IMAGE_HEIGHT][IMAGE_WIDTH];
} Channel;

/**
 * Image channel gradient information struct
 */
class Gradient
{
public:
    Gradient() { };
    virtual ~Gradient() { };

    int x[IMAGE_HEIGHT][IMAGE_WIDTH];
    int y[IMAGE_HEIGHT][IMAGE_WIDTH];
    int mag[IMAGE_HEIGHT][IMAGE_WIDTH];
    bool peaks[IMAGE_HEIGHT][IMAGE_WIDTH];

    const static int rows = IMAGE_HEIGHT;
    const static int cols = IMAGE_WIDTH;

    static int dir(int y, int x) {
        return static_cast<int>(atan2(y, x) / M_PI * 128.0) & 0xff;
    }
};

#endif /* _Gradient_h_DEFINED */
