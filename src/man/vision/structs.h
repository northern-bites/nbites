#pragma once

#include <math.h>

namespace man {
namespace vision {

const double BALL_RADIUS = 32.5;
const double VERT_FOV_DEG = 47.64;
const double VERT_FOV_RAD = 47.64 * M_PI / 180;

struct point {
    double x;
    double y;
};

// TODO: This needs to be better
struct Circle{
    point center;
    double radius;
};

}
}
