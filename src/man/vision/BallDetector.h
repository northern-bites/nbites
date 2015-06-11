#pragma once

#include <vector>

#include "Images.h"
#include "Camera.h"
#include "FastBlob.h"
#include "Homography.h"


namespace man {
namespace vision {

class Ball;

class BallDetector {
public:
    BallDetector(const FieldHomography& homography_);
    ~BallDetector();

    void findBall(ImageLiteU8 orange);

    int ballOn;

private:
    Connectivity blobber;
    const FieldHomography homography;

    std::vector<Ball> candidates;
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
    Ball(const Blob& b, double x_, double y_, int imgHeight_);

    double confidence() const { return _confidence; }
private:
    void compute();

    double pixDiameterFromDist(double d) const;

    Blob blob;
    double x_rel;
    double y_rel;
    int imgHeight;

    double _confidence;
};


}
}
