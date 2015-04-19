#pragma once

#include <vector>
#include <string>

#include "structs.h"
#include "Blob.h"

namespace man {
namespace vision {

class Ball {
public:
    Ball();
    Ball(Circle fit, double rating, double distance, Blob blob);

    double getRating() { return rating; }
    Circle getFit() { return fit; }
    double getDist() { return distance; }
    double getHead() { return heading; }

    void setRating(double r) { rating = r; }

private:
    Circle fit;
    double rating;
    double distance;
    double heading;

    Blob properties;
};
}
}
