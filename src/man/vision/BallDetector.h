#pragma once

#include "Images.h"
#include "Camera.h"
#include "FastBlob.h"

namespace man {
namespace vision {

class BallDetector {
public:
    BallDetector(bool topCamera_);
    ~BallDetector();

    void findBall(ImageLiteU8 orange);

    int ballOn;

private:
    bool topCamera;
    Connectivity blobber;
    //Ball makeBall(Blob b, bool occluded);
    //std::pair<Circle, int> fitCircle(Blob b);
    //std::vector<point> rateCircle(Circle c, std::vector<point> p, int delta);
    //Circle circleFromPoints(point a, point b, point c);
    //Circle leastSquares(std::vector<point>& points);
    //double distanceFromRadius(double rad);

    //bool sanityCheck(Ball& b);

};

class Ball {
public:
    Ball(Blob& b);

private:
    Blob blob;
    void compute();

    double _confidence;
};


}
}
