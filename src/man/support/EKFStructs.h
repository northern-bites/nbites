#ifndef EKFStructs_h
#define EKFStructs_h
#include <ostream>
//#include "VisualBall.h"

class MotionModel;
typedef MotionModel DeltaMotionModel; //to be used to represent a delta CHANGE between MotionModels

// Structs
// Odometery model
// Should be used as the current TOTAL xy-translation and theta-rotation
// as measured by motion
class MotionModel
{
public:
    MotionModel(float x = 0.0f, float y = 0.0f, float theta = 0.0f)
        : x(x), y(y), theta(theta) { }

    DeltaMotionModel operator- (const MotionModel& other) const {
        return MotionModel(x - other.x, y - other.y, theta - other.theta);
    }

    bool isValid() const { return x != 0.0f && y != 0.0f && theta != 0.0f; }

    float x;
    float y;
    float theta;

    friend std::ostream& operator<< (std::ostream &o, const MotionModel &u) {
        return o << "(" << u.x << ", " << u.y << ", " << u.theta << ")";
    }

};


/**
 * BallMeasurement - A non-generic class for holding the basic type information
 *                   required (by Tucker) for running the correctionStep in EKF
 */
struct RangeBearingMeasurement
{
    RangeBearingMeasurement(float _dist = 0.0f, float _bearing = 0.0f,
                            float _distSD = 0.0f, float _bearingSD = 0.0f) :
        distance(_dist), bearing(_bearing), distanceSD(_distSD),
        bearingSD(_bearingSD) {}
    RangeBearingMeasurement(const RangeBearingMeasurement& other) :
        distance(other.distance), bearing(other.bearing),
        distanceSD(other.distanceSD), bearingSD(other.bearingSD) {}
    RangeBearingMeasurement(VisualBall * ball) :
        distance(ball->getDistance()), bearing(ball->getBearing()),
        distanceSD(ball->getDistanceSD()), bearingSD(ball->getBearingSD()) {}

    friend std::ostream& operator<< (std::ostream &o,
                                     const RangeBearingMeasurement &m) {
        return o << "(" << m.distance << ", " << m.bearing << ", "
                 << m.distanceSD << ", " << m.bearingSD << ")";
    }

    float distance;
    float bearing;
    float distanceSD;
    float bearingSD;
};


/**
 * AccelMeasurement - A non-generic class for holding accelerometer values
 *                    required (by motion) for filtering accel sensor values.
 */
struct AccelMeasurement {
    float x;
    float y;
    float z;
};

/**
 * AngleMeasurement - a class for holding angleX, angleY
 */

struct AngleMeasurement{
  float angleX;
  float angleY;
};


/**
 * ZmpTimeUpdate - For ZmpEKF, the apriori input comes as the zmp from the
 *                 third entry in the controller's state vector.
 *
 */

struct ZmpTimeUpdate{
    float cur_zmp_x;
    float cur_zmp_y;
};

/**
 * ZmpMeasurement - A non-generic class for holding accelerometer values
 *                    required (by motion) for filtering accel sensor values.
 */
struct ZmpMeasurement {
    float comX;
    float comY;
    float accX;
    float accY;
};

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

#endif
