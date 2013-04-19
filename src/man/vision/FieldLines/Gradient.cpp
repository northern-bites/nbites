#include "Gradient.h"
#include <iostream>
#include <mmintrin.h>
#include <string.h>
#include "Utility.h"

using std::cout;
using std::endl;

namespace man {
namespace vision {

const int Gradient::dxTab[DIRECTIONS] = { 1,  1,  0, -1, -1, -1,  0,  1};
const int Gradient::dyTab[DIRECTIONS] = { 0,  1,  1,  1,  0, -1, -1, -1};

Gradient::Gradient() :
    numPeaks(0)
{

}

Gradient::Gradient(const Gradient& other) :
    numPeaks(other.numPeaks)
{
    memcpy(angles, other.angles, sizeof(AnglePeak)*num_angles_limit);
    memcpy(values, other.values, sizeof(uint16_t)*IMAGE_HEIGHT*3*IMAGE_WIDTH);
}

// Restore the gradient to its original state so we can reuse the same
// gradient object,
// *****************************************
// MUST BE CALLED EVERY FRAME BEFORE REUSING A GRADIENT OBJECT
// *****************************************
void Gradient::reset()
{
    numPeaks = 0;
}

// @note: Not needed for ASM edge detection, since the gradients get
// overwritten every time, but good to have
void Gradient::clear()
{
    __m64* ptr = (__m64*)values;
    __m64 zero;
    zero ^= zero;
    for (int i = -IMAGE_HEIGHT * IMAGE_WIDTH * 3; i < 0; i += 16) {
        *ptr = zero;
        *(ptr+1) = zero;
        *(ptr+2) = zero;
        *(ptr+3) = zero;
        ptr += 4;
    }
    _mm_empty();
}

void Gradient::clearPeakGrid()
{
    for(int i=0; i < IMAGE_HEIGHT; ++i){
        for(int j=0; j < IMAGE_WIDTH; j+=4){
            peaks[i][j] = -1;
            peaks[i][j+1] = -1;
            peaks[i][j+2] = -1;
            peaks[i][j+3] = -1;
        }
    }
}

// Looks for the given coordinates in the peak list of the gradient
// and returns the index+1 of it, if present. If not present, returns 0.
//
// Adds 1 to index so that the zero _index_ is returned as true, not false
int Gradient::peaks_list_contains(int i, int j){

    for( int n = 0; isPeak(n); ++n){
        if (getAnglesXCoord(n) == j &&
            getAnglesYCoord(n) == i){
            return n+1;
        }
    }
    return 0;
}

void Gradient::printAnglesList()
{
    for (int n = 0; isPeak(n); n++){
        cout << "x,y,t:\t" << (int)getAnglesXCoord(n)
             << "\t" << (int)getAnglesYCoord(n)
             << "\t" << (int)getAngle(n) << endl;
    }
}

/**
 * Update the 2D grid of peak angles
 */
void Gradient::updatePeakGrid()
{
    clearPeakGrid();

    // Write angle to grid for each peak in list
    for(int i=0; isPeak(i); ++i){
        peaks[getAnglesYCoord(i) + Gradient::rows/2]
            [getAnglesXCoord(i) + Gradient::cols/2] =
            getAngle(i);
    }
}

void Gradient::createLineAtPoint(uint8_t angle, float radius)
{
    float radAngle = static_cast<float>(angle) * M_PI_FLOAT/128.f;

    double sn = sin(radAngle);
    double cs = cos(radAngle);

    double x0 = radius * cs;
    double y0 = radius * sn;

    for (double u = -200.; u <= 200.; u+=1.){
        int x = (int)round(x0 + u * sn);
        int y = (int)round(y0 - u * cs);

        if ( abs(x) < IMAGE_WIDTH/2 &&
             abs(y) < IMAGE_HEIGHT/2){
            addAngle(angle,
                     static_cast<int16_t>(x),
                     static_cast<int16_t>(y));
        }
    }
}

void Gradient::createSegment(const point<int>& l,
                             const point<int>& r)
{
    double radAngle = atan2((l.x - r.x), (r.y - l.y));

    double sn = sin(radAngle);
    double cs = cos(radAngle);

    double x0 = l.x;
    double y0 = l.y;

    float length = Utility::getLength(l,r);

    float xDiff = static_cast<float>(r.x - l.x)/length;
    float yDiff = static_cast<float>(r.y - l.y)/length;

    for (double u = 0; u <= length; u+=1.){
        int x = (int)round(x0 + u * xDiff);
        int y = (int)round(y0 + u * yDiff);

        if ( abs(x) < IMAGE_WIDTH/2 &&
             abs(y) < IMAGE_HEIGHT/2){
            addAngle(static_cast<uint8_t>(radAngle/M_PI * 128),
                     static_cast<int16_t>(x),
                     static_cast<int16_t>(y));
        }
    }
}

}
}
