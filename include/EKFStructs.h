#ifndef EKFStructs_h
#define EKFStructs_h

// Structs
// Odometery change
class MotionModel
{
public:
    MotionModel(float f, float l, float r)
        : deltaF(f), deltaL(l), deltaR(r) { }
    MotionModel(const MotionModel& other)
        : deltaF(other.deltaF), deltaL(other.deltaL), deltaR(other.deltaR) { }
    MotionModel()
        : deltaF(0.0f), deltaL(0.0f), deltaR(0.0f) { }
    float deltaF;
    float deltaL;
    float deltaR;
};


/**
 * BallMeasurement - A non-generic class for holding the basic type information
 *                   required (by Tucker) for running the correctionStep in EKF
 */
struct RangeBearingMeasurement
{
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

#endif
