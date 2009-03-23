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
public:
    // Our template dimensions allow us to use bounded arrays for storage
    // We define our own types for simpler use throughout the class

    // A vector with the number of state dimensions
    typedef boost::numeric::ublas::vector<float, boost::numeric::ublas::
                                          bounded_array<float,dimension> >
        StateVector;
    // A vector with the length of the measurement dimensions
    typedef boost::numeric::ublas::vector<float, boost::numeric::ublas::
                                          bounded_array<float,mSize> >
        MeasurementVector;

    // A square matrix with state dimension number of rows and cols
    typedef boost::numeric::ublas::matrix<float,
                                          boost::numeric::ublas::row_major,
                                          boost::numeric::ublas::
                                          bounded_array<float, dimension*
                                                        dimension> >
    StateMatrix;

    // A square matrix with measurement dimension number of rows and cols
    typedef boost::numeric::ublas::matrix<float,
                                          boost::numeric::ublas::row_major,
                                          boost::numeric::ublas::
                                          bounded_array<float, mSize*
                                                        mSize> >
    MeasurementMatrix;

    // A matrix that is of size measurement * states
    typedef boost::numeric::ublas::matrix<float,
                                          boost::numeric::ublas::row_major,
                                          boost::numeric::ublas::
                                          bounded_array<float, mSize*
                                                        dimension> >
    StateMeasurementMatrix;

protected:
    StateVector xhat_k; // Estimate Vector
    StateVector xhat_k_bar; // A priori Estimate Vector
    StateMatrix Q_k; // Input noise covariance matrix
    StateMatrix A_k; // Update measurement Jacobian
    StateMatrix P_k; // Uncertainty Matrix
    StateMatrix P_k_bar; // A priori uncertainty Matrix
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
    virtual StateVector associateTimeUpdate(UpdateModel u_k) = 0;
    virtual void incorporateMeasurement(Measurement z,
                                        StateMeasurementMatrix &H_k,
                                        MeasurementMatrix &R_k,
                                        MeasurementVector &V_k) = 0;
};

#endif //EKF_h_DEFINED
