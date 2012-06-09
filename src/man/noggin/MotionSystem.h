/**
 * Implements the interface between the localization system
 * and the motion system for procuring odometry measurement
 * data.
 *
 * @author Ellis Ratner <eratner@bowdoin.edu>
 */
#ifndef MOTION_SYSTEM_H
#define MOTION_SYSTEM_H

#include "VisionSystem.h"

class VisionSystem; 

/**
 * Holds information about an odometry measurement.
 */
namespace PF
{
    struct OdometryMeasurement
    {
         OdometryMeasurement(float X = 0.0f, float Y = 0.0f, float H = 0.0f)
	 : x(X), y(Y), h(H)
	{ }
	
	float x;
	float y;
	float h;
	
	friend std::ostream& operator<<(std::ostream& out, OdometryMeasurement o)
	{
	    out << "Last odometry measurement, (" << o.x << ", "
		<< o.y << ", " << o.h << ")" << "\n";
	    return out;
	}
    };
}

/**
 * @class MotionSystem
 * 
 * Responsible for updating particles based on an odometry
 * measurement from the motion system.
 */
class MotionSystem : public PF::MotionModel
{
 public:
    MotionSystem();
    ~MotionSystem() { }

    void setCurrentOdometry(const PF::OdometryMeasurement &current);
    void setLastOdometry(const PF::OdometryMeasurement &last);
    PF::OdometryMeasurement noisyDeltaOdometry(const PF::OdometryMeasurement &newOdometry);
    PF::ParticleSet update(PF::ParticleSet particles);

    PF::OdometryMeasurement getLastOdometry() const { return lastOdometry; }

 private:
    bool moved;
    PF::OdometryMeasurement currentOdometry;
    PF::OdometryMeasurement lastOdometry;
};

#endif // MOTION_SYSTEM_H
