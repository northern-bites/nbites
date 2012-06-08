#ifndef _MultiMeasurementEKF_h_DEFINED
#define _MultiMeasurementEKF_h_DEFINED

#include "EKF.h"
#include <vector>

namespace ekf {

/**
 * Declare the measurment types for a given measurement number
 */
#define MEASUREMENT_MATRIX_TYPES(n)                                          \
    /* A vector with the length of the measurement dimensions */             \
    typedef boost::numeric::ublas::vector<float, boost::numeric::ublas::     \
                                          bounded_array<float,mSize##n> >    \
    MeasurementVector##n;                                                    \
                                                                             \
    /* A square matrix with measurement dimension number of rows and cols */ \
    typedef boost::numeric::ublas::matrix<float,                             \
                                          boost::numeric::ublas::row_major,  \
                                          boost::numeric::ublas::            \
                                          bounded_array<float, mSize##n*     \
                                                        mSize##n> >          \
    MeasurementMatrix##n;                                                    \
                                                                             \
    /* A matrix that is of size measurement * states */                      \
    typedef boost::numeric::ublas::matrix<float,                             \
                                          boost::numeric::ublas::row_major,  \
                                          boost::numeric::ublas::            \
                                          bounded_array<float, mSize##n*     \
                                                        numStates> >         \
    StateMeasurementMatrix##n;


/**
 * Declare the measurement matrices associated with an observation
 * parameter
 */
#define MEASUREMENT_MATRICES(n)                 \
    StateMeasurementMatrix##n K_k##n, H_k##n;   \
    MeasurementMatrix##n R_k##n;                \
    MeasurementVector##n v_k##n;

/**
 * For use in the constructor list, it initializes the measurement
 * matrices in the same order as they are declared in the
 * MEASUREMENT_MATRICES macro.
 */
#define INIT_MEASUREMENT_MATRICES(n)            \
        K_k##n(numStates, mSize##n, 0.0f),      \
        H_k##n(mSize##n, numStates, 0.0f),      \
        R_k##n(mSize##n, mSize##n, 0.0f),       \
        v_k##n(mSize##n)



/**
 *
 * An EKF which can take in two observation types and fuse their
 * information to get a single consistent update.
 *
 */
template <class Measurement1,
          unsigned int mSize1,
          class Measurement2,
          unsigned int mSize2,
          class UpdateModel,
          unsigned int numStates>
class TwoMeasurementEKF
{
public:
   // Our template dimensions allow us to use bounded arrays for storage
   // We define our own types for simpler use throughout the class

   // A vector with the number of state dimensions
   typedef boost::numeric::ublas::vector<float, boost::numeric::ublas::
                                         bounded_array<float,numStates> >
   StateVector;


   // A square matrix with state dimension number of rows and cols
   typedef boost::numeric::ublas::matrix<float,
                                         boost::numeric::ublas::row_major,
                                         boost::numeric::ublas::
                                         bounded_array<float, numStates*
                                                       numStates> >
   StateMatrix;


   // MEASUREMENT SIZE DEPENDANT VECTORS AND MATRICES
    MEASUREMENT_MATRIX_TYPES(1);
    MEASUREMENT_MATRIX_TYPES(2);

protected:
   StateVector xhat_k; // Estimate Vector
   StateVector xhat_k_bar; // A priori Estimate Vector
   StateMatrix Q_k; // Input noise covariance matrix
   StateMatrix A_k; // Update measurement Jacobian
   StateMatrix P_k; // Uncertainty Matrix
   StateMatrix P_k_bar; // A priori uncertainty Matrix
   StateVector betas; // constant uncertainty increase
   StateVector gammas; // scaled uncertainty increase

   // Necessary computational matrices for correction step
    MEASUREMENT_MATRICES(1);
    MEASUREMENT_MATRICES(2);

   const boost::numeric::ublas::identity_matrix<float> dimensionIdentity;

   int frameCounter;

public:
   // Constructors & Destructors
   TwoMeasurementEKF(float _beta, float _gamma)
       : xhat_k(numStates), xhat_k_bar(numStates),
         Q_k(numStates,numStates), A_k(numStates,numStates),
         P_k(numStates,numStates), P_k_bar(numStates,numStates),
         betas(numStates), gammas(numStates),
         INIT_MEASUREMENT_MATRICES(1),
         INIT_MEASUREMENT_MATRICES(2),
         dimensionIdentity(numStates), frameCounter(0)
      {

          // Initialize all matrix values to 0
          for(unsigned i = 0; i < numStates; ++i) {
              for(unsigned j = 0; j < numStates; ++j) {
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
   //copy into an TwoMeasurementEKF of the wrong size;
   TwoMeasurementEKF & operator=(const TwoMeasurementEKF & other){
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

   virtual ~TwoMeasurementEKF() {}

   StateVector getState() const { return xhat_k; }
   StateMatrix getStateUncertainty() const { return P_k; }

   // Core functions
   virtual void timeUpdate(UpdateModel u_k) {
       ++frameCounter;
       // Have the time update prediction incorporated
       // i.e. odometery, natural roll, etc.
       const StateVector deltas = associateTimeUpdate(u_k);
       xhat_k_bar = xhat_k + deltas;

       // Calculate the uncertainty growth for the current update
       for(unsigned int i = 0; i < numStates; ++i) {
           Q_k(i,i) = betas(i) + gammas(i) * deltas(i) * deltas(i);
       }

       // Update error covariance matrix
       const StateMatrix newP = prod(P_k, trans(A_k));
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

   virtual void correctionStep(const std::vector<Measurement1>& z_k1,
                               const std::vector<Measurement2>& z_k2) {

       // Incorporate all correction observations
       typename std::vector<Measurement1>::const_iterator m1;
       typename std::vector<Measurement2>::const_iterator m2;

       for(m1 = z_k1.begin(); m1 != z_k1.end(); ++m1) {
           correctionStep<mSize1, Measurement1,
              StateMeasurementMatrix1,
              MeasurementMatrix1,
              MeasurementVector1>(*m1, H_k1, K_k1, R_k1, v_k1);
       }

       for(m2 = z_k2.begin(); m2 != z_k2.end(); ++m2) {
           correctionStep<mSize2, Measurement2,
              StateMeasurementMatrix2,
              MeasurementMatrix2,
              MeasurementVector2>(*m2, H_k2, K_k2, R_k2, v_k2);
       }

       // Allow implementing classes to do things before copying
       // the vectors. For most implementations this should be
       // ignored
       beforeCorrectionFinish();

       // Copy the updated states onto the EKF's state
       updateState();
   }

   template<int mSize, typename MeasT,
            typename StateMeasMT, typename MeasMT, typename MeasVT>
   void correctionStep(const MeasT& z_k,
                       StateMeasMT& H_k,
                       StateMeasMT& K_k,
                       MeasMT& R_k,
                       MeasVT& V_k) {
       incorporateMeasurement(z_k, H_k, R_k, V_k);

       if (R_k(0,0) == DONT_PROCESS_KEY) {
           return;
       }

       // Calculate the Kalman gain matrix
       const StateMeasMT pTimesHTrans =
          prod(P_k_bar, trans(H_k));

       if(mSize == 2){
           K_k = prod(pTimesHTrans,
                      NBMath::invert2by2(prod(H_k,
                                              pTimesHTrans) + R_k));
       } else {
           const MeasMT inv =
              NBMath::solve(prod(H_k, pTimesHTrans) + R_k,
                            boost::numeric::ublas::identity_matrix<float>(
                                mSize));
           K_k = prod(pTimesHTrans, inv);
       }

       // Use the Kalman gain matrix to determine the next estimate
       xhat_k_bar = xhat_k_bar + prod(K_k, V_k);

       // Update associate uncertainty
       P_k_bar = prod(dimensionIdentity - prod(K_k,H_k), P_k_bar);
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

   virtual void updateState() {
       xhat_k = xhat_k_bar;
       P_k = P_k_bar;
   }

protected:
   // Pure virtual methods to be specified by implementing class
   virtual StateVector associateTimeUpdate(const UpdateModel& u_k) = 0;
   virtual void incorporateMeasurement(const Measurement1& z,
                                       StateMeasurementMatrix1 &H_k,
                                       MeasurementMatrix1 &R_k,
                                       MeasurementVector1 &V_k) = 0;

   virtual void incorporateMeasurement(const Measurement2& z,
                                       StateMeasurementMatrix2 &H_k,
                                       MeasurementMatrix2 &R_k,
                                       MeasurementVector2 &V_k) = 0;

   virtual void reset() {}
   virtual bool testForNaNReset() {
       for (unsigned int i = 0; i < numStates; ++i) {
           for (unsigned int j = 0; j < numStates; ++j) {
               if(isnan(P_k(i,j)) || isnan(P_k_bar(i,j)) ||
                  isnan(xhat_k(i)) || isnan(xhat_k_bar(i)) ||
                  isinf(P_k(i,j)) || isinf(P_k_bar(i,j)) ||
                  isinf(xhat_k(i)) || isinf(xhat_k_bar(i))) {
                   std::cout << "Resetting EKF due to nan or inf value."
                             << std::endl;
                   std::cout << "xhat_k: " << xhat_k << std::endl;
                   std::cout << "xhat_k_bar: " << xhat_k_bar << std::endl;
                   std::cout << "P_k: " << P_k << std::endl;
                   std::cout << "P_k_bar: " << P_k_bar << std::endl;
                   reset();
                   return true;
               }
           }
       }
       return false;
   }
};
}
#endif /* _MultiMeasurementEKF_h_DEFINED */
