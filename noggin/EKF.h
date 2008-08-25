/**
 * EKF.h - Header file for the EKF class
 *
 * @author Tucker Hermans
 */

#ifndef EKF_h_DEFINED
#define EKF_h_DEFINED

// Boost libraries
#include <boost/numeric/ublas/matrix.hpp>
#include <boost/numeric/ublas/vector.hpp>
using namespace boost::numeric;

/**
 * EKF.cpp - An abstract class which implements the computational components of
 *           an Extended Kalman Filter.
 *
 * @date August 2008
 * @author Tucker Hermans
 */
class EKF
{
private:
    ublas::vector<double> xhat_k; // Estimate Vector
    ublas::vector<double> xhat_k_bar; // A priori Estimate Vector
    ublas::matrix<double> Q_k; // Input noise covariance matrix
    ublas::matrix<double> A_k; // Jacobian associated with update measurement
    ublas::matrix<double> P_k; // 

public:
    // Constructors & Destructors
    virtual EKF(int dimension);
    virtual ~EKF();

    // Core functions
    virtual void timeUpdate(MotionModel u_k);
    virtual void correctionStep(std::vector<Measurement> z_k);
    virtual void noCorrectionStep();
private:
    // Pure virtual methods to be specified by implementing class
    virtual void associateTimeUpdate(MotionModel u_k) = 0;
    virtual void incorporateCorrectionMeasurement() = 0;
};
#endif EKF_h_DEFINED
