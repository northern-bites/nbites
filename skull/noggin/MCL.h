/**
 * MCL.h
 *
 * @author Tucker Hermans
 */

#ifndef MCL_h_DEFINED
#define MCL_h_DEFINED
using namespace std;

// Includes
// STL
#include <vector>
#include <math.h>
// Local
#include "Observation.h"

// Structs
// Pose Estimate
class PoseEst
{
public:
    double x;
    double y;
    double h;
}

// Odometery change
class MotionModel
{
public:
    double deltaF;
    flaot deltaL;
    double deltaR;
}

// Particle
class Particle
{
public:
    PoseEst pose;
    double weight;
    //BallEKF ball;
    //vector<EKF> opponents;
}

// Math Macros
#define DEG_TO_RAD (2. * PI) / 360.
#define RAD_TO_DEG 360. / (2. * PI)

// Constants
static const int M = 1000; // Number of particles

// The Monte Carlo Localization class
class MCL
{
public:
    // Constructors & Destructors
    MCL();
    virtual ~MCL();

    // Core Functions
    void updateLocalization(MotionModel u_t, vector<Observation> z_t);

    // Getters
    /**
     * @return The current x esitamte of the robot
     */
    double getXEst() { return curEst.x;}

    /**
     * @return The current y esitamte of the robot
     */
    double getYEst() { return curEst.y;}

    /**
     * @return The current heading esitamte of the robot
     */
    double getHEst() { return curEst.h;}

    /**
     * @return The uncertainty associated with the x estimate of the robot.
     */
    double getXUncert() { return curUncert.x;}

    /**
     * @return The uncertainty associated with the y estimate of the robot.
     */
    double getYUncert() { return curUncert.y;}

    /**
     * @return The uncertainty associated with the robot's heading estimate.
     */
    double getHUncert() { return curUncert.h;}

    // Setters
    /**
     * @param xEst The current x esitamte of the robot
     */
    void setXEst(double xEst) { curEst.x = xEst;}

    /**
     * @param yEst The current y esitamte of the robot
     */
    void setYEst(double yEst) { curEst.y = yEst;}

    /**
     * @param hEst The current heading esitamte of the robot
     */
    void setHEst(double hEst) { curEst.h = hEst;}

    /**
     * @param uncertX The uncertainty of the x estimate of the robot.
     */
    void setXUncert(double uncertX) { curUncert.x = uncertX;}

    /**
     * @return uncertY The uncertainty of the y estimate of the robot.
     */
    void setYUncert(double uncertY) { curUncert.y = uncertY;}

    /**
     * @param uncertH The uncertainty of the robot's heading estimate.
     */
    void setHUncert(double uncertH) { curUncert.h = uncertH;}

private:
    // Class variables
    PoseEst curEst; // Current {x,y,h} esitamates
    PoseEst curUncert; // Associated {x,y,h} uncertainties (standard deviations)
    vector<Particle> X_t; // Current set of particles

    // Core Functions
    PoseEst updateOdometery(MotionModel u_t, PoseEst x_t);
    double updateMeasurementModel(vector<Observation> z_t, PoseEst x_t,
                                 int m);
    void updateEstimates();

    // Helpers
    double determinePointWeight(Observation z, PoseEst x_t, LocLandmark l);
    double determineLineWeight(Observation z, PoseEst x_t, LocLandmark l);
    double getSimilarity(double r_d, double r_a, double z);
}

#endif // _MCL_H_DEFINED
