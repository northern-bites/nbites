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
    BallDetector(const messages::PackedImage8* orangeImage_);
    ~BallDetector();

    std::vector<std::pair<Circle,double> >& findBalls();

private:
    void rateBlob(Blob b);
    std::pair<Circle, int> fitCircle(Blob b);
    std::vector<point> rateCircle(Circle c, std::vector<point> p, int delta);
    Circle circleFromPoints(point a, point b, point c);
    Circle leastSquares(std::vector<point> points);

    const messages::PackedImage8* orangeImage;
    std::vector<std::pair<Circle,double> > balls;


};



}
}
