#include "Gradient.h"
#include <iostream>
#include <mmintrin.h>

using namespace std;

const int Gradient::dxTab[DIRECTIONS] = { 1,  1,  0, -1, -1, -1,  0,  1};
const int Gradient::dyTab[DIRECTIONS] = { 0,  1,  1,  1,  0, -1, -1, -1};

Gradient::Gradient() :
    numPeaks(0)
{

}

// Restore the gradient to its original state so we can reuse the same
// gradient object,
//
// @note: Useless really, since the gradients get
// overwritten every time, but good to have
void Gradient::reset()
{
    numPeaks = 0;
}

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


// Looks for the given coordinates in the peak list of the gradient
// and returns the index of it, if present. If not present, returns 0.
int Gradient::peaks_list_contains(int i, int j){
    int n = 0;
    while (isPeak(n)){
        if (getAnglesXCoord(n) == j &&
            getAnglesYCoord(n) == i){
            return n;
        }
        n++;
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
