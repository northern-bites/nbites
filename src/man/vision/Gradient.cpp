#include "Gradient.h"

const int Gradient::dxTab[DIRECTIONS] = { 1,  1,  0, -1, -1, -1,  0,  1};
const int Gradient::dyTab[DIRECTIONS] = { 0,  1,  1,  1,  0, -1, -1, -1};

Gradient::Gradient() {
    values = new uint16_t[(IMAGE_HEIGHT * 3) * (IMAGE_WIDTH * 3)];
}


