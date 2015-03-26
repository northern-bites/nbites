#pragma once

#include "Images.h"
#include "Blobber.h"
#include "Blob.h"

#include <vector>


namespace man {
namespace vision {

// TODO: This needs to be better
typedef struct circle_{
    point center;
    double radius;
} Circle;

class BallDetector {
public:
    BallDetector(messages::PackedImage8* orangeImage_);
    ~BallDetector();

    void findBalls();

private:
    void rateBlob(Blob b);
    std::pair<Circle, int> fitCircle(Blob b);
    int rateCircle(Circle c, std::vector<point> p, int delta);
    Circle circleFromPoints(point a, point b, point c);

    messages::PackedImage8* orangeImage;
    std::vector<Blob> blobs;

};



}
}
