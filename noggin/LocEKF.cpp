#include "LocEKF.h"
using namespace boost::numeric;
using namespace boost;

using namespace NBMath;

// Parameters
const float LocEKF::BETA_LOC = 5.0f;
const float LocEKF::GAMMA_LOC = 0.4f;
const float LocEKF::BETA_LAT = 5.0f;
const float LocEKF::GAMMA_LAT = 0.4f;
const float LocEKF::BETA_ROT = 5.0f;
const float LocEKF::GAMMA_ROT = 0.4f;

// Default initialization values
const float LocEKF::INIT_LOC_X = 100.0f;
const float LocEKF::INIT_LOC_Y = 100.0f;
const float LocEKF::INIT_LOC_H = 0.0f;
const float LocEKF::X_UNCERT_MAX = 440.0f;
const float LocEKF::Y_UNCERT_MAX = 680.0f;
const float LocEKF::H_UNCERT_MAX = M_PI;
const float LocEKF::X_UNCERT_MIN = 1.0e-6;
const float LocEKF::Y_UNCERT_MIN = 1.0e-6;
const float LocEKF::H_UNCERT_MIN = 1.0e-6;
const float LocEKF::INIT_X_UNCERT = 440.0f;
const float LocEKF::INIT_Y_UNCERT = 680.0f;
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
          LOC_MEASUREMENT_DIMENSION>(BETA_LOC,GAMMA_LOC)
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
    timeUpdate(MotionModel());

    // Correct step based on the observed stuff
    correctionStep(Z);
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
}
