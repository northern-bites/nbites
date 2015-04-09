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

std::vector<Ball>& BallDetector::findBalls() {
    Blobber<uint8_t> b(orangeImage->pixelAddress(0, 0), orangeImage->width(),
                       orangeImage->height(), 1, orangeImage->width());

    b.run(NeighborRule::eight, 90, 120, 10, 10);

    std::vector<Blob> blobs = b.getResult();

    // First look for non-occluded balls. Making heavier use of aspect ratio
    for(std::vector<Blob>::iterator i=blobs.begin(); i!=blobs.end(); i++) {
        Ball b = makeBall(*i, false);
        if (sanityCheck(b)) {
            balls.push_back(b);
        }
    }

    if (balls.size() == 0) {
        std::cout << "Didn't find any balls.. Let's lower our standards" << std::endl;
        for(std::vector<Blob>::iterator i=blobs.begin(); i!=blobs.end(); i++) {
            Ball b = makeBall(*i, true);
            if (sanityCheck(b)) {
                balls.push_back(b);
            }
        }
    }

    return balls;
}

Ball BallDetector::makeBall(Blob b, bool occluded) {
    // One rough measure of roundness
    double aspectRatio =  b.principalLength2() / b.principalLength1();


    std::pair<Circle, int> fit = fitCircle(b);

    double circleFit = ((double)fit.second) / b.getPerimeter();

    if (occluded) {
        aspectRatio *= 1.5;
        circleFit *= 1.5;
    }

    double rating = aspectRatio * b.density() * circleFit;

    double dist = distanceFromRadius(fit.first.radius);

    Ball ball(fit.first, rating, dist, b);

    std::cout << "BallDetector rated blob as: " << rating << std::endl;
    std::cout << "\tBecause aspectRatio: " << aspectRatio << " fit: " << circleFit
              << " density: " << b.density() << std::endl;

    // TODO: put distance estimations in here

    return ball;
}

std::pair<Circle, int> BallDetector::fitCircle(Blob b)
{
    point centerMass = {b.xCenter(), b.yCenter()};

    Circle best = {centerMass, b.maxX - b.minX};

    std::vector<point> perimeter = b.getPerimeterPoints();
    int numEdge = perimeter.size();
    std::cout << numEdge << " total edges" << std::endl;
    // Maybe right here we should do a least-squares on the points
    // It might give us an idea of how many outliers there are and
    // how many times we need to run ransac.

    int delta = 2;
    std::vector<point> inliers = rateCircle(best, perimeter, delta);
    int bestRating = inliers.size();

    // seed rand with current time
    srand(time(NULL));
    // TODO: this needs to be better!!!!
    for (int i=0; i<numEdge / 2; i++) {
        point one = perimeter.at(rand()%numEdge);
        point two = perimeter.at(rand()%numEdge);
        point three = perimeter.at(rand()%numEdge);

        Circle candidate = circleFromPoints(one, two, three);
        std::vector<point> tmp = rateCircle(candidate, perimeter, delta);
        int rating = tmp.size();
        if (rating > bestRating) {
            //std::cout << "Rating changed from " << bestRating << " to " << rating << std::endl;
            best = candidate;
            bestRating = rating;
            inliers = tmp;
        }
    }
    Circle ls = leastSquares(inliers);
    std::vector<point> lsResult = rateCircle(ls, perimeter, delta);
    std::cout << "RANSAC found: " << bestRating << " While LS found: " << lsResult.size() << std::endl;

    std::pair<Circle, int> ret;
    ret.first = ls;
    ret.second = bestRating;
    return ret;
}

std::vector<point> BallDetector::rateCircle(Circle c, std::vector<point> p, int delta)
{
    int count = 0;
    point center = c.center;
    std::vector<point> rets;

    for (std::vector<point>::iterator i = p.begin(); i != p.end(); i++) {
        double error = hypot(center.x - (*i).x, center.y - (*i).y) - c.radius;
        error = std::max(error, error * -1);
        if (error < delta) {
            count++;
            rets.push_back(*i);
        }
    }

    return rets;
}

double BallDetector::distanceFromRadius(double rad) {
    double trig = 1 / tan(rad * VERT_FOV_RAD / 240);
    return BALL_RADIUS * trig;
}

// Returns true if ball is reasonable size, distance, etc.
// Implement more!
bool BallDetector::sanityCheck(Ball& b) {
    return b.getRating() > .6;
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

Circle BallDetector::leastSquares(std::vector<point>& points)
{
    double mx = 0;
    double my = 0;
    double su = 0;
    double sv = 0;
    double suu = 0;
    double svv = 0;
    double suv = 0;
    double suuu = 0;
    double svvv = 0;
    double suvv = 0;
    double svuu = 0;

    double n = points.size();

    for (unsigned int i = 0; i < n; i++)
    {
        point p = points[i];
        mx += p.x;
        my += p.y;
        //std::cout << mx << std::endl;
    }
    mx = mx / n;
    my = my / n;


    for (unsigned int i = 0; i < n; i++)
    {
        point p = points[i];
        double u = p.x - mx;
        double v = p.y - my;

        su += u;
        sv += v;

        suu += u*u;
        svv += v*v;
        suv += u*v;

        suuu += u*u*u;
        svvv += v*v*v;

        suvv += u*v*v;
        svuu += v*u*u;
    }
    // std::cout << n << " " << mx << " " << my << std::endl;
    // std::cout << suu << " " << suv << " " << svv << std::endl;
    // std::cout << suuu << " " << svvv << " " << suvv << " " << svuu << std::endl;

    double detBase = suu*svv - suv*suv;
    double ca = .5 * (suuu + suvv);
    double cb = .5 * (svvv + svuu);

    double uc = (ca * svv - cb * suv) / detBase;
    double vc = (cb * suu - ca * suv) / detBase;

    double xc = uc + mx;
    double yc = vc + my;
    double r = pow(uc*uc + vc*vc + (suu + svv)/n, .5);
    point c = {xc, yc};
    Circle ret = {c, r};
    return ret;
    std::cout << "LEAST SQUARES: (" << xc << ", " << yc << ") " << r << std::endl;
}


}
}
