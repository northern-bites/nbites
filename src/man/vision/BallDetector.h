#pragma once

#include "Images.h"
#include "Blobber.h"
#include "Blob.h"
#include "structs.h"
#include "Ball.h"

#include <vector>


namespace man {
namespace vision {

class BallDetector {
public:
    BallDetector(const messages::PackedImage8* orangeImage_);
    ~BallDetector();

    std::vector<Ball>& findBalls();

private:
    Ball makeBall(Blob b, bool occluded);
    std::pair<Circle, int> fitCircle(Blob b);
    std::vector<point> rateCircle(Circle c, std::vector<point> p, int delta);
    Circle circleFromPoints(point a, point b, point c);
    Circle leastSquares(std::vector<point>& points);
    double distanceFromRadius(double rad);

    bool sanityCheck(Ball& b);

    const messages::PackedImage8* orangeImage;
    std::vector<Ball> balls;


};



}
}
