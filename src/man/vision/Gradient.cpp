#include "Gradient.h"
#include <iostream>
#include <mmintrin.h>
#include <string.h>

using namespace std;

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
