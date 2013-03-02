/**
 * @file   BehaviorsStructs.h
 * @author Tucker Hermans <thermans@bowdoin.edu>
 * @date   Mon Mar  9 19:32:25 2009
 *
 * @brief  Houses all of our struct classes for the Behaviors module
 */

#pragma once
#include "EKFStructs.h"
#include <math.h>
#include <ostream>
#include "NBMath.h"
using namespace NBMath;

/**
 * @brief Estimate of the robot's (x,y,heading) pose.
 */
class PoseEst
{
public:
    // Constructors
    PoseEst(float _x, float _y, float _h) :
        x(_x), y(_y), h(_h) {}
    PoseEst(const PoseEst& other) :
        x(other.x), y(other.y), h(other.h) {}
    PoseEst() {}
    float x;
    float y;
    float h;

    PoseEst operator+ (const PoseEst o)
        {
            return PoseEst(o.x + x,
                           o.y + y,
                           o.h + h);
        }
    void operator+= (const PoseEst o)
        {
            x += o.x;
            y += o.y;
            h += o.h;
        }
//    PoseEst operator+ (const MotionModel u_t)
//        {
//            // Translate the relative change into the global coordinate system
//            // And add that to the current estimate
//            float sinh, cosh;
//            sincosf(h, &sinh, &cosh);
//            return PoseEst(x + u_t.deltaF * cosh -
//                           u_t.deltaL * sinh,
//                           y + u_t.deltaF * sinh +
//                           u_t.deltaL * cosh,
//                           h + u_t.deltaR);
//        }
//    void operator+= (const MotionModel u_t)
//        {
//            float sinh, cosh;
//            sincosf(h, &sinh, &cosh);
//
//            // Translate the relative change into the global coordinate system
//            // And add that to the current estimate
//            x += u_t.deltaF * cosh - u_t.deltaL * sinh;
//            y += u_t.deltaF * sinh + u_t.deltaL * cosh;
//            h += u_t.deltaR;
//        }

    friend std::ostream& operator<< (std::ostream &o, const PoseEst &c)
        {
            return o << "(" << c.x << ", " << c.y << ", " << c.h << ")";
        }


};

/**
 * @brief Class to hold the ball positin and velocity in the x and y directions
 */
class BallPose
{
public:
    float x;
    float y;
    float velX;
    float velY;

    BallPose operator+ (const BallPose o) {
        return BallPose(x + o.velX,
                        y + o.velY,
                        o.velX,
                        o.velY);
    }

    void operator+= (const BallPose o) {
        x += o.velX;
        y += o.velY;
        velX = o.velX;
        velY = o.velY;
    }

    friend std::ostream& operator<< (std::ostream &o, const BallPose &c) {
        return o << "(" << c.x << ", " << c.y << ", " << c.velX
                 << ", " << c.velY << ")";
    }

    BallPose(float _x, float _y, float _vx, float _vy) :
        x(_x), y(_y), velX(_vx), velY(_vy) {}
    BallPose() {}

};

/**
 * @brief Stores field location information about a concrete point landmark
 */
struct PointLandmark
{
    PointLandmark(float _x, float _y) : x(_x), y(_y) {}
    PointLandmark() {}
    float x;
    float y;
    friend std::ostream& operator<< (std::ostream &o, const PointLandmark &c)
        {
            return o << c.x << " " << c.y;
        }

};

/**
 * @brief Stores field location information about a concrete corner landmark
 */
struct CornerLandmark
{
    CornerLandmark(float _x, float _y, float _a) : x(_x), y(_y), angle(_a) {}
    CornerLandmark() {}
    float x;
    float y;
    float angle;
    friend std::ostream& operator<< (std::ostream &o, const CornerLandmark &c) {
        return o << c.x << " " << c.y << " " << c.angle;
    }

};

/**
 * @brief Stores field location information about a concrete line landmark
 */
struct LineLandmark
{
    LineLandmark(float _x1, float _y1, float _x2, float _y2) :
        x1(_x1), y1(_y1), x2(_x2), y2(_y2),
        dx(_x2-_x1), dy(_y2-_y1)    // Components of line unit vector
        {
            // We want to make dx, dy components of line's _unit_ vector,
            // so we normalize them
            if (dx !=0 || dy != 0) {
                const float length = hypotf(dx,dy);
                dx = dx / length;
                dy = dy / length;
            }
        }
    LineLandmark() {}
    float x1;
    float y1;
    float x2;
    float y2;
    float dx;
    float dy;
    friend std::ostream& operator<< (std::ostream &o, const LineLandmark &c)
        {
            return o << c.x1 << " " << c.y1 << " " << c.x2 << " " <<
                c.y2 << " " << c.dx << " " << c.dy;
        }

};


class TeammateBallMeasurement
{
public:
    float ballX;
    float ballY;
    TeammateBallMeasurement(float _x = 0.0f, float _y = 0.0f) :
        ballX(_x), ballY(_y) {}
};
