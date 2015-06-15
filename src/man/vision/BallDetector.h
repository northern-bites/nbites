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
    BallDetector(FieldHomography* homography_, bool topCamera);
    ~BallDetector();

    void findBall(ImageLiteU8 orange);

    int ballOn;


    // For tool
    const std::vector<Ball>& getBalls() const { return candidates; }
private:
    Connectivity blobber;
    FieldHomography* homography;
    bool topCamera;

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
    Ball(Blob& b, double x_, double y_, int imgHeight_);

    double confidence() const { return _confidence; }

    // For tool
    Blob& getBlob() { return blob; }
//private: should be private. leaving public for now
    void compute();

    double pixDiameterFromDist(double d) const;

    Blob blob;
    double x_rel;
    double y_rel;
    int imgHeight;
    double expectedDiam;

    double _confidence;
};


}
}
