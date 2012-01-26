#ifndef COLORZONE_H
#define COLORZONE_H

#include "image/Color.h"

namespace qtool {
namespace colorcreator {

class ColorSpace {

public:
    ColorSpace();

    bool contains(image::Color color) {

        // the circle can wrap around
        int h = color.getHb();
        int s = color.getSb();
        if (hHi > hLo) {
            if (hLo > h || hHi < h) {
                return false;
            }
        } else if (hLo > h && hHi < h) {
            return false;
        }
        if (s < sLo || s > sHi) {
            return false;
        }

        int y = color.getYb();
        int v = color.getVb();
        if (y < yLo || y > yHi) {
            return false;
        }
        if (v < vLo || v > vHi) {
            return false;
        }
        return true;
    }

    float hLo, hHi;
    float sLo, sHi;
    float yLo, yHi;
    float vLo, vHi;
    float zSlice;
};

}
}

#endif // COLORZONE_H
