#include "LocEKF.h"
//#define DEBUG_LOC_EKF
using namespace boost::numeric;
using namespace boost;

using namespace NBMath;

// Parameters
const float LocEKF::BETA_LOC = 15.0f;
const float LocEKF::GAMMA_LOC = 0.4f;
const float LocEKF::BETA_LAT = 15.0f;
const float LocEKF::GAMMA_LAT = 0.4f;
const float LocEKF::BETA_ROT = 5.0f;
const float LocEKF::GAMMA_ROT = 0.4f;

// Default initialization values
const float LocEKF::INIT_LOC_X = 370.0f;
const float LocEKF::INIT_LOC_Y = 270.0f;
const float LocEKF::INIT_LOC_H = 0.0f;
const float LocEKF::X_UNCERT_MAX = 440.0f;
const float LocEKF::Y_UNCERT_MAX = 680.0f;
const float LocEKF::H_UNCERT_MAX = M_PI;
const float LocEKF::X_UNCERT_MIN = 1.0e-6;
const float LocEKF::Y_UNCERT_MIN = 1.0e-6;
const float LocEKF::H_UNCERT_MIN = 1.0e-6;
const float LocEKF::INIT_X_UNCERT = 220.0f;
const float LocEKF::INIT_Y_UNCERT = 340.0f;
const float LocEKF::INIT_H_UNCERT = M_PI;
const float LocEKF::X_EST_MIN = -600.0f;
const float LocEKF::Y_EST_MIN = -1000.0f;
const float LocEKF::X_EST_MAX = 600.0f;
const float LocEKF::Y_EST_MAX = 1000.0f;

/**
 * Initialize the localization EKF class
 *
 * @param initX Initial x estimate
 * @param initY Initial y estimate
 * @param initH Initial heading estimate
 * @param initXUncert Initial x uncertainty
 * @param initYUncert Initial y uncertainty
 * @param initHUncert Initial heading uncertainty
 */
LocEKF::LocEKF(float initX, float initY, float initH,
               float initXUncert,float initYUncert, float initHUncert)
    : EKF<Observation, MotionModel, LOC_EKF_DIMENSION,
          LOC_MEASUREMENT_DIMENSION>(BETA_LOC,GAMMA_LOC), frameNumber(1)
{
    // ones on the diagonal
    A_k(0,0) = 1.0;
    A_k(1,1) = 1.0;
    A_k(2,2) = 1.0;
    A_k(3,3) = 1.0;

    // Setup initial values
    setXEst(initX);
    setYEst(initY);
    setHEst(initH);
    setXUncert(initXUncert);
    setYUncert(initYUncert);
    setHUncert(initHUncert);
}

/**
 * Method to deal with updating the entire loc model
 *
 * @param loc the loc seen this frame.
 */
void LocEKF::updateLocalization(MotionModel u, std::vector<Observation> Z)
{
    // Update expected position based on odometry
    timeUpdate(u);

#ifdef DEBUG_LOC_EKF
    std::cout << "Frame number " << frameNumber++ << std::endl;
    std::cout << "Pose is (" << getXEst() << ", " << getYEst() << ", "
              << getHEst() << ")" << std::endl;
#endif
    // Correct step based on the observed stuff
    if (Z.size() > 0) {
        correctionStep(Z);
    } else {
        noCorrectionStep();
    }
}

/**
 * Method incorporate the expected change in loc position from the last
 * frame.  Updates the values of the covariance matrix Q_k and the jacobian
 * A_k.
 *
 * @param u The motion model of the last frame.  Ignored for the loc.
 * @return The expected change in loc position (x,y, xVelocity, yVelocity)
 */
EKF<Observation, MotionModel,
    LOC_EKF_DIMENSION, LOC_MEASUREMENT_DIMENSION>::StateVector
LocEKF::associateTimeUpdate(MotionModel u)
{
    // Calculate the assumed change in loc position
    // Assume no decrease in loc velocity
    StateVector deltaLoc(LOC_EKF_DIMENSION);
    float h = getHEst();
    deltaLoc(0) = u.deltaF * cos(h) - u.deltaL * sin(h);
    deltaLoc(1) = u.deltaF * sin(h) + u.deltaL * cos(h);
    deltaLoc(2) = u.deltaR;

    return deltaLoc;
}

/**
 * Method to deal with incorporating a loc measurement into the EKF
 *
 * @param z the measurement to be incorporated
 * @param H_k the jacobian associated with the measurement, to be filled out
 * @param R_k the covariance matrix of the measurement, to be filled out
 * @param V_k the measurement invariance
 *
 * @return the measurement invariance
 */
void LocEKF::incorporateMeasurement(Observation z,
                                    StateMeasurementMatrix &H_k,
                                    MeasurementMatrix &R_k,
                                    MeasurementVector &V_k)
{
    // Convert our sighting to cartesian coordinates
    float x_b_r = z.getVisDistance() * cos(z.getVisBearing());
    float y_b_r = z.getVisDistance() * sin(z.getVisBearing());
    MeasurementVector z_x(2);

    z_x(0) = x_b_r;
    z_x(1) = y_b_r;


    // Get expected values of ball
    float x_b = z.getPointPossibilities()[0].x;
    float y_b = z.getPointPossibilities()[0].y;
    MeasurementVector d_x(2);

    float x = getXEst();
    float y = getYEst();
    float h = getHEst();

    d_x(0) = (x_b - x) * cos(h) + (y_b - y) * sin(h);
    d_x(1) = -(x_b - x) * sin(h) + (y_b - y) * cos(h);

    // Calculate invariance
    V_k = z_x - d_x;

#ifdef DEBUG_LOC_EKF
    std::cout << "Landmark: " << z.getID() << std::endl;
    std::cout << "Expected (" << d_x(0) << ", " << d_x(1) << ")\n";
    std::cout << "Observed (" << z_x(0) << ", " << z_x(1) << ")\n\n";
#endif
    // Calculate jacobians
    H_k(0,0) = -cos(h);
    H_k(0,1) = -sin(h);
    H_k(0,2) = 0;//-(x_b - x) * sin(h) + (y_b - y) * cos(h);

    H_k(1,0) = -sin(h);
    H_k(1,1) = -cos(h);
    H_k(1,2) = 0; //-(x_b - x) * cos(h) - (y_b - y) * sin(h);

    // Update the measurement covariance matrix
    R_k(0,0) = 2.0f*z.getDistanceSD();
    R_k(1,1) = 2.0f*z.getDistanceSD();}
