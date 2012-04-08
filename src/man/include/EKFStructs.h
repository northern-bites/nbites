#ifndef EKFStructs_h
#define EKFStructs_h
#include <ostream>
#include "VisualBall.h"

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

#endif
