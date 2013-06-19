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
            if(std::fabs(first.x - second.x) > 15.f)
                return false;
            if(std::fabs(first.y - second.y) > 15.f)
                return false;
            if(std::fabs(first.h - second.h) > TO_RAD*20.f)
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

    float distanceTo(Point p) {
        return std::sqrt((p.x-x)*(p.x-x) + (p.y-y)*(p.y-y));
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
            slope = 0.f;
        }
        else {
            slope = (end.y - start.y)/(end.x - start.x);
            vert = false;
        }
    }

    float length() {
        return std::sqrt((end.x - start.x)*(end.x - start.x) + (end.y - start.y)*(end.y - start.y));
    }

    Point midpoint() {
        Point midpoint = shiftDownLine(start, length()/2.f);
        if (!contains(midpoint))
            midpoint =   shiftDownLine(start,-length()/2.f);
        if (!contains(midpoint))
            std::cout << "Massive Precision Error in Line Struct" << std::endl;
        return midpoint;
    }

    bool containsPoint(Point p)
    {
        float off = std::fabs((start.y - p.y)*(end.x - p.x) - (end.y - p.y)*(start.x - p.x));
        if (vert) {
            // check on the line
            if(std::fabs(p.x - start.x) < ERROR)
                // check on the segment
                if((start.y <= p.y && p.y <= end.y) || (start.y >= p.y && p.y >= end.y))
                    return true;
            return false;
        }
       // check on the line
        else {
            if (std::fabs((start.y - p.y)*(end.x - p.x) - (end.y - p.y)*(start.x - p.x)) < ERROR) {
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
        float n = NBMath::dotProduct(p.x-start.x,
                                     p.y - start.y,
                                     end.x - start.x,
                                     end.y - start.y);
        float d = NBMath::dotProduct(end.x - start.x,
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

    /*
     * @brief - Project onto the line (not segment as seen in closestPoinTo)
     */
    Point project(Point p)
    {
        // l = start + t(end - start) for t = [0,1]
        // projection of p onto l given by:
        float n = NBMath::dotProduct(p.x-start.x,
                                     p.y - start.y,
                                     end.x - start.x,
                                     end.y - start.y);
        float d = NBMath::dotProduct(end.x - start.x,
                                     end.y - start.y,
                                     end.x - start.x,
                                     end.y - start.y);

        float t = n /d;
        Point closest(start.x + t*(end.x - start.x), start.y + t*(end.y - start.y));
        return closest;
    }

    float getError(Line obsv) {
        // New Strategy: project midpoint onto line and shift both ways equal to half length
        Point midpoint = obsv.midpoint();
        Point midpointProj = closestPoint(midpoint);

        Point startProj = shiftDownLine(midpoint,  obsv.length()/2);
        Point endProj   = shiftDownLine(midpoint, -obsv.length()/2);

        // ensure projections are close to the points they're 'projected' from
        if (startProj.distanceTo(obsv.end) < startProj.distanceTo(obsv.start)) {
            startProj = shiftDownLine(midpoint, -obsv.length()/2);
            endProj   = shiftDownLine(midpoint,  obsv.length()/2);
        }

        // if both arent on line, then return massive error (doesnt fit on line)
        if(!containsPoint(startProj) && !containsPoint(endProj))
            return 100000.f;

        // if one isnt on line, shift the whole thing down
        if( !containsPoint(startProj) ) {
            // get distance off the line by
            float distOff = startProj.distanceTo(obsv.start);

            // shift by that amount
            Point newStartProj = shiftDownLine(startProj, distOff);
            if (!containsPoint(newStartProj)) { // need to shift other way
                startProj = shiftDownLine(startProj, -distOff);
                endProj   = shiftDownLine(  endProj, -distOff);
            }
            else { // got it right
                startProj = shiftDownLine(startProj, distOff);
                endProj   = shiftDownLine(  endProj, distOff);
            }
        }

        // if one is still not on line then return massive error
        if(!containsPoint(startProj) || !containsPoint(endProj))
            return 100000.f;



        //               if both on line check for intersection before computing errors




        // project obsv start onto current line
        Point startProj = closestPointTo(obsv.start);
        float obsvLength = 0.f;

        // see if end fits on line segment
        if(obsv.start.x < obsv.end.x)
            obsvLength = std::fabs(obsv.length());
        else if (obsv.start.x > obsv.end.x)
            obsvLength = -std::fabs(obsv.length());
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
        float l1 = obsv.start.distanceTo     (startProj);
        float l2 = startProj.distanceTo(obsv.end);
        float l3 = obsv.end.distanceTo (obsv.start);
        float area1 = NBMath::calcTriangleArea(l1, l2, l3);

        // get side lengths of second triangle
        l1 = obsv.end.distanceTo(endProj);
        l2 = endProj.distanceTo(startProj);
        l3 = startProj.distanceTo(obsv.end);
        float area2 = NBMath::calcTriangleArea(l1, l2, l3);

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
        if (vert) {
            Point vertShift(initial.x, initial.y + dist);
            return vertShift;
        }

        else {
            float dX = dist / std::sqrt((slope * slope) + 1.f);
            float shiftedX = initial.x + dX;
            float shiftedY = slope*(shiftedX - start.x) + start.y;
            Point shifted(shiftedX, shiftedY);
            return shifted;
        }
    }
};

} // namespace localization
} // namespace man
