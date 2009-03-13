/**
 * MCL.h
 *
 * @author Tucker Hermans
 * @version %I%
 */

#ifndef MCL_h_DEFINED
#define MCL_h_DEFINED

// Includes
// STL
#include <vector>
#include <math.h>
#include <time.h> // for srand(time(NULL))
#include <cstdlib> // for MAX_RAND
// Local
#include "Observation.h"
#include "FieldConstants.h"
#include "NBMath.h"
#include "NogginStructs.h"

#ifdef USE_PER_PARTICLE_EKF
#include "VisualBall.h"
#include "BallEKF.h"
#endif // USE_PER_PARTICLE_EKF

// Particle
class Particle
{
public:
    Particle(PoseEst _pose, float _weight);
    Particle(const Particle& other);
    Particle();
    PoseEst pose;
    float weight;
#   ifdef USE_PER_PARTICLE_EKF
    BallEKF ball;
    //std::vector<EKF> opponents;
#   endif // USE_PER_PARTICLE_EKF

    friend std::ostream& operator<< (std::ostream &o, const Particle &c) {
        return o << c.pose.x << " " << c.pose.y << " " << c.pose.h << " "
                 << c.weight;
    }

};

// Constants
static const int M = 100; // Number of particles
static const float MIN_SIMILARITY = 1.0e-15; // Minimum possible similarity

// The Monte Carlo Localization class
class MCL
{
public:
    // Constructors & Destructors
    MCL();
    virtual ~MCL();

    // Core Functions
    void updateLocalization(MotionModel u_t, std::vector<Observation> z_t,
                            bool resample=true);
#   ifdef USE_PER_PARTICLE_EKF
    void updateLocalization(MotionModel u_t, std::vector<Observation> z_t,
                            VisualBall * ball, bool resample=true);
#   endif
    // Getters
    const PoseEst getCurrentEstimate() const { return curEst; }
    const PoseEst getCurrentUncertainty() const { return curUncert; }
    /**
     * @return The current x esitamte of the robot
     */
    const float getXEst() const { return curEst.x;}

    /**
     * @return The current y esitamte of the robot
     */
    const float getYEst() const { return curEst.y;}

    /**
     * @return The current heading esitamte of the robot in radians
     */
    const float getHEst() const { return curEst.h;}

    /**
     * @return The current heading esitamte of the robot in degrees
     */
    const float getHEstDeg() const { return curEst.h * DEG_OVER_RAD;}

    /**
     * @return The uncertainty associated with the x estimate of the robot.
     */
    const float getXUncert() const { return curUncert.x * 2;}

    /**
     * @return The uncertainty associated with the y estimate of the robot.
     */
    const float getYUncert() const { return curUncert.y * 2;}

    /**
     * @return The uncertainty associated with the robot's heading estimate.
     */
    const float getHUncert() const { return curUncert.h * 2;}

    /**
     * @return The uncertainty associated with the robot's heading estimate.
     */
    const float getHUncertDeg() const { return curUncert.h * 2 * DEG_OVER_RAD;}

    /**
     * @return The current set of particles in the filter
     */
    const std::vector<Particle> getParticles() const { return X_t; }

#   ifdef USE_PER_PARTICLE_EKF

    /**
     * @return The current x esitamte of the ball
     */
    const float getBallXEst() const { return curBallEst.x; }

    /**
     * @return The current y esitamte of the ball
     */
    const float getBallYEst() const { return curBallEst.y; }

    /**
     * @return The current heading esitamte of the ball
     */
    const float getBallXVelocityEst() const { return curBallEst.velX; }

    /**
     * @return The current y velocity esitamte of the ball
     */
    const float getBallYVelocityEst() const { return curBallEst.velY; }

    /**
     * @return The uncertainty associated with the x estimate of the ball.
     */
    const float getBallXUncert() const { return curBallUncert.x * 2;}

    /**
     * @return The uncertainty associated with the y estimate of the ball.
     */
    const float getBallYUncert() const { return curBallUncert.y * 2;}

    /**
     * @return The uncertainty associated with the ball's heading estimate.
     */
    const float getBallXVelocityUncert() const { return curBallUncert.velX * 2;}

    /**
     * @return The uncertainty associated with the ball's heading estimate.
     */
    const float getBallYVelocityUncert() const { return curBallUncert.velY * 2;}

#   endif // USE_PER_PARTICLE_EKF
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
    std::vector<Particle> X_t; // Current set of particles
#   ifdef USE_PER_PARTICLE_EKF
    BallPose curBallEst;
    BallPose curBallUncert;
#   endif

    // Core Functions
    float updateMeasurementModel(std::vector<Observation> z_t, PoseEst x_t);
    void updateEstimates();

    // Helpers
    float determinePointWeight(Observation z, PoseEst x_t,
                               PointLandmark landmark);
    float determineLineWeight(Observation z, PoseEst x_t, LineLandmark _line);
    float getSimilarity(float r_d, float r_a, Observation &z);
    Particle randomWalkParticle(Particle p);

public:
    friend std::ostream& operator<< (std::ostream &o, const MCL &c) {
        return o << "Est: " << c.curEst << "\nUnct: " << c.curUncert;
    }

};

#endif // _MCL_H_DEFINED
