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
 * Measurement - A generic class for holding the basic type information required
 *               for running the correctionStep in EKF.
 */
class Measurement
{
};

/**
 * EKF - An abstract class which implements the computational components of
 *       an Extended Kalman Filter.
 *
 * @date August 2008
 * @author Tucker Hermans
 */
class EKF
{
protected:
    ublas::matrix<float> xhat_k; // Estimate Vector
    ublas::matrix<float> xhat_k_bar; // A priori Estimate Vector
    ublas::matrix<float> Q_k; // Input noise covariance matrix
    ublas::matrix<float> A_k; // Jacobian associated with update measurement
    ublas::matrix<float> P_k; // Uncertainty Matrix
    ublas::matrix<float> P_k_bar; // A priori uncertainty Matrix
    ublas::identity_matrix<float> dimensionIdentity;
    unsigned int states; // number of states in the kalman filter
    float beta; // constant uncertainty increase
    float gamma; // scaled uncertainty increase

public:
    // Constructors & Destructors
    EKF(unsigned int dimension);
    virtual ~EKF();

    // Core functions
    virtual void timeUpdate(MotionModel u_k);
    virtual void correctionStep(std::vector<Measurement> z_k);
    virtual void noCorrectionStep();
private:
    // Pure virtual methods to be specified by implementing class
    virtual ublas::matrix<float> associateTimeUpdate(MotionModel u_k) = 0;
    virtual ublas::matrix<float> incorporateCorrectionMeasurement(Measurement z,
                                                  ublas::matrix<float> &H_k,
                                                  ublas::matrix<float> &R_k)=0;
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
