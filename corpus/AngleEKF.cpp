

#include "AngleEKF.h"
#include "BasicWorldConstants.h"
using namespace boost::numeric;

const int AngleEKF::num_dimensions = ANGLE_NUM_DIMENSIONS;
const float AngleEKF::beta = 3.0f;
const float AngleEKF::gamma = 2.0f;
const float AngleEKF::variance  = 0.22f;
//const float AccEKF::variance  = 100.00f;

AngleEKF::AngleEKF()
    : EKF<AngleMeasurement,int, num_dimensions, num_dimensions>(beta, gamma)
{
    // ones on the diagonal
    A_k(0,0) = 1.0;
    A_k(1,1) = 1.0;


    // Set default values for the angles
    xhat_k(0) = 0.0f;
    xhat_k(1) = 0.0f;


    //Set uncertainties
    P_k(0,0) = -GRAVITY_mss;
    P_k(1,1) = -GRAVITY_mss;
    //    P_k(2,2) = -GRAVITY_mss;


}

AngleEKF::~AngleEKF()
{

}


/**
 * Method that gets called when we want to advance the filter to the next state
 */
void AngleEKF::update(const float angleX,
		      const float angleY) {
    timeUpdate(0); // update model? we don't have one. it's an int. don't care.

    AngleMeasurement m = { angleX, angleY };
    // we create a vector of size 1 and default value m
    std::vector<AngleMeasurement> z(1,m);
    correctionStep(z);
}

EKF<AngleMeasurement, int, 2, 2>::StateVector
AngleEKF::associateTimeUpdate(int u_k)
{
    return ublas::zero_vector<float>(num_dimensions);
}

const float AngleEKF::scale(const float x) {
    //return .4f * std::pow(3.46572f, x);
    return 100.0f * std::pow(x, 5.0f) + 580.4f;
    // A bezier curve
    //return 6.73684f * std::pow(x,3) +
    //    37.8947f * std::pow(x,2) +
    //    -54.6316f * x +
    //    20.0f;

/*
    return 6.73684f * std::pow(x,3) +
       37.8947f * std::pow(x,2) +
       -54.6316f * x +
       70.0f;
*/
    //return 80 - 79 * std::exp( - .36f * std::pow( - 2.5f + x , 2));
    /*
    if (x > 9.0f)
        return 400.0f;
    else
        return 80 - 79 * std::exp( - .25f * std::pow( - 2.7f + x , 2));
    */
}

const float AngleEKF::getVariance(float delta, float divergence) {
    delta = std::abs(delta);
    divergence = std::abs(divergence);

    const float big = 3.5f;
    const float small = 1.0f;
    const float trust = .2f;
    const float dont_trust = 1000.0f;

    if (delta > big && divergence < small) {
        return trust;
    }

    if (delta < small && divergence < small) {
        return trust;
    }

    return dont_trust;
}

void AngleEKF::incorporateMeasurement(AngleMeasurement z,
                                    StateMeasurementMatrix &H_k,
                                    MeasurementMatrix &R_k,
                                    MeasurementVector &V_k)
{
    static MeasurementVector last_measurement(
        ublas::scalar_vector<float>(num_dimensions, 0.0f));

    MeasurementVector z_x(num_dimensions);
    z_x(0) = z.angleX;
    z_x(1) = z.angleY;
    //    z_x(2) = z.z; // hahahha

    V_k = z_x - xhat_k; // divergence

    // The Jacobian is the identity because the observation space is the same
    // as the state space.
    H_k(0,0) = 1.0f;
    H_k(1,1) = 1.0f;
    //    H_k(2,2) = 1.0f;

    MeasurementVector deltaS = z_x - last_measurement;

/*
    R_k(0,0) = getVariance(deltaS(0), V_k(0));
    R_k(1,1) = getVariance(deltaS(1), V_k(1));
    R_k(2,2) = getVariance(deltaS(2), V_k(2));

*/

    // Update the measurement covariance matrix
//     R_k(0,0) = scale(std::abs(deltaS(0)));
//     R_k(1,1) = scale(std::abs(deltaS(1)));
//     R_k(2,2) = scale(std::abs(deltaS(2)));

    R_k(0,0) = scale(std::abs(V_k(0)));
    R_k(1,1) = scale(std::abs(V_k(1)));
    //    R_k(2,2) = scale(std::abs(V_k(2)));
    last_measurement = z_x;
}
