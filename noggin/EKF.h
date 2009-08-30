/**
 * EKF.h - Implementation of the abstract EKF class
 *
 * @author Tucker Hermans
 */

#ifndef EKF_h_DEFINED
#define EKF_h_DEFINED
//#define DEBUG_JACOBIAN_JUNK
// Boost libraries
#include <boost/numeric/ublas/matrix.hpp>
#include <boost/numeric/ublas/vector.hpp>

#include "NBMatrixMath.h"
#include "NBMath.h"
#include <boost/numeric/ublas/io.hpp> // for cout

// Default uncertainty growth parameters
#define DEFAULT_BETA 3.0f
#define DEFAULT_GAMMA 2.0f

// Dimensions of implemented classes
#define LOC_EKF_DIMENSION 3 // Number of states in Loc EKF
#define LOC_MEASUREMENT_DIMENSION 2 // Number of dimensions in a measurement
#define BALL_EKF_DIMENSION 4 // Number of states in Ball EKF
#define BALL_MEASUREMENT_DIMENSION 2 // Number of dimensions in a measurement

static const int ACC_NUM_DIMENSIONS = 3;
static const int ACC_NUM_MEASUREMENTS = 3;

static const int ANGLE_NUM_DIMENSIONS = 2;
static const int ANGLE_NUM_MEASUREMENTS = 2;

static const int ZMP_NUM_DIMENSIONS = 2;
static const int ZMP_NUM_MEASUREMENTS = 2;
static const float DONT_PROCESS_KEY = -1337.0f;
/**
 * @brief An abstract class which implements the computational components of
 * an Extended Kalman Filter.
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
    const boost::numeric::ublas::identity_matrix<float> dimensionIdentity;
    const unsigned int numStates; // number of states in the kalman filter
    const unsigned int measurementSize; // dimension of the observation (z_k)

    StateVector betas; // constant uncertainty increase
    StateVector gammas; // scaled uncertainty increase
    int frameCounter;
public:
    // Constructors & Destructors
    EKF(float _beta, float _gamma)
        : xhat_k(dimension), xhat_k_bar(dimension),
          Q_k(dimension,dimension), A_k(dimension,dimension),
          P_k(dimension,dimension), P_k_bar(dimension,dimension),
          dimensionIdentity(dimension), numStates(dimension),
          measurementSize(mSize), betas(dimension), gammas(dimension),
          frameCounter(0) {

        // Initialize all matrix values to 0
        for(unsigned i = 0; i < dimension; ++i) {
            for(unsigned j = 0; j < dimension; ++j) {
                Q_k(i,j) = 0.0f;
                A_k(i,j) = 0.0f;
                P_k(i,j) = 0.0f;
                P_k_bar(i,j) = 0.0f;
            }
            xhat_k(i) = 0.0f;
            xhat_k_bar(i) = 0.0f;
            betas(i) = _beta;
            gammas(i) = _gamma;
        }
    }

    //Assignment operator
    //The compiler should use template matching to ensure we never
    //copy into an EKF of the wrong size;
    EKF & operator=(const EKF & other){
        if(this != &other){
            xhat_k     = other.xhat_k;
            xhat_k_bar = other.xhat_k_bar;
            Q_k        = other.Q_k;
            A_k        = other.A_k;
            P_k        = other.P_k;
            P_k_bar    = other.P_k_bar;
            betas      = other.betas;
            gammas     = other.gammas;
        }
        return *this;
    }

    virtual ~EKF() {}

    // Core functions
    virtual void timeUpdate(UpdateModel u_k) {
        ++frameCounter;
        // Have the time update prediction incorporated
        // i.e. odometery, natural roll, etc.
        StateVector deltas = associateTimeUpdate(u_k);
        xhat_k_bar = xhat_k + deltas;

        // Calculate the uncertainty growth for the current update
        for(unsigned int i = 0; i < numStates; ++i) {
            Q_k(i,i) = betas(i) + gammas(i) * deltas(i) * deltas(i);
        }

        // Update error covariance matrix
        StateMatrix newP = prod(P_k, trans(A_k));
        P_k_bar = prod(A_k, newP) + Q_k;

#ifdef DEBUG_JACOBIAN_JUNK
        bool outputInfos = false;
        for(int a = 0; a < 3; a++) {
            for (int b = 0; b < 3; b++) {
                if (P_k_bar(a,b) < 0) {
                    outputInfos = true;
                }
            }
        }
        if(outputInfos) {
            std::cout << "Frame # " << frameCounter << std::endl;
            std::cout << "\t x uncert is " << P_k_bar(0,0) << std::endl;
            std::cout << "\t y uncert is " << P_k_bar(1,1) << std::endl;
            std::cout << "\t h uncert is " << P_k_bar(2,2) << std::endl;
            std::cout << "\t Q_k is " << Q_k << std::endl;
            std::cout << "\t P_k_bar is " << P_k_bar << std::endl;
            std::cout << "\t P_k is " << P_k << std::endl;
            std::cout << "\t A_k is " << A_k << std::endl;
            std::cout << "\t deltas are " << deltas << std:: endl;
            std::cout << "\t betas are " << betas << std:: endl;
            std::cout << "\t gammas are " << gammas << std:: endl;
        }
#endif
    }

    virtual void correctionStep(std::vector<Measurement> z_k) {
        // Necessary computational matrices
        // Kalman gain matrix
        StateMeasurementMatrix K_k =
            boost::numeric::ublas::scalar_matrix<float>(numStates,
                                        measurementSize,
                                        0.0f);
        // Observation jacobian
        StateMeasurementMatrix H_k =
            boost::numeric::ublas::scalar_matrix<float>(measurementSize,
                                        numStates, 0.0f);
        // Assumed error in measurment sensors
        MeasurementMatrix R_k = boost::numeric::ublas::scalar_matrix<float>(
            measurementSize, measurementSize, 0.0f);
        // Measurement invariance
        MeasurementVector v_k(measurementSize);

        // Incorporate all correction observations
        for(unsigned int i = 0; i < z_k.size(); ++i) {
            incorporateMeasurement(z_k[i], H_k, R_k, v_k);

            if (R_k(0,0) == DONT_PROCESS_KEY) {
                continue;
            }
            // Calculate the Kalman gain matrix
            StateMeasurementMatrix pTimesHTrans = prod(P_k_bar, trans(H_k));

            if(measurementSize == 2){
                K_k = prod(pTimesHTrans,
                           NBMath::invert2by2(prod(H_k, pTimesHTrans) + R_k));
            }else{
                MeasurementMatrix temp = prod(H_k, pTimesHTrans) + R_k;
                MeasurementMatrix inv =
                    NBMath::solve(temp,
                                  boost::numeric::ublas::identity_matrix<float>(
                                      measurementSize));
                K_k = prod(pTimesHTrans, inv);
            }

            // Use the Kalman gain matrix to determine the next estimate
            xhat_k_bar = xhat_k_bar + prod(K_k, v_k);

            // Update associate uncertainty
            P_k_bar = prod(dimensionIdentity - prod(K_k,H_k), P_k_bar);
        }

        // Allow implementing classes to do things before copying the vectors
        // For most implementations this should be ignored
        beforeCorrectionFinish();

        xhat_k = xhat_k_bar;
        P_k = P_k_bar;
    }

    virtual void noCorrectionStep(void) {
        // Set current estimates to a priori estimates
        xhat_k = xhat_k_bar;
        P_k = P_k_bar;
    }

    /**
     * Allow implementing classes to do things before copying the vectors
     * For most implementations this should be ignored
     */
    virtual void beforeCorrectionFinish(void) {}

protected:
    // Pure virtual methods to be specified by implementing class
    virtual StateVector associateTimeUpdate(UpdateModel u_k) = 0;
    virtual void incorporateMeasurement(Measurement z,
                                        StateMeasurementMatrix &H_k,
                                        MeasurementMatrix &R_k,
                                        MeasurementVector &V_k) = 0;
    virtual void reset() {}
    virtual bool testForNaNReset() {
        for (unsigned int i = 0; i < numStates; ++i) {
            for (unsigned int j = 0; j < numStates; ++j) {
                if(isnan(P_k(i,j)) || isnan(P_k_bar(i,j)) ||
                   isnan(xhat_k(i)) || isnan(xhat_k_bar(i)) ||
                   isinf(P_k(i,j)) || isinf(P_k_bar(i,j)) ||
                   isinf(xhat_k(i)) || isinf(xhat_k_bar(i))) {
                    std::cout << "Reseting EKF do to nan or inf value."
                              << std::endl;
                    reset();
                    return true;
                }
            }
        }
        return false;
    }
};

#endif //EKF_h_DEFINED
