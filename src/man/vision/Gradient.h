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
    // Structures and constants
public:
    struct AnglePeak {
        uint16_t angle;
        int16_t x;
        int16_t y;
    };

    enum {
        // Row Offsets in value array
        magnitudes = 0,
        x_grads = IMAGE_HEIGHT,
        y_grads = IMAGE_HEIGHT * 2,

        num_angles_limit = IMAGE_WIDTH * IMAGE_HEIGHT /2,
    };

    // Public member functions
public:
    Gradient();
    virtual ~Gradient() { };

    void reset();
    void clear();
    int peaks_list_contains(int i, int j);
    void printAnglesList();


    inline void addAngle(uint8_t angle, int16_t x, int16_t y){
        angles[numPeaks].angle = angle;
        angles[numPeaks].x     = static_cast<uint16_t>(x - IMAGE_WIDTH/2);
        angles[numPeaks].y     = static_cast<uint16_t>(y - IMAGE_HEIGHT/2);
        numPeaks++;
    }

    inline bool isPeak(int n){
        return (n < numPeaks);
    }

    static inline uint8_t dir(int y, int x) {
        return static_cast<uint8_t>(atan2(y, x) / M_PI * 128.0) & 0xff;
    }

    /**
     * Calculate the highest three bits of an angle for the given y and x
     */
    static inline uint8_t dir3(int y, int x) {
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

        return static_cast<uint8_t>(d);
    }

// **********************************************
//                    Getters
// **********************************************

    // Values are all offset by one (see EdgeDetection.s)
    inline uint16_t getMagnitude(int i, int j){
        return (values[i + magnitudes][j+1]);
    }

    inline int16_t getX(int i, int j){
        return values[i + x_grads][j+1];
    }

    inline int16_t getY(int i, int j){
        return values[i + y_grads][j + 1];
    }

    // Return the nth angle in the angles array
    inline uint8_t getAngle(int n){
        return static_cast<uint8_t>(angles[n].angle);
    }

    // Return the nth x coordinate in the angles array
    inline int16_t getAnglesXCoord(int n){
        return angles[n].x;
    }

    // Return the nth y coordinate in the angles array
    inline int16_t getAnglesYCoord(int n){
        return angles[n].y;
    }

// **********************************************
//                    Setters
// **********************************************

    void setMagnitude(uint16_t v, int i, int j){
        values[i + magnitudes][j + 1] = v;
    }

    void setX(int16_t v, int i, int j){
        values[i + x_grads][j + 1] = v;
    }

    void setY(int16_t v, int i, int j){
        values[i + y_grads][j + 1] = v;
    }


    // Public member variables
public:
    int numPeaks;
    const static int rows = IMAGE_HEIGHT;
    const static int cols = IMAGE_WIDTH;

    // Values is the 3 magnitude, x, & y gradient value arrays in one
    uint16_t values[IMAGE_HEIGHT*3][IMAGE_WIDTH];
    AnglePeak angles[num_angles_limit];

    // Tables that specify the + neighbor of a pixel indexed by
    // gradient direction octant (the high 3 bits of direction).
    const static int DIRECTIONS = 8;
    const static int dxTab[DIRECTIONS];
    const static int dyTab[DIRECTIONS];

};

#endif /* _Gradient_h_DEFINED */
