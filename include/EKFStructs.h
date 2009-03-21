#ifndef EKFStructs_h
#define EKFStructs_h

/**
 * BallMeasurement - A non-generic class for holding the basic type information
 *                   required (by Tucker) for running the correctionStep in EKF
 */
struct BallMeasurement
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
