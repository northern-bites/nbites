#include "Gradient.h"
#include <iostream>

using namespace std;

const int Gradient::dxTab[DIRECTIONS] = { 1,  1,  0, -1, -1, -1,  0,  1};
const int Gradient::dyTab[DIRECTIONS] = { 0,  1,  1,  1,  0, -1, -1, -1};

Gradient::Gradient() :
    values(new uint16_t[(IMAGE_HEIGHT * 3) * (IMAGE_WIDTH)]),
    angles(new int16_t[angles_size]),
    numPeaks(0)
{

}

// @TODO: Profile and rewrite using MMX intrinsics to do this 64bits at once
void Gradient::reset()
{
    for (int i = 0; i < angles_size; ++i) {
        angles[i] = 0;
    }

    for (int i = 0; i < IMAGE_HEIGHT * IMAGE_WIDTH * 3; ++i) {
        values[i] = 0;
    }
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
