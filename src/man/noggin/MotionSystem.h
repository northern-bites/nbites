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
 * Holds information about a single step.
 */
struct Step
{
    Step(float dX = 0.0f, float dY = 0.0f, float dA = 0.0f)
    : dx(dX), dy(dY), da(dA)
    { }
    
    float dx;
    float dy;
    float da;

    friend std::ostream& operator<<(std::ostream& out, Step s)
    {
	out << "Step taken, (" << s.dx << ", "
	    << s.dy << ", " << s.da << ")" << "\n";
	return out;
    }
};

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

    void feedStep(float dx, float dy, float da);
    void feedStep(Step s);
    Step noisyStep();
    PF::ParticleSet update(PF::ParticleSet particles);

 private:
    bool moved;
    Step lastStep;
};

#endif // MOTION_SYSTEM_H
