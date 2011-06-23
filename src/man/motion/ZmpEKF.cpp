
#include "ZmpEKF.h"
#include "BasicWorldConstants.h"
#include "Kinematics.h"
using namespace Kinematics;
using namespace ekf;

using namespace boost::numeric;

const float ZmpEKF::beta = 0.1f;
const float ZmpEKF::gamma = 0.5f;
//const float ZmpEKF::variance  = 100.00f;

ZmpEKF::ZmpEKF()
    : EKF<ZmpMeasurement,ZmpTimeUpdate,
          zmp_num_dimensions, ekf::zmp_num_measurements>(beta, gamma)
{
    // ones on the diagonal
    A_k(0,0) = 1.0;
    A_k(1,1) = 1.0;

    // Set default values for sensor zmp
    xhat_k(0) = 0.0f;
    xhat_k(1) = 0.0f;

    //Set uncertainties
    P_k(0,0) = HIP_OFFSET_Y;
    P_k(1,1) = HIP_OFFSET_Y;

}

ZmpEKF::~ZmpEKF()
{

}


/**
 * Method that gets called when we want to advance the filter to the next state
 */
void ZmpEKF::update(const ZmpTimeUpdate tUp,
                    const ZmpMeasurement zMeasure) {
    timeUpdate(tUp);

    std::vector<ZmpMeasurement> z(1,zMeasure);
    correctionStep(z);
    //noCorrectionStep();
}

EKF<ZmpMeasurement,ZmpTimeUpdate, zmp_num_dimensions, zmp_num_measurements>::StateVector
ZmpEKF::associateTimeUpdate(ZmpTimeUpdate u_k)
{
    static ZmpTimeUpdate lastUpdate = {0.0f, 0.0f};

    StateVector delta(zmp_num_dimensions);
    delta(0) = u_k.cur_zmp_x - xhat_k(0);
    delta(1) = u_k.cur_zmp_y - xhat_k(1);

    return delta;
}


const float getDontTrustVariance(const float divergence){
    //For observations from untrustworth sensors, we inflate the variance
    //very high
    static const float minVariance = 20000.0f;
    static const float varianceScale = 4000.0f;
    return minVariance + std::abs(divergence) *varianceScale;
}

const float getVariance(const float divergence){
    //For observations from mostly trustworthy sensors,
    // can use a lower variance
    static const float minVariance = .5f;
    static const float varianceScale = 1.5f;
    return minVariance + std::abs(divergence) *varianceScale;
}


void ZmpEKF::incorporateMeasurement(const ZmpMeasurement& z,
                                    StateMeasurementMatrix &H_k,
                                    MeasurementMatrix &R_k,
                                    MeasurementVector &V_k)
{
    static const float com_height  = 310; //TODO: Move this
	float zheight_div_G = com_height/GRAVITY_mss;
    static MeasurementVector last_measurement(
        ublas::scalar_vector<float>(zmp_num_measurements, 0.0f));

    MeasurementVector z_x(zmp_num_measurements);
    z_x(0) = z.comX + zheight_div_G * z.accX;
    z_x(1) = z.comY + zheight_div_G * z.accY;

    // The Jacobian is the identity because the observation space is the same
    // as the state space.
    H_k(0,0) = 1.0f;
    H_k(1,1) = 1.0f;

    V_k = z_x - xhat_k; // divergence

    //MeasurementVector deltaS = z_x - last_measurement;

    R_k(0,0) = getVariance(V_k(0));//variance;
    R_k(1,1) = getVariance(V_k(1));//variance;

    last_measurement = z_x;
}
