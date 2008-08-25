/**
 * EKF.cpp - An abstract class which implements the computational components of
 *           an Extended Kalman Filter.
 *
 * @date August 2008
 * @author Tucker Hermans
 */

EKF::EKF(int dimension)
{
    // Declare 4 dimensional state thing
}

void EKF::timeUpdate(MotionModel u_k)
{
    // Have the time update prediction incorporated
    // i.e. odometery, natural roll, etc.
    incorporateEstiamtePrediction();

    // Calculate
}

void EKF::correctionStep(std::vector<Measurement> z_k)
{
    for(unsigned int i = 0; i < z_k.size(); ++i) {
        incorporateCorrectionMeasurement();
    }
}
