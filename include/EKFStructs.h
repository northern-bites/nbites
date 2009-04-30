#ifndef EKFStructs_h
#define EKFStructs_h
#include <ostream>
#include "VisualBall.h"

// Structs
// Odometery change
class MotionModel
{
public:
    MotionModel(float f = 0.0f, float l = 0.0f, float r = 0.0f)
        : deltaF(f), deltaL(l), deltaR(r) { }
    MotionModel(const MotionModel& other)
        : deltaF(other.deltaF), deltaL(other.deltaL), deltaR(other.deltaR) { }
    float deltaF;
    float deltaL;
    float deltaR;

    friend std::ostream& operator<< (std::ostream &o, const MotionModel &u) {
        return o << "(" << u.deltaF << ", " << u.deltaL << ", " << u.deltaR
                 << ")";
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
 * ZmpTimeUpdate - For ZmpEKF, the apriori input comes as the zmp from the
 *                 third entry in the controller's state vector.
 *
 */

struct ZmpTimeUpdate{
    float cur_zmp_x;
    float cur_zmp_y;
};

/**
 * AccelMeasurement - A non-generic class for holding accelerometer values
 *                    required (by motion) for filtering accel sensor values.
 */
struct ZmpMeasurement {
    float comX;
    float comY;
    float accX;
    float accY;
};

#endif
