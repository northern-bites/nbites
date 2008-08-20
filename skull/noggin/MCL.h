/**
 * MCL.h
 *
 * @author Tucker Hermans
 */

#ifndef _MCL_h_DEFINED
#define _MCL_h_DEFINED
using namespace std;

// Includes
// STL
#include <vector>
#include <math.h>
// Local
//#include "FieldConstants.h"
#include "Lnadmark.h"

// Structs
// Pose Estimate
class PoseEst
{
public:
    float x;
    float y;
    float h;
}

// Odometery change
class MotionModel
{
public:
    float deltaF;
    flaot deltaL;
    float deltaR;
}

// Particle
class Particle
{
public:
    PoseEst pose;
    float weight;
    //BallEKF ball;
    //vector<EKF> opponents;
}

// Math Macros
#define DEG_TO_RAD (2. * PI) / 360.
#define RAD_TO_DEG 360. / (2. * PI)

// Constants
static const int M = 1000; // Number of particles

// TypeDefs
typedef pair<float, float> Measurement;

// The Monte Carlo Localization class
class MCL
{
public:
    // Constructors & Destructors
    MCL();
    virtual ~MCL();

    // Core Functions
    void updateLocalization(MotionModel u_t, vector<Measurement> z_t);

    // Getters
    /**
     * @return The current x esitamte of the robot
     */
    float getXEst() { return curEst.x;}

    /**
     * @return The current y esitamte of the robot
     */
    float getYEst() { return curEst.y;}

    /**
     * @return The current heading esitamte of the robot
     */
    float getHEst() { return curEst.h;}

    /**
     * @return The uncertainty associated with the x estimate of the robot.
     */
    float getXUncert() { return curUncert.x;}

    /**
     * @return The uncertainty associated with the y estimate of the robot.
     */
    float getYUncert() { return curUncert.y;}

    /**
     * @return The uncertainty associated with the robot's heading estimate.
     */
    float getHUncert() { return curUncert.h;}

    // Setters
    /**
     * @param xEst The current x esitamte of the robot
     */
    void setXEst(float xEst) { curEst.x = xEst;}

    /**
     * @param yEst The current y esitamte of the robot
     */
    void setYEst(float yEst) { curEst.y = yEst;}

    /**
     * @param hEst The current heading esitamte of the robot
     */
    void setHEst(float hEst) { curEst.h = hEst;}

    /**
     * @param uncertX The uncertainty of the x estimate of the robot.
     */
    void setXUncert(float uncertX) { curUncert.x = uncertX;}

    /**
     * @return uncertY The uncertainty of the y estimate of the robot.
     */
    void setYUncert(float uncertY) { curUncert.y = uncertY;}

    /**
     * @param uncertH The uncertainty of the robot's heading estimate.
     */
    void setHUncert(float uncertH) { curUncert.h = uncertH;}

private:
    // Class variables
    PoseEst curEst; // Current {x,y,h} esitamates
    PoseEst curUncert; // Associated {x,y,h} uncertainties (standard deviations)
    vector<Particle> X_t; // Current set of particles

    // Core Functions
    PoseEst updateOdometery(MotionModel u_t, PoseEst x_t);
    float updateMeasurementModel(vector<Measurement> z_t, PoseEst x_t,
                                 int m);
    // Helpers
    void updateEstimates();
}

#endif // _MCL_H_DEFINED
