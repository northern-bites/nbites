/**
 * @brief Some useful funcitons and structs for the localization system
 *
 * @author EJ Googins <egoogins@bowdoin.edu>
 * @date   Febraury 2013
 */

#pragma once

#include <vector>
#include <cmath>
#include <iostream>

#include "NBMath.h"
#include "FieldConstants.h"

#include <boost/random.hpp>
#include <boost/random/normal_distribution.hpp>
#include <boost/random/uniform_real.hpp>
#include <boost/math/constants/constants.hpp>

namespace man
{
namespace localization
{
/**
 * @struct ParticleFilterParams
 * @brief Parameters used for the particle filter.
 */
struct ParticleFilterParams
{
    float fieldHeight;        //! Field height.
    float fieldWidth;         //! Field width.
    float numParticles;       //! Size of particle population.
    float alpha_fast;         //! Weight factor for fast exponential weight filter.
    float alpha_slow;         //! Weight factor for slow exponential weight filter.
    float odometryXYNoise;    //! Variance for x,y in MotionSystem when updating.
    float odometryHNoise;     //! Variance for h in MotionSystem when updating.
    // Need to add how much we prefer best particles
};



/**
 * Samples a Gaussian normal distribution of specified
 * mean and standard deviation (sigma.)
 * @param mean the mean of the data.
 * @param sigma the standard deviation of the data.
 * @return A random sample of the specified normal
 *         distribution.
 */
static float sampleNormal(float mean, float sigma)
{
    // Seed the random number generator.
    static boost::mt19937 rng(static_cast<unsigned>(std::time(0)));

    boost::normal_distribution<float> dist(mean, sigma);

    boost::variate_generator<boost::mt19937&,
                             boost::normal_distribution<float> > sample(rng, dist);

    return sample();
}

// A struct for storing the Loc Parameters
struct LocNormalParams
{
    LocNormalParams(float sx, float sy, float sh)
        : sigma_x(sx), sigma_y(sy), sigma_h(sh)
        {
        }
    LocNormalParams()
        : sigma_x(.50f), sigma_y(.50f), sigma_h(.1f)
        {
        }

    float sigma_x;
    float sigma_y;
    float sigma_h;
};

struct ReconstructedLocation
{
    ReconstructedLocation(float x_, float y_, float h_, float defSide_)
        : x(x_), y(y_), h(h_), defSide(defSide_)
        {
        }
    float x;
    float y;
    float h; // Robot Pose

    bool defSide;

    friend bool operator ==(const ReconstructedLocation& first,
                            const ReconstructedLocation& second)
        {
            if(fabs(first.x - second.x) > 15.f)
                return false;
            if(fabs(first.y - second.y) > 15.f)
                return false;
            if(fabs(first.h - second.h) > TO_RAD*20.f)
                return false;

            // Made it through, so close enough
            return true;
        }

};

template <class T>
struct RingBuffer
{
    int bufferLength;
    T *buffer;

    int curEntry;

    RingBuffer(float bufferLength_)
        : bufferLength(bufferLength_)
    {
        buffer = new T[bufferLength];
        curEntry = 0;
    }

    void add(T newElement) {
        curEntry = (curEntry++)%bufferLength;
        buffer[curEntry] = newElement;
    }

};

struct Point {
    float x,y;

    Point(float x_, float y_)
    {
        x = x_;
        y = y_;
    }
};

struct Line {
    static const float ERROR = .1f;

    Point start, end;
    float slope;
    bool vert;

    Line(Point start_, Point end_) :
        start(start_),
        end(end_)
    {
        if(end.x == start.x) {
            vert = true;
        }
        else {
            slope = (end.y - start.y)/(end.x - start.x);
            vert = false;
        }
    }

    float length() {
        return std::sqrt((end.x - start.x)*(end.x - start.x) + (end.y - start.y)*(end.y - start.y));
    }

    bool containsPoint(Point p)
    {
        if (vert) {
            // check on the line
            if(abs(p.x - start.x) < ERROR)
                // check on the segment
                if((start.y <= p.y && p.y <= end.y) || (start.y >= p.y && p.y >= end.y))
                    return true;
            return false;
        }

       // check on the line
        else {
            if (abs((start.y - p.y)*(end.x - p.x) - (end.y - p.y)*(start.x - p.x)) < ERROR) {
                // check on the segment
                if((start.x <= p.x && p.x <= end.x) || (start.x >= p.x && p.x >= end.x))
                    return true;
            }
            return false;
        }
    }

    /*
     * @brief - Calculate the point on the line segment closest to
     *          a given point in space
     */
    Point closestPointTo(Point p)
    {
        // l = start + t(end - start) for t = [0,1]
        // projection of p onto l given by:
        float n = dotProduct(p.x-start.x,
                             p.y - start.y,
                             end.x - start.x,
                             end.y - start.y);
        float d = dotProduct(end.x - start.x,
                             end.y - start.y,
                             end.x - start.x,
                             end.y - start.y);

        float t = n /d;
        if (t<0.f)
            return start;
        else if (t>1.f)
            return end;
        else {
            Point closest(start.x + t*(end.x - start.x), start.y + t*(end.y - start.y));
            return closest;
        }
    }

    float getError(Line obsv) {
        // project obsv start onto current line
        Point startProj = closestPointTo(obsv.start);
        float obsvLength = 0.f;

        // see if end fits on line segment
        if(obsv.start.x < obsv.end.x)
            obsvLength = abs(obsv.length());
        else if (obsv.start.x > obsv.end.x)
            obsvLength = -abs(obsv.length());
        else
            return 1000000.f;

        Point endProj = shiftDownLine(startProj, obsvLength);

        if (!containsPoint(endProj)) {
            endProj   = closestPointTo(obsv.end);
            startProj = shiftDownLine (endProj, -obsvLength);

            if (!containsPoint(startProj)) {
                // Failed to match obsv to this line (obsv too long)
                return 1000000.f;
            }
        }

        // matched segment onto this line, calculate area of given polygon
        // split into two trianges: obsvstart, start Proj, obsvend and
        //                          obsvEnd, endProj, startProj
        // get side lengths of first triangle
        float l1 = distance(obsv.start.x, obsv.start.y,
                            startProj.x , startProj.y );
        float l2 = distance(startProj.x , startProj.y,
                            obsv.end.x  , obsv.end.y );
        float l3 = distance(obsv.end.x  , obsv.end.y,
                            obsv.start.x, obsv.start.y);
        float area1 = calcTriangleArea(l1, l2, l3);

        // get side lengths of second triangle
        l1 = distance(obsv.end.x, obsv.end.y,
                      endProj.x, endProj.y);
        l2 = distance(endProj.x  , endProj.y,
                      startProj.x, startProj.y);
        l3 = distance(startProj.x, startProj.y,
                      obsv.end.x, obsv.end.y);
        float area2 = calcTriangleArea(l1, l2, l3);

        // Return in units cm, not cm^2
        return std::sqrt(area1 + area2);
    }


    /*
     * @brief - Starting at given point, calculate point given dist along line from inital
     *          Not gaurenteed to be on line segment,
     *          -dist = left, +dist = right (assume slope is never vertical)
     * @assume - given point is on this line
     */
    Point shiftDownLine(Point initial, float dist) {
        // Shift in x is dist*cos((tan^-1(slope))
        // Calculated w/o trig w/ identities #wolfram
        float dX = dist / std::sqrt((slope * slope) + 1.f);
        float dY = slope * dX;
        Point shifted(initial.x + dX, initial.y + dY);
        return shifted;
    }

    //HACK helper function since won't let me make straight and call NBfunction i want
    float dotProduct(float x1, float y1, float x2, float y2) {
        return (x1*x2) + (y1*y2);
    }

    float distance(float x1, float y1, float x2, float y2) {
        return std::sqrt((x2-x1)*(x2-x1) + (y2-y1)*(y2-y1));
    }

    /*
     * @brief - Calculate area of a triangle from its side lengths
     */
    float calcTriangleArea(float l1, float l2, float l3) {
        return std::sqrt(((l1+l2+l3)/2)*(((l1+l2+l3)/2)-l1)*(((l1+l2+l3)/2)-l2)*(((l1+l2+l3)/2)-l3));
    }
};

} // namespace localization
} // namespace man
