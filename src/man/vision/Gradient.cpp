#include "Gradient.h"

const int Gradient::dxTab[DIRECTIONS] = { 1,  1,  0, -1, -1, -1,  0,  1};
const int Gradient::dyTab[DIRECTIONS] = { 0,  1,  1,  1,  0, -1, -1, -1};

Gradient::Gradient() {
    values = new uint16_t[(IMAGE_HEIGHT * 3) * (IMAGE_WIDTH)];
    // Tangent array is made of triplets of (tangent, x coord, y coord)
    angles = new uint16_t[angles_size];
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
    while (getAnglesXCoord(n) != 0){
        if (getAnglesXCoord(n) == j &&
            getAnglesYCoord(n) == i){
            return n;
        }
        n++;
    }
    return 0;
}
