#include "BallDetector.h"

#include <math.h>
#include <algorithm>
#include <time.h>
#include <stdio.h>
#include <iostream>

namespace man {
namespace vision {

BallDetector::BallDetector(const messages::PackedImage8* orangeImage_):
orangeImage(orangeImage_)
{

}

BallDetector::~BallDetector() { }

std::vector<std::pair<Circle,double> >& BallDetector::findBalls() {
    Blobber<uint8_t> b(orangeImage->pixelAddress(0, 0), orangeImage->width(),
                       orangeImage->height(), 1, orangeImage->width());

    b.run(NeighborRule::eight, 90, 100, 100, 100);

    std::vector<Blob> blobs = b.getResult();

    for(std::vector<Blob>::iterator i=blobs.begin(); i!=blobs.end(); i++) {
        rateBlob(*i);
    }

    return balls;
}

void BallDetector::rateBlob(Blob b) {
    // One rough measure of roundness
    double aspectRatio =  b.principalLength2() / b.principalLength1();

    double area = M_PI * pow(b.principalLength1(), 2);

    // HMM... Think about this
    double circumToArea = b.getPerimeter() / area;

    std::pair<Circle, int> fit = fitCircle(b);
    double circleFit = fit.second / b.getPerimeter();

    double rating = aspectRatio * b.density() * circleFit;
    b.setRating(rating);

    if (rating > .7) {
        std::pair<Circle, double> ball;
        ball.first = fit.first;
        ball.second = rating;
        balls.push_back(ball);
    }
}

std::pair<Circle, int> BallDetector::fitCircle(Blob b)
{
    point centerMass = {b.xCenter(), b.yCenter()};

    Circle best = {centerMass, b.maxX - b.minX};

    std::vector<point> perimeter = b.getPerimeterPoints();
    int numEdge = perimeter.size();

    // Maybe right here we should do a least-squares on the points
    // It might give us an idea of how many outliers there are and
    // how many times we need to run ransac.

    int delta = 2;

    int bestRating = rateCircle(best, perimeter, delta);

    // seed rand with current time
    srand(time(NULL));
    // TODO: this needs to be better!!!!
    for (int i=0; i<numEdge / 2; i++) {
        point one = perimeter.at(rand()%numEdge);
        point two = perimeter.at(rand()%numEdge);
        point three = perimeter.at(rand()%numEdge);

        Circle candidate = circleFromPoints(one, two, three);
        int rating = rateCircle(candidate, perimeter, delta);

        if (rating > bestRating) {
            best = candidate;
            bestRating = rating;
        }
    }
    std::pair<Circle, int> ret;
    ret.first = best;
    ret.second = bestRating;
    return ret;
}

// Should be better, currently only returns count of points within delta of
// circle. TODO
int BallDetector::rateCircle(Circle c, std::vector<point> p, int delta)
{
    int count = 0;
    point center = c.center;

    for (std::vector<point>::iterator i = p.begin(); i != p.end(); i++) {
        double error = hypot(center.x - (*i).x, center.y - (*i).y) - c.radius;
        error = std::max(error, error * -1);
        if (error < delta) {
            count++;
        }
    }

    return count;
}

// Calculates the circle which intersects the three given points
// Done by applying Cramers rule to linear system of eqtn
Circle BallDetector::circleFromPoints(point a, point b, point c)
{
    // Based on Cramer's rule
    double detBase = (b.x - a.x)*(c.y - a.y) - (c.x - a.x)*(b.y - a.y);
    double constA = ((b.x*b.x - a.x*a.x) + (b.y*b.y - a.y*a.y)) / 2.0;
    double constB = ((c.x*c.x - a.x*a.x) + (c.y*c.y - a.y*a.y)) / 2.0;

    double xCenter = ((c.y-a.y)*constA - (b.y-a.y)*constB) / detBase;
    double yCenter = ((b.x-a.x)*constB - (c.x-a.x)*constA) / detBase;
    double radius = hypot(a.x - xCenter, a.y - yCenter);

    point center = {xCenter, yCenter};
    Circle circle = {center, radius};

    return circle;
}

}
}
