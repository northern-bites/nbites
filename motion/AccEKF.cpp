

#include "AccEKF.h"
#include "BasicWorldConstants.h"
using namespace boost::numeric;

const int AccEKF::num_dimensions = 3;
const float AccEKF::beta = 0.2f;
const float AccEKF::gamma = .2f;
const float AccEKF::variance  = 0.22f;
//const float AccEKF::variance  = 100.00f;

AccEKF::AccEKF()
    : EKF<AccelMeasurement,int>(num_dimensions, num_dimensions, beta, gamma)
{
    // ones on the diagonal
    A_k(0,0) = 1.0;
    A_k(1,1) = 1.0;
    A_k(2,2) = 1.0;

    // Set default values for the accelerometers
    xhat_k(0) = 0.0f;
    xhat_k(1) = 0.0f;
    xhat_k(2) = GRAVITY_mss;

    //Set uncertainties
    P_k(0,0) = -GRAVITY_mss;
    P_k(1,1) = -GRAVITY_mss;
    P_k(2,2) = -GRAVITY_mss;


}

AccEKF::~AccEKF()
{

}


/**
 * Method that gets called when we want to advance the filter to the next state
 */
void AccEKF::update(const float accX,
                    const float accY,
                    const float accZ) {
    timeUpdate(0); // update model? we don't have one. it's an int. don't care.

    AccelMeasurement m = { accX, accY, accZ };
    // we create a vector of size 1 and default value m
    std::vector<AccelMeasurement> z(1,m);
    correctionStep(z);
}

ublas::vector<float>
AccEKF::associateTimeUpdate(int u_k)
{
    return ublas::zero_vector<float>(num_dimensions);
}

ublas::vector<float> AccEKF::getGain(const ublas::vector<float> &est_error){
    return est_error * 17.0f;
}

const float scale(const float x) {
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

const float scaleDivergence(const float x){
    return 100.0f * std::pow(x, 5.0f) + 580.4f;
}

const float getVariance(float delta, float divergence) {
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

void AccEKF::incorporateMeasurement(AccelMeasurement z,
                                    ublas::matrix<float> &H_k,
                                    ublas::matrix<float> &R_k,
                                    ublas::vector<float> &V_k)
{
    static ublas::vector<float> last_measurement(
        ublas::scalar_vector<float>(num_dimensions, 0.0f));
    ublas::vector<float> z_x(num_dimensions);
    z_x(0) = z.x;
    z_x(1) = z.y;
    z_x(2) = z.z; // hahahha

    V_k = z_x - xhat_k; // divergence

    // The Jacobian is the identity because the observation space is the same
    // as the state space.
    H_k(0,0) = 1.0f;
    H_k(1,1) = 1.0f;
    H_k(2,2) = 1.0f;

    ublas::vector<float> deltaS =
        z_x - last_measurement;

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
    R_k(2,2) = scale(std::abs(V_k(2)));
    last_measurement = z_x;
}
