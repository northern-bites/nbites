#ifndef VisionStructs_h_defined
#define VisionStructs_h_defined

#include "Structs.h"

namespace man {
namespace vision {

// a blob structure that holds information about its own location, and
// information involving its larger blob structure
struct blob {
    // bounding coordinates of the blob
    point <int> leftTop;
    point <int> rightTop;
    point <int> leftBottom;
    point <int> rightBottom;
    int pixels; // the total number of correctly colored pixels in our blob
    int area;
};

struct run {
    int x;
    int y;
    int h;
};

struct stop {
    int x;
    int y;
    int bad;
    int good;
    int span;
};

}
}

#endif // VisionStructs_h_defined
