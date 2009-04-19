#include "LocEKF.h"
#include <boost/numeric/ublas/io.hpp> // for cout
#define DEBUG_LOC_EKF
//#define DEBUG_LOC_EKF_INPUTS
using namespace boost::numeric;
using namespace boost;

using namespace NBMath;

// Parameters
const float LocEKF::USE_CARTESIAN_DIST = 50.0f;
const float LocEKF::BETA_LOC = 3.0f;
const float LocEKF::GAMMA_LOC = 0.4f;
const float LocEKF::BETA_LAT = 3.0f;
const float LocEKF::GAMMA_LAT = 0.4f;
const float LocEKF::BETA_ROT = M_PI/16.0f;
const float LocEKF::GAMMA_ROT = 0.04f;

// Default initialization values
const float LocEKF::INIT_LOC_X = 370.0f;
const float LocEKF::INIT_LOC_Y = 270.0f;
const float LocEKF::INIT_LOC_H = 0.0f;
const float LocEKF::X_UNCERT_MAX = 680.0f;
const float LocEKF::Y_UNCERT_MAX = 440.0f;
const float LocEKF::H_UNCERT_MAX = 4*M_PI;
const float LocEKF::X_UNCERT_MIN = 1.0e-6;
const float LocEKF::Y_UNCERT_MIN = 1.0e-6;
const float LocEKF::H_UNCERT_MIN = 1.0e-6;
const float LocEKF::INIT_X_UNCERT = X_UNCERT_MAX / 2.0f;
const float LocEKF::INIT_Y_UNCERT = Y_UNCERT_MAX / 2.0f;
const float LocEKF::INIT_H_UNCERT = M_PI*4;
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

    // Setup initial values
    setXEst(initX);
    setYEst(initY);
    setHEst(initH);
    setXUncert(initXUncert);
    setYUncert(initYUncert);
    setHUncert(initHUncert);

    betas(2) = BETA_ROT;
    gammas(2) = GAMMA_ROT;

#ifdef DEBUG_LOC_EKF_INPUTS
    std::cout << "Initializing LocEKF with: " << *this << std::endl;
#endif
}

/**
 * Reset the EKF to a starting configuration
 */
void LocEKF::reset()
{
    setXEst(INIT_LOC_X);
    setYEst(INIT_LOC_Y);
    setHEst(INIT_LOC_H);
    setXUncert(INIT_X_UNCERT);
    setYUncert(INIT_Y_UNCERT);
    setHUncert(INIT_H_UNCERT);
}

/**
 * Method to deal with updating the entire loc model
 *
 * @param u The odometry since the last frame
 * @param Z The observations from the current frame
 */
void LocEKF::updateLocalization(MotionModel u, std::vector<Observation> Z)
{
#ifdef DEBUG_LOC_EKF_INPUTS
    std::cout << "Loc update: " << std::endl;
    std::cout << "Before updates: " << *this << std::endl;
    std::cout << "\tOdometery is " << u <<std::endl;
    std::cout << "\tObservations are: " << std::endl;
    for(unsigned int i = 0; i < Z.size(); ++i) {
        std::cout << "\t\t" << Z[i] <<std::endl;
    }
#endif

    // Update expected position based on odometry
    timeUpdate(u);
    limitAPrioriUncert();

    // // Remove ambiguous observations
    // std::vector<Observation>::iterator iter = Z.begin();
    // while( iter != Z.end() )
    // {
    //     if (iter->getNumPossibilities() > 1 ) {
    //         iter = Z.erase( iter );
    //     } else {
    //         ++iter;
    //     }
    // }

    // Correct step based on the observed stuff
    if (Z.size() > 0) {
        correctionStep(Z);
    } else {
        noCorrectionStep();
    }
    limitPosteriorUncert();

#ifdef DEBUG_LOC_EKF_INPUTS
    std::cout << "After updates: " << *this << std::endl;
    std::cout << std::endl;
    std::cout << std::endl;
    std::cout << std::endl;
#endif
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
#ifdef DEBUG_LOC_EKF_INPUTS
    std::cout << "\t\t\tUpdating Odometry of " << u << std::endl;
#endif

    // Calculate the assumed change in loc position
    // Assume no decrease in loc velocity
    StateVector deltaLoc(LOC_EKF_DIMENSION);
    const float h = getHEst();
    float sinh, cosh;
    sincosf(h, &sinh, &cosh);

    deltaLoc(0) = u.deltaF * cosh - u.deltaL * sinh;
    deltaLoc(1) = u.deltaF * sinh + u.deltaL * cosh;
    deltaLoc(2) = u.deltaR;

    A_k(0,2) =  -u.deltaF * sinh - u.deltaL * cosh;
    A_k(1,2) =  u.deltaF * sinh - u.deltaL * cosh;

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
#ifdef DEBUG_LOC_EKF_INPUTS
    std::cout << "\t\t\tIncorporating measurement " << z << std::endl;
#endif
    unsigned int obsIndex;

    // Get the best fit for ambigious data
    if (z.getNumPossibilities() > 1) {
        obsIndex = findBestLandmark(&z);
    } else {
        obsIndex = 0;
    }

    if (z.getVisDistance() < USE_CARTESIAN_DIST) {

#ifdef DEBUG_LOC_EKF_INPUTS
        std::cout << "\t\t\tUsing cartesian " << std::endl;
#endif

        // Convert our sighting to cartesian coordinates
        MeasurementVector z_x(2);
        z_x(0) = z.getVisDistance() * cos(z.getVisBearing());
        z_x(1) = z.getVisDistance() * sin(z.getVisBearing());

        // Get expected values of the post
        const float x_b = z.getPointPossibilities()[obsIndex].x;
        const float y_b = z.getPointPossibilities()[obsIndex].y;
        MeasurementVector d_x(2);

        const float x = xhat_k_bar(0);
        const float y = xhat_k_bar(1);
        const float h = xhat_k_bar(2);

        float sinh, cosh;
        sincosf(h, &sinh, &cosh);

        d_x(0) = (x_b - x) * cosh + (y_b - y) * sinh;
        d_x(1) = -(x_b - x) * sinh + (y_b - y) * cosh;

        // Calculate invariance
        V_k = z_x - d_x;

        // Calculate jacobians
        H_k(0,0) = -cosh;
        H_k(0,1) = -sinh;
        H_k(0,2) = -(x_b - x) * sinh + (y_b - y) * cosh;

        H_k(1,0) = sinh;
        H_k(1,1) = -cosh;
        H_k(1,2) = -(x_b - x) * cosh - (y_b - y) * sinh;

        // Update the measurement covariance matrix
        R_k(0,0) = z.getDistanceSD();
        R_k(1,1) = z.getDistanceSD();
    } else {

#ifdef DEBUG_LOC_EKF_INPUTS
        std::cout << "\t\t\tUsing polar " << std::endl;
#endif

        // Convert our sighting to cartesian coordinates
        MeasurementVector z_x(2);
        z_x(0) = z.getVisDistance();
        z_x(1) = z.getVisBearing();

        // Get expected values of the post
        const float x_b = z.getPointPossibilities()[obsIndex].x;
        const float y_b = z.getPointPossibilities()[obsIndex].y;
        MeasurementVector d_x(2);

        const float x = xhat_k_bar(0);
        const float y = xhat_k_bar(1);
        const float h = xhat_k_bar(2);

        d_x(0) = hypot(x - x_b, y - y_b);
        d_x(1) = atan2(y_b - y, x_b - x) - h;
        d_x(1) = NBMath::subPIAngle(d_x(1));

        // Calculate invariance
        V_k = z_x - d_x;
        V_k(1) = NBMath::subPIAngle(V_k(1));

        // Calculate jacobians
        H_k(0,0) = (x - x_b) / d_x(0);
        H_k(0,1) = (y - y_b) / d_x(0);
        H_k(0,2) = 0;

        H_k(1,0) = (y_b - y) / (d_x(0)*d_x(0));
        H_k(1,1) = (x - x_b) / (d_x(0)*d_x(0));
        H_k(1,2) = -1;

        // Update the measurement covariance matrix
        R_k(0,0) = z.getDistanceSD();
        R_k(1,1) = z.getBearingSD();

#ifdef DEBUG_LOC_EKF_INPUTS
        std::cout << "\t\t\tR vector is" << R_k << std::endl;
        std::cout << "\t\t\tH vector is" << H_k << std::endl;
        std::cout << "\t\t\tV vector is" << V_k << std::endl;
        std::cout << "\t\t\t\td vector is" << d_x << std::endl;
        std::cout << "\t\t\t\t\tx est is " << x << std::endl;
        std::cout << "\t\t\t\t\ty est is " << y << std::endl;
        std::cout << "\t\t\t\t\th est is " << h << std::endl;
        std::cout << "\t\t\t\t\tx_b est is " << x_b << std::endl;
        std::cout << "\t\t\t\t\ty_b est is " << y_b << std::endl;
#endif
    }
}

/**
 * Given an observation with multiple possibilities, we return the observation
 * with the best possibility as the first element of the vector
 *
 * @param z The observation to be fixed
 */
unsigned int LocEKF::findBestLandmark(Observation *z)
{
    std::vector<PointLandmark> possiblePoints = z->getPointPossibilities();
    float minDivergence = 10000.0f;
    unsigned int minIndex = 0;
    for (unsigned int i = 0; i < possiblePoints.size(); ++i) {
        float divergence = getDivergence(z, possiblePoints[i]);

        if (divergence < minDivergence) {
            minDivergence = divergence;
            minIndex = i;
        }
    }
    return minIndex;
}

/**
 * Get the divergence between an observation and a possible point landmark
 *
 * @param z The observation measurement to be examined
 * @param pt The possible point to check against
 *
 * @return The divergence value
 */
float LocEKF::getDivergence(Observation * z, PointLandmark pt)
{
    const float x = xhat_k_bar(0);
    const float y = xhat_k_bar(1);
    const float h = xhat_k_bar(2);
    float x_b_r = z->getVisDistance() * cos(z->getVisBearing());
    float y_b_r = z->getVisDistance() * sin(z->getVisBearing());

    float sinh, cosh;
    sincosf(h, &sinh, &cosh);

    float x_b  = (pt.x - x) * cosh + (pt.y - y) * sinh;
    float y_b = -(pt.x - x) * sinh + (pt.y - y) * cosh;

    return hypot(x_b_r - x_b, y_b_r - y_b);
}

/**
 * Method to ensure that uncertainty does not grow without bound
 */
void LocEKF::limitAPrioriUncert()
{
    // Check x uncertainty
    if(P_k_bar(0,0) < X_UNCERT_MIN) {
        P_k_bar(0,0) = X_UNCERT_MIN;
    }
    // Check y uncertainty
    if(P_k_bar(1,1) < Y_UNCERT_MIN) {
        P_k_bar(1,1) = Y_UNCERT_MIN;
    }
    // Check h uncertainty
    if(P_k_bar(2,2) < H_UNCERT_MIN) {
        P_k_bar(2,2) = H_UNCERT_MIN;
    }
    // Check x uncertainty
    if(P_k_bar(0,0) > X_UNCERT_MAX) {
        P_k_bar(0,0) = X_UNCERT_MAX;
    }
    // Check y uncertainty
    if(P_k_bar(1,1) > Y_UNCERT_MAX) {
        P_k_bar(1,1) = Y_UNCERT_MAX;
    }
    // Check h uncertainty
    if(P_k_bar(2,2) > H_UNCERT_MAX) {
        P_k_bar(2,2) = H_UNCERT_MAX;
    }
}

/**
 * Method to ensure that uncertainty does not grow or shrink without bound
 */
void LocEKF::limitPosteriorUncert()
{
    // Check x uncertainty
    if(P_k(0,0) < X_UNCERT_MIN) {
        P_k(0,0) = X_UNCERT_MIN;
        P_k_bar(0,0) = X_UNCERT_MIN;
    }
    // Check y uncertainty
    if(P_k(1,1) < Y_UNCERT_MIN) {
        P_k(1,1) = Y_UNCERT_MIN;
        P_k_bar(1,1) = Y_UNCERT_MIN;
    }
    // Check h uncertainty
    if(P_k(2,2) < H_UNCERT_MIN) {
        P_k(2,2) = H_UNCERT_MIN;
        P_k_bar(2,2) = H_UNCERT_MIN;
    }
    // Check x uncertainty
    if(P_k(0,0) > X_UNCERT_MAX) {
        P_k(0,0) = X_UNCERT_MAX;
        P_k_bar(0,0) = X_UNCERT_MAX;
    }
    // Check y uncertainty
    if(P_k(1,1) > Y_UNCERT_MAX) {
        P_k(1,1) = Y_UNCERT_MAX;
        P_k_bar(1,1) = Y_UNCERT_MAX;
    }
    // Check h uncertainty
    if(P_k(2,2) > H_UNCERT_MAX) {
        P_k(2,2) = H_UNCERT_MAX;
        P_k_bar(2,2) = H_UNCERT_MAX;
    }
}
