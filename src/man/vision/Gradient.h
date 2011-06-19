#ifndef _Gradient_h_DEFINED
#define _Gradient_h_DEFINED

#include <stdlib.h>
#include <stdint.h>

#include <iostream>
#include "VisionDef.h"
#include "Common.h"
#include "Structs.h"


/**
 * Image channel gradient information struct
 */
class Gradient
{
    // Structures and constants
public:
    // x, y coords are stored relative to image center (origin)
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
    explicit Gradient();
    virtual ~Gradient() { };

    // MUST BE CALLED EVERY FRAME BEFORE REUSING A GRADIENT OBJECT
    void reset();

    void clear();
    void clearPeakGrid();
    int peaks_list_contains(int i, int j);
    void printAnglesList();
    void updatePeakGrid();
    void createLineAtPoint(uint8_t angle, float radius);

    void createSegment(const point<int>& l,
                       const point<int>& r);


    inline void addAngle(uint8_t angle, int16_t x, int16_t y){
        angles[numPeaks].angle = angle;
        angles[numPeaks].x     = x;
        angles[numPeaks].y     = y;
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
        uint8_t d = 0x0;
        if (y <0){
            d |= 0x4;
        }

        if ((x^y) < 0){
            d |= 0x2;
        }


        if (d & 0x2) {
            if (abs(x) > abs(y)){
                d |= 0x1;
            }
        } else if (abs(x) < abs(y)){
            d |= 0x1;
        }

        return d;
    }

// **********************************************
//                    Getters
// **********************************************

    // Values are all offset by one (see EdgeDetection.s)
    inline uint16_t getMagnitude(int i, int j){
#ifdef USE_MMX
        return (values[i + magnitudes][j+1]);
#else
        return (values[i + magnitudes][j]);
#endif
    }

    inline int16_t getX(int i, int j){
#ifdef USE_MMX
        return values[i + x_grads][j+1];
#else
        return values[i + x_grads][j];
#endif
    }

    inline int16_t getY(int i, int j){
#ifdef USE_MMX
        return values[i + y_grads][j + 1];
#else
        return values[i + y_grads][j];
#endif
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
#ifdef USE_MMX
        values[i + magnitudes][j + 1] = v;
#else
        values[i + magnitudes][j] = v;
#endif
    }

    void setX(int16_t v, int i, int j){
#ifdef USE_MMX
        values[i + x_grads][j + 1] = v;
#else
        values[i + x_grads][j] = v;
#endif
    }

    void setY(int16_t v, int i, int j){
#ifdef USE_MMX
        values[i + y_grads][j + 1] = v;
#else
        values[i + y_grads][j] = v;
#endif
    }


    // Public member variables
public:
    int numPeaks;
    enum {
        rows = IMAGE_HEIGHT,
        cols = IMAGE_WIDTH
    };

    // Values is the 3 magnitude, x, & y gradient value arrays in one
    uint16_t  values[rows*3][cols];
    int16_t  peaks [rows][cols];
    AnglePeak angles[num_angles_limit];

    // Tables that specify the + neighbor of a pixel indexed by
    // gradient direction octant (the high 3 bits of direction).
    const static int DIRECTIONS = 8;
    const static int dxTab[DIRECTIONS];
    const static int dyTab[DIRECTIONS];
private:
    Gradient(const Gradient& other);
};

#endif /* _Gradient_h_DEFINED */
