EKF::EKF(int dimension)
{
    // Declare 4 dimensional state thing
}

void EKF::timeUpdate(MotionModel u_k)
{
    // Have the time update prediction incorporated
    // i.e. odometery, natural roll, etc.
    xhat_k_bar = xhat_k + incorporateTimeUpdate(u_k, A_k, Q_k);

    // Update error covariance matrix
    P_k = prod(A_k, prod(P_k, trans(A_k))) + Q_k;
}

void EKF::correctionStep(std::vector<Measurement> z_k)
{
    for(unsigned int i = 0; i < z_k.size(); ++i) {
        incorporateCorrection(z_k[i]);
    }
}
