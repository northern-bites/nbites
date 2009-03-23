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

// Local headers
#include "VisualBall.h"

// Default parameters
#define DEFAULT_BETA 3.0f
#define DEFAULT_GAMMA 2.0f

/**
 * EKF - An abstract class which implements the computational components of
 *       an Extended Kalman Filter.
 *
 * @date August 2008
 * @author Tucker Hermans
 */
template <class Measurement, class UpdateModel,
          unsigned int dimension, unsigned int mSize>
class EKF
{
protected:
    boost::numeric::ublas::vector<float, boost::numeric::ublas::
                                  bounded_array<float, dimension>
                                  > xhat_k; // Estimate Vector
    boost::numeric::ublas::vector<float, boost::numeric::ublas::
                                  bounded_array<float, dimension>
                                  > xhat_k_bar; // A priori Estimate Vector
    boost::numeric::ublas::matrix<float, boost::numeric::ublas::row_major,
                                  boost::numeric::ublas::
                                  bounded_array<float, dimension*dimension>
                                  > Q_k; // Input noise covariance matrix
    boost::numeric::ublas::matrix<float, boost::numeric::ublas::row_major,
                                  boost::numeric::ublas::
                                  bounded_array<float, dimension*dimension>
                                  > A_k; // Update measurement Jacobian
    boost::numeric::ublas::matrix<float, boost::numeric::ublas::row_major,
                                  boost::numeric::ublas::
                                  bounded_array<float, dimension*dimension>
                                  > P_k; // Uncertainty Matrix
    boost::numeric::ublas::matrix<float, boost::numeric::ublas::row_major,
                                  boost::numeric::ublas::
                                  bounded_array<float, dimension*dimension>
                                  > P_k_bar; // A priori uncertainty Matrix
    boost::numeric::ublas::identity_matrix<float> dimensionIdentity;
    const unsigned int numStates; // number of states in the kalman filter
    const unsigned int measurementSize; // dimension of the observation (z_k)

    float beta; // constant uncertainty increase
    float gamma; // scaled uncertainty increase

public:
    // Constructors & Destructors
    EKF(float _beta, float _gamma);
    virtual ~EKF() {}

    // Core functions
    virtual void timeUpdate(UpdateModel u_k);

    virtual void correctionStep(std::vector<Measurement> z_k);
    virtual void noCorrectionStep();
protected:
    // Pure virtual methods to be specified by implementing class
    virtual boost::numeric::ublas::vector<float>
        associateTimeUpdate(UpdateModel u_k) = 0;
    virtual void incorporateMeasurement(Measurement z,
                                        boost::numeric::ublas::
                                        matrix<float> &H_k,
                                        boost::numeric::ublas::
                                        matrix<float> &R_k,
                                        boost::numeric::ublas::
                                        vector<float> &V_k) = 0;
};

#endif //EKF_h_DEFINED
