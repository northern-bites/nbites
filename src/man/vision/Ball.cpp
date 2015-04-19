#include "Ball.h"
namespace man {
namespace vision {

Ball::Ball()
{}

Ball::Ball(Circle f, double r, double d, Blob b):
    fit(f),
    rating(r),
    distance(d),
    properties(b)
{ }

}
}
