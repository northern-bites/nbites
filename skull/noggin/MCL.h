/**
 * MCL.h
 *
 * @author Tucker Hermans
 */

#ifndef _MCL_h_DEFINED
#define _MCL_h_DEFINED
using namespace std;

// Includes
#include <vector>
#include <math.h>

// Math Macros
#define DEG_TO_RAD (2. * PI) / 360.
#define RAD_TO_DEG 360. / (2. * PI)

// Constants
static const int M = 1000; // Number of particles

// TypeDefs
typedef pair<float, float> Measurement;

// Structs
// Pose Estimate
struct PoseEst
{
    float x, y, h;
};
// Odometery change
struct MotionModel
{
    float deltaF, deltaL, deltaR;
};
// Particle
struct Particle
{
    PoseEst pose;
    float weight;
};

// The Monte Carlo Localization class
class MCL
{
public:
    // Constructors & Destructors
    MCL();
    virtual ~MCL();

    // Core Functions
    vector<Particle> updateLoc(vector<Particle> X_t_1, MotionModel u_t,
                               vector<Measurement> z_t);

    // Getters
    /**
     * @return The current x esitamte of the robot
     */
    float getXEst() { return xEst;}
    /**
     * @return The current y esitamte of the robot
     */
    float getYEst() { return yEst;}
    /**
     * @return The current heading esitamte of the robot
     */
    float getHEst() { return hEst;}
    /**
     * @return The uncertainty associated with the x estimate of the robot.
     */
    float getXUncert() {return xUncert;}
    /**
     * @return The uncertainty associated with the y estimate of the robot.
     */
    float getYUncert() {return yUncert;}
    /**
     * @return The uncertainty associated with the robot's heading estimate.
     */
    float getHUncert() {return hUncert;}

    // Setters

private:
    // Core Functions
    PoseEst updateOdometery(MotionModel u_t, PoseEst x_t);
    float updateMeasurementModel(vector<Measurement> z_t, PoseEst x_t,
                                 int m);
    // Helpers
}

#endif // _MCL_H_DEFINED
