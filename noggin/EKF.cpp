#include "EKF.h"

/**
 * Simplest constructor for the EKF class
 *
 * @param dimension - the number of dimensions (states) for the EKF to track.
 */
EKF::EKF(unsigned int dimension) : states(dimension)
{
    // Declare 4 dimensional state thing
    xhat_k.resize(dimension,1);
    xhat_k_bar.resize(dimension,1);
    Q_k.resize(dimension,dimension);
    A_k.resize(dimension,dimension);
    P_k.resize(dimension,dimension);
    P_k_bar.resize(dimension,dimension);
    dimensionIdentity.resize(dimension);
}

/**
 * Function to calculate the required steps of the EKF time update.
 * Implementing classes must implement associateTimeUpdate to return the change
 * in estimate based on the inputed MotionModel u_k.
 *
 * @param u_k - the MotionModel showing the estimate change since the last time
 * update.
 */
void EKF::timeUpdate(MotionModel u_k)
{
    // Have the time update prediction incorporated
    // i.e. odometery, natural roll, etc.
    ublas::matrix<float> deltas = associateTimeUpdate(u_k);
    xhat_k_bar = xhat_k + deltas;

    // Calculate the uncertainty growth for the current update
    for(unsigned int i = 0; i < states; ++i) {
        Q_k(i,i) = beta + gamma * deltas(i,0) * deltas(i,0);
    }

    // Update error covariance matrix
    ublas::matrix<float> newP = prod(P_k, trans(A_k));
    P_k_bar = prod(A_k, newP) + Q_k;
}

/**
 * Function to perform the correction step of the EKF. Implementing classes must
 * implement the incorporateCorrectionMeasurement, to set...
 *
 * @param z_k - All measurements to be incoporated at the current update.
 */
void EKF::correctionStep(std::vector<Measurement> z_k)
{
    // Necessary computational matrices
    ublas::matrix<float> K_k; // Kalman gain matrix
    K_k.resize(states, 2);
    ublas::matrix<float> H_k; //
    H_k.resize(2, 2);
    ublas::matrix<float> R_k; // Assumed error in measurment sensors
    R_k.resize(2, 2);
    ublas::matrix<float> v_k; // Measurement invariance

    // Incorporate all correction observations
    for(unsigned int i = 0; i < z_k.size(); ++i) {
        v_k = incorporateCorrectionMeasurement(z_k[i], H_k, R_k);
    }

    // Calculate the Kalman gain matrix
    ublas::matrix<float> pTimesHTrans;
    pTimesHTrans = prod(P_k_bar, trans(H_k));
    K_k = prod(P_k_bar, prod(H_k, pTimesHTrans)+ R_k);
    // Use the Kalman gain matrix to determine the next estimate
    xhat_k = xhat_k_bar + prod(K_k, v_k);
    // Update associate uncertainty
    P_k = prod(dimensionIdentity - prod(K_k,H_k), P_k_bar);
}

/**
 * Function to update necessary information when there is noCorrectionStep.
 * Should be called at every timeUpdate which has no correctionStep.
 */
void EKF::noCorrectionStep()
{
    // Set current estimates to a priori estimates
    xhat_k = xhat_k_bar;
    P_k = P_k_bar;
}
