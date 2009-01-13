#include "EKF.h"

EKF::EKF(int dimension) : states(dimension)
{
    // Declare 4 dimensional state thing
    xhat_k(dimension);
    xhat_k_bar(dimension);
    Q_k(dimension,dimension);
    A_k(dimension,dimension);
    P_k(dimension,dimension);
}

void EKF::timeUpdate(MotionModel u_k)
{
    // Have the time update prediction incorporated
    // i.e. odometery, natural roll, etc.
    ublas::vector<float> deltas = associateTimeUpdate(u_k);
    xhat_k_bar = xhat_k + deltas;

    for(unsigned int i = 0; i < states; ++i) {
        Q_k(i,i) = beta + gamma * deltas(i) * deltas(i);
    }

    // Update error covariance matrix
    ublas::matrix<float> newP = prod(P_k, trans(A_k));
    P_k = prod(A_k, newP) + Q_k;
}

void EKF::correctionStep(std::vector<Observation> z_k)
{
    for(unsigned int i = 0; i < z_k.size(); ++i) {
        incorporateCorrectionMeasurement(z_k[i]);
    }
}

void EKF::noCorrectionStep()
{
}
