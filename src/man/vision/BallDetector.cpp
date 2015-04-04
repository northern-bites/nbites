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

    b.run(NeighborRule::eight, 90, 100, 10, 10);

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
    std::cout << fit.second << " was fit out of " << b.getPerimeter() <<std::endl;
    double circleFit = ((double)fit.second) / b.getPerimeter();
    if(circleFit == 0) {
        std::cout << "LITERALLY couldn't fit a circle" << std::endl;
    }

    double rating = aspectRatio * b.density() * circleFit;
    b.setRating(rating);

    std::cout << "BallDetector rated blob as: " << rating << std::endl;

    if (rating > .20) {
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

// Should be better, currently only returns count of points within delta of
// circle. TODO
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

Circle BallDetector::leastSquares(std::vector<point> points)
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
