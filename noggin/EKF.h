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

// Local headers
#include "MCL.h"

/**
 * EKF.cpp - An abstract class which implements the computational components of
 *           an Extended Kalman Filter.
 *
 * @date August 2008
 * @author Tucker Hermans
 */
class EKF
{
protected:
    ublas::vector<float> xhat_k; // Estimate Vector
    ublas::vector<float> xhat_k_bar; // A priori Estimate Vector
    ublas::matrix<float> Q_k; // Input noise covariance matrix
    ublas::matrix<float> A_k; // Jacobian associated with update measurement
    ublas::matrix<float> P_k; // Uncertainty Matrix

public:
    // Constructors & Destructors
    EKF(int dimension);
    virtual ~EKF();

    // Core functions
    virtual void timeUpdate(MotionModel u_k);
    virtual void correctionStep(std::vector<Observation> z_k);
    virtual void noCorrectionStep();
private:
    // Pure virtual methods to be specified by implementing class
    virtual ublas::vector<float> associateTimeUpdate(MotionModel u_k,
                                                     ublas::matrix<float>,
                                                     ublas::matrix<float>) = 0;
    virtual void incorporateCorrectionMeasurement(Observation z) = 0;
};
#endif EKF_h_DEFINED

/**
Invert a two by two matrix easily
given:
 [a b
  c d]
return:
(1/(ad - bc)) [ d -b
               -c  a]
 */
