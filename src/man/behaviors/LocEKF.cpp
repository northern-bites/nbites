#include "LocEKF.h"
#include <boost/numeric/ublas/io.hpp> // for cout
#include "FieldConstants.h"
//#define DEBUG_LOC_EKF_INPUTS
//#define DEBUG_STANDARD_ERROR
using namespace boost::numeric;
using namespace boost;
using namespace std;
using namespace NBMath;

// Parameters
// Measurement conversion form
const float LocEKF::USE_CARTESIAN_DIST = 50.0f;
// Uncertainty
const float LocEKF::BETA_LOC = 1.0f;
const float LocEKF::GAMMA_LOC = 0.1f;
const float LocEKF::BETA_ROT = M_PI_FLOAT/64.0f;
const float LocEKF::GAMMA_ROT = 0.1f;

// Default initialization values
const float LocEKF::INIT_LOC_X = CENTER_FIELD_X;
const float LocEKF::INIT_LOC_Y = CENTER_FIELD_Y;
const float LocEKF::INIT_LOC_H = 0.0f;
const float LocEKF::INIT_BLUE_GOALIE_LOC_X = (FIELD_WHITE_LEFT_SIDELINE_X +
                                              GOALBOX_DEPTH / 2.0f);
const float LocEKF::INIT_BLUE_GOALIE_LOC_Y = CENTER_FIELD_Y;
const float LocEKF::INIT_BLUE_GOALIE_LOC_H = 0.0f;
const float LocEKF::INIT_RED_GOALIE_LOC_X = (FIELD_WHITE_RIGHT_SIDELINE_X -
                                             GOALBOX_DEPTH / 2.0f);
const float LocEKF::INIT_RED_GOALIE_LOC_Y = CENTER_FIELD_Y;
const float LocEKF::INIT_RED_GOALIE_LOC_H = M_PI_FLOAT;
// Uncertainty limits
const float LocEKF::X_UNCERT_MAX = FIELD_WIDTH / 2.0f;
const float LocEKF::Y_UNCERT_MAX = FIELD_HEIGHT / 2.0f;
const float LocEKF::H_UNCERT_MAX = 4*M_PI_FLOAT;
const float LocEKF::X_UNCERT_MIN = 1.0e-6f;
const float LocEKF::Y_UNCERT_MIN = 1.0e-6f;
const float LocEKF::H_UNCERT_MIN = 1.0e-6f;
// Initial estimates
const float LocEKF::INIT_X_UNCERT = X_UNCERT_MAX / 2.0f;
const float LocEKF::INIT_Y_UNCERT = Y_UNCERT_MAX / 2.0f;
const float LocEKF::INIT_H_UNCERT = M_PI_FLOAT * 2.0f;
// Estimate limits
const float LocEKF::X_EST_MIN = 0.0f;
const float LocEKF::Y_EST_MIN = 0.0f;
const float LocEKF::X_EST_MAX = FIELD_GREEN_WIDTH;
const float LocEKF::Y_EST_MAX = FIELD_GREEN_HEIGHT;

using namespace ekf;

/**
 * Initialize the localization EKF class
 *
 * @param initX Initial x estimate
 * @param initY Initial y estimate
 * @param initH Initial heading estimate
 * @param initXUncert Initial x uncertainty
 * @param initYUncert Initial y uncertainty
 * @param initHUncert Initial heading uncertainty
 */
LocEKF::LocEKF(float initX, float initY, float initH,
               float initXUncert,float initYUncert, float initHUncert)
    : EKF<Observation, MotionModel, LOC_EKF_DIMENSION,
          LOC_MEASUREMENT_DIMENSION>(BETA_LOC,GAMMA_LOC), LocSystem(),
      lastOdo(0,0,0),
      lastObservations(0), useAmbiguous(true),
      R_pred_k(LOC_MEASUREMENT_DIMENSION, LOC_MEASUREMENT_DIMENSION, 0.0f)
{
    // ones on the diagonal
    A_k(0,0) = 1.0;
    A_k(1,1) = 1.0;
    A_k(2,2) = 1.0;

    // Setup initial values
    setXEst(initX);
    setYEst(initY);
    setHEst(initH);
    setXUncert(initXUncert);
    setYUncert(initYUncert);
    setHUncert(initHUncert);

    betas(2) = BETA_ROT;
    gammas(2) = GAMMA_ROT;

#ifdef DEBUG_LOC_EKF_INPUTS
    cout << "Initializing LocEKF with: " << *this << endl;
#endif
}

/**
 * Set the EKF to be identical to the given EKF
 *
 * @param other EKF to be copied
 */
void LocEKF::copyEKF(const LocEKF& other)
{
    if(this != &other){
        xhat_k     = other.xhat_k;
        xhat_k_bar = other.xhat_k_bar;
        Q_k        = other.Q_k;
        A_k        = other.A_k;
        P_k        = other.P_k;
        P_k_bar    = other.P_k_bar;
        betas      = other.betas;
        gammas     = other.gammas;
        setProbability(other.getProbability());
    }

}


/**
 * Merges the current EKF with the given EKF in such a way as to preserve the
 * the probability distribution.
 *
 * @param other The EKF that this EKF will be merged with.
 */
void LocEKF::mergeEKF(const LocEKF& other)
{
    // Make sure that the new probability is not 0 or rediculously close to it.
    const double newProbability = max(probability + other.getProbability(),
                                      0.000001);

    StateVector new_xhat_k(LOC_EKF_DIMENSION);

    const int DRIFT_PROB_MAX_DIFF = 10;
    if ( other.getProbability() > probability * DRIFT_PROB_MAX_DIFF ){
        xhat_k = other.getState();
        P_k = other.getStateUncertainty();
    } else if ( probability > other.getProbability() * DRIFT_PROB_MAX_DIFF) {
        new_xhat_k = xhat_k;
    } else {
        new_xhat_k(0) = static_cast<float>( (1/newProbability)*
                                            (probability * getXEst() +
                                             other.getProbability() * other.getXEst()) );
        new_xhat_k(1) = static_cast<float>( (1/newProbability)*
                                            (probability * getYEst() +
                                             other.getProbability() * other.getYEst()) );
        new_xhat_k(2) = static_cast<float>( (1/newProbability)*
                                            (probability * getHEst() +
                                             other.getProbability() * other.getHEst()) );

        const StateVector diffA = (xhat_k - new_xhat_k);
        const StateMatrix a = P_k + outer_prod(diffA,
                                               trans(diffA));

        const StateVector diffB = (other.getState() - new_xhat_k);
        const StateMatrix b = (other.getStateUncertainty() +
                               outer_prod(diffB, trans(diffB)));

        P_k = (((probability * a) + (other.getProbability() * b))
               /
               newProbability);

        xhat_k = new_xhat_k;
    }

    setProbability(newProbability);
}

/**
 * Reset the EKF to a starting configuration
 */
void LocEKF::reset()
{
    resetLocTo(INIT_LOC_X,
               INIT_LOC_Y,
               INIT_LOC_H);
}

/**
 * Reset the EKF to a blue goalie starting configuration
 */
void LocEKF::blueGoalieReset()
{
    resetLocTo(INIT_BLUE_GOALIE_LOC_X,
               INIT_BLUE_GOALIE_LOC_Y,
               INIT_BLUE_GOALIE_LOC_H);

}

/**
 * Reset the EKF to a red goalie starting configuration
 */
void LocEKF::redGoalieReset()
{
    resetLocTo(INIT_RED_GOALIE_LOC_X,
               INIT_RED_GOALIE_LOC_Y,
               INIT_RED_GOALIE_LOC_H);
}

/**
 * Reset the EKF to the given position. Gives it a large uncertainity.
 *
 * @param x,y,h the position to set the EKF to.
 */
void LocEKF::resetLocTo(float x, float y, float h)
{
    setXEst(x);
    setYEst(y);
    setHEst(subPIAngle(h));
    setXUncert(INIT_X_UNCERT);
    setYUncert(INIT_Y_UNCERT);
    setHUncert(INIT_H_UNCERT);
}

/**
 * Method to deal with updating the entire loc model
 *
 * @param u The odometry since the last frame
 * @param Z The observations from the current frame
 */
void LocEKF::updateLocalization(const MotionModel& u,
                                const vector<Observation>& Z)
{
#ifdef DEBUG_LOC_EKF_INPUTS
    printBeforeUpdateInfo();
#endif

    odometryUpdate(u);
    applyObservations(Z);
    endFrame();

#ifdef DEBUG_LOC_EKF_INPUTS
    printAfterUpdateInfo();
#endif
}

// Update expected position based on odometry
void LocEKF::odometryUpdate(const MotionModel& u)
{
    timeUpdate(u);
    limitAPrioriUncert();
    lastOdo = u;
}

/**
 * Apply a whole set of observations from one time frame.
 */
void LocEKF::applyObservations(vector<Observation> Z)
{
    lastObservations = Z;

    if (! useAmbiguous) {
        // Remove ambiguous observations
        vector<Observation>::iterator iter = Z.begin();
        while( iter != Z.end() ) {
            if (iter->getNumPossibilities() > 1 ) {
                iter = Z.erase( iter );
            } else {
                ++iter;
            }
        }
    }

    // Correct step based on the observed stuff
    if (Z.size() > 0) {
        correctionStep(Z);
    } else {
        noCorrectionStep();
    }
    //limitPosteriorUncert();
}


/**
 * Apply an individual observation to the EKF.
 */
bool LocEKF::applyObservation(const Observation& Z)
{
#ifdef DEBUG_LOC_EKF_INPUTS
    printBeforeUpdateInfo();
#endif

    correctionStep(Z);
    updateState();

#ifdef USE_MM_LOC_EKF
    return updateProbability(Z);
#else
    return true;
#endif

}

#ifdef USE_MM_LOC_EKF
bool LocEKF::updateProabbility(const Observation& Z)
{

    if (R_k(0,0) == DONT_PROCESS_KEY)
        return true;

    const MeasurementMatrix measurementVar = R_k + R_pred_k;

    MeasurementMatrix measurementVarInv = measurementVar;
    const double denom = (-measurementVar(0,1) * measurementVar(1,0) +
                          measurementVar(0,0) * measurementVar(1,1));

    if (denom < 0.0001){
        probability *= 0.0001;
        return true;
    }

    measurementVarInv(0,0) = measurementVar(1,1)/denom;
    measurementVarInv(0,1) = -measurementVar(0,1)/denom;
    measurementVarInv(1,0) = -measurementVar(1,0)/denom;
    measurementVarInv(1,1) = measurementVar(0,0)/denom;

    v_k(0) = abs(v_k(0));
    v_k(1) = abs(v_k(1));
    v_k(2) = abs(v_k(2));

    // We need the measurement innovation or invariance, aka v_k
    const double exponent = -0.5 * inner_prod(trans(v_k),
                                              prod(measurementVarInv, v_k));

    if (abs(exponent) > 2.25)
        return true;

    double detMeasVar = (-measurementVar(0,1) * measurementVar(1,0) +
                         measurementVar(0,0) * measurementVar(1,1));
    if (detMeasVar < 0.0001)
        detMeasVar = 1e-08;
    const double coefficient = 1 / sqrt( //pow(2 * PI, LOC_MEASUREMENT_DIMENSION) *
        detMeasVar );

    const double outlierProb = 0.08;
    const double probCo = (1-outlierProb)*(pow(M_E, exponent)) + outlierProb;

    // cout << "probCo= " << probCo << "\tprob:" << probability << endl;

    probability *= probCo;
    return false;
}

#endif




/**
 * Performs final cleanup at the end of a time step. Clips robot position
 * to be on the field and tests fof NaN values.
 */
void LocEKF::endFrame()
{

    // Clip values if our estimate is off the field
    clipRobotPose();
    if (testForNaNReset()) {
        cout << "LocEKF reset to: "<< *this << endl;
        cout << "\tLast odo is: " << lastOdo << endl;
        cout << endl;
    }
}


/**
 * Method incorporate the expected change in loc position from the last
 * frame.  Updates the values of the covariance matrix Q_k and the jacobian
 * A_k.
 *
 * @param u The motion model of the last frame.  Ignored for the loc.
 * @return The expected change in loc position (x,y, xVelocity, yVelocity)
 */
EKF<Observation, MotionModel,
    LOC_EKF_DIMENSION, LOC_MEASUREMENT_DIMENSION>::StateVector
LocEKF::associateTimeUpdate(MotionModel u)
{
#ifdef DEBUG_LOC_EKF_INPUTS
    cout << "\t\t\tUpdating Odometry of " << u << endl;
#endif

    // Calculate the assumed change in loc position
    // Assume no decrease in loc velocity
    StateVector deltaLoc(LOC_EKF_DIMENSION);
    const float h = getHEst();
    float sinh, cosh;
    sincosf(h, &sinh, &cosh);

    deltaLoc(0) = u.deltaF * cosh - u.deltaL * sinh;
    deltaLoc(1) = u.deltaF * sinh + u.deltaL * cosh;
    deltaLoc(2) = u.deltaR;

    A_k(0,2) =  0;//-u.deltaF * sinh - u.deltaL * cosh;
    A_k(1,2) =  0;//u.deltaF * cosh - u.deltaL * sinh;

    return deltaLoc;
}

/**
 * Method to deal with incorporating a loc measurement into the EKF
 *
 * @param z the measurement to be incorporated
 * @param H_k the jacobian associated with the measurement, to be filled out
 * @param R_k the covariance matrix of the measurement, to be filled out
 * @param V_k the measurement invariance
 *
 * @return the measurement invariance
 */
void LocEKF::incorporateMeasurement(const Observation& z,
                                    StateMeasurementMatrix &H_k,
                                    MeasurementMatrix &R_k,
                                    MeasurementVector &V_k)
{
#ifdef DEBUG_LOC_EKF_INPUTS
    cout << "\t\t\tIncorporating measurement " << z << endl;
#endif

    const int obsIndex = findBestLandmark(z);

    // No landmark is close enough, don't attempt to use one
    if (obsIndex < 0) {
        R_k(0,0) = DONT_PROCESS_KEY;
        return;
    }

    if (z.getVisDistance() < USE_CARTESIAN_DIST) {
        incorporateCartesianMeasurement( obsIndex, z, H_k, R_k, V_k);
    } else {
        incorporatePolarMeasurement( obsIndex, z, H_k, R_k, V_k);
    }

    // Calculate the standard error of the measurement
    const StateMeasurementMatrix newP = prod(P_k, trans(H_k));
    MeasurementMatrix se = prod(H_k, newP) + R_k;
    se(0,0) = sqrt(se(0,0));
    se(1,1) = sqrt(se(1,1));

    // Ignore observations based on standard error
    if ( se(0,0)*6.0f < abs(V_k(0))) {
#ifdef DEBUG_STANDARD_ERROR
        cout << "\t Ignoring measurement " << endl;
        cout << "\t Standard error is " << se << endl;
        cout << "\t Invariance is " << abs(V_k(0))*5 << endl;
#endif
        R_k(0,0) = DONT_PROCESS_KEY;
    }

}

void LocEKF::incorporateCartesianMeasurement(int obsIndex,
                                             const Observation& z,
                                             StateMeasurementMatrix &H_k,
                                             MeasurementMatrix &R_k,
                                             MeasurementVector &V_k)
{

#ifdef DEBUG_LOC_EKF_INPUTS
    cout << "\t\t\tUsing cartesian " << endl;
#endif
    // Convert our sighting to cartesian coordinates
    MeasurementVector z_x(2);
    z_x(0) = z.getVisDistance() * cos(z.getVisBearing());
    z_x(1) = z.getVisDistance() * sin(z.getVisBearing());

    // Get expected values of the post
    const float x_b = z.getPointPossibilities()[obsIndex].x;
    const float y_b = z.getPointPossibilities()[obsIndex].y;

    const float x = xhat_k_bar(0);
    const float y = xhat_k_bar(1);
    const float h = xhat_k_bar(2);

    float sinh, cosh;
    sincosf(h, &sinh, &cosh);

    MeasurementVector d_x(2);
    d_x(0) = (x_b - x) * cosh + (y_b - y) * sinh;
    d_x(1) = -(x_b - x) * sinh + (y_b - y) * cosh;

    // Calculate invariance
    V_k = z_x - d_x;

    // Calculate jacobians
    H_k(0,0) = -cosh;
    H_k(0,1) = -sinh;
    H_k(0,2) = -(x_b - x) * sinh + (y_b - y) * cosh;

    H_k(1,0) = sinh;
    H_k(1,1) = -cosh;
    H_k(1,2) = -(x_b - x) * cosh - (y_b - y) * sinh;

    // Update the measurement covariance matrix
    const float dist_sd_2 = pow(z.getDistanceSD(), 2);
    const float v = dist_sd_2 * sin(z.getVisBearing()) * cos(z.getVisBearing());

    R_k(0,0) = dist_sd_2 * pow(cos(z.getVisBearing()), 2);
    R_k(0,1) = v;
    R_k(1,0) = v;
    R_k(1,1) = dist_sd_2 * pow(sin(z.getVisBearing()), 2);

    const double uncertX = getXUncert();
    const double uncertY = getYUncert();
    const double uncertH = getHUncert();

    const double sinhUncert = uncertH * cosh;
    const double coshUncert = uncertH * sinh;

    const float xInvariance = abs(x_b -x);
    const float yInvariance = abs(y_b -y);

#ifdef USE_MM_LOC_EKF
    R_pred_k(0,0) = ((uncertX / xInvariance + coshUncert / cosh) +
                     (uncertY / yInvariance + sinhUncert / sinh));
    R_pred_k(0,1) = 0;
    R_pred_k(1,0) = 0;
    R_pred_k(1,1) = ((uncertX / xInvariance + sinhUncert / sinh) +
                     (uncertY / yInvariance + coshUncert / cosh));
#endif


#ifdef DEBUG_LOC_EKF_INPUTS
    cout << "\t\t\tR vector is" << R_k << endl;
    cout << "\t\t\tH vector is" << H_k << endl;
    cout << "\t\t\tV vector is" << V_k << endl;
    cout << "\t\t\t\td vector is" << d_x << endl;
    cout << "\t\t\t\t\tx est is " << x << endl;
    cout << "\t\t\t\t\ty est is " << y << endl;
    cout << "\t\t\t\t\th est is " << h << endl;
    cout << "\t\t\t\t\tx_b est is " << x_b << endl;
    cout << "\t\t\t\t\ty_b est is " << y_b << endl;
#endif
}

void LocEKF::incorporatePolarMeasurement(int obsIndex,
                                         const Observation& z,
                                         StateMeasurementMatrix &H_k,
                                         MeasurementMatrix &R_k,
                                         MeasurementVector &V_k)
{
#ifdef DEBUG_LOC_EKF_INPUTS
    cout << "\t\t\tUsing polar " << endl;
#endif

    // Get the observed range and bearing
    MeasurementVector z_x(2);
    z_x(0) = z.getVisDistance();
    z_x(1) = z.getVisBearing();

    // Get expected values of the post
    PointLandmark bestPossibility = z.getPointPossibilities()[obsIndex];
    const float x_b = bestPossibility.x;
    const float y_b = bestPossibility.y;
    MeasurementVector d_x(2);

    const float x = xhat_k_bar(0);
    const float y = xhat_k_bar(1);
    const float h = xhat_k_bar(2);

    d_x(0) = static_cast<float>(hypot(x - x_b, y - y_b));
    d_x(1) = safe_atan2(y_b - y,
                        x_b - x) - h;
    d_x(1) = NBMath::subPIAngle(d_x(1));

    // Calculate invariance
    V_k       = z_x - d_x;
    V_k(1) = NBMath::subPIAngle(V_k(1));

    // Calculate jacobians
    H_k(0,0) = (x - x_b) / d_x(0);
    H_k(0,1) = (y - y_b) / d_x(0);
    H_k(0,2) = 0;

    H_k(1,0) = (y_b - y) / (d_x(0)*d_x(0));
    H_k(1,1) = (x - x_b) / (d_x(0)*d_x(0));
    H_k(1,2) = -1;

    // Update the measurement covariance matrix
    R_k(0,0) = z.getDistanceSD() * z.getDistanceSD();
    R_k(0,1) = 0.0;
    R_k(1,0) = 0.0;
    R_k(1,1) = z.getBearingSD() * z.getBearingSD();

#ifdef USE_MM_LOC_EKF
    const double uncertX = getXUncert();
    const double uncertY = getYUncert();
    const double uncertH = getHUncert();

    const float xInvariance = abs(x - x_b);
    const float yInvariance = abs(y - y_b);

    R_pred_k(0,0) = ((uncertX/xInvariance) + (uncertY/yInvariance) /
                     (xInvariance*xInvariance + yInvariance*yInvariance));
    R_pred_k(0,1) = 0;
    R_pred_k(1,0) = 0;
    R_pred_k(1,1) = (((uncertY / yInvariance) + (uncertX / xInvariance)) /
                     (yInvariance / xInvariance) + uncertH);
#endif

#ifdef DEBUG_LOC_EKF_INPUTS
    cout << "\t\t\tR vector is" << R_k << endl;
    cout << "\t\t\tH vector is" << H_k << endl;
    cout << "\t\t\tV vector is" << V_k << endl;
    cout << "\t\t\t\td vector is" << d_x << endl;
    cout << "\t\t\t\t\tx est is " << x << endl;
    cout << "\t\t\t\t\ty est is " << y << endl;
    cout << "\t\t\t\t\th est is " << h << endl;
    cout << "\t\t\t\t\tx_b est is " << x_b << endl;
    cout << "\t\t\t\t\ty_b est is " << y_b << endl;
#endif
}

/**
 * Given an observation with multiple possibilities, we return the observation
 * with the best possibility as the first element of the vector
 *
 * @param z The observation to be fixed
 */
int LocEKF::findBestLandmark(const Observation& z)
{

    // Hack in here for if the vision system cannot identify this observation
    // (no possible identities)
    // @TODO this should never happen, so fix vision system to stop it from happening.
    if (z.getNumPossibilities() == 0){
        return -1;
    } else if (z.getNumPossibilities() == 1){
        return 0;
    } else {
        return findNearestNeighbor(z);
    }
}

/**
 * Find the point closest to our observation.
 *
 * @param The observed point
 * @return Index of the nearest neighbor
 */
int LocEKF::findNearestNeighbor(const Observation& z)
{
    vector<PointLandmark> possiblePoints = z.getPointPossibilities();
    float minDivergence = 250.0f;
    int minIndex = -1;
    for (unsigned int i = 0; i < possiblePoints.size(); ++i) {
        float divergence = getDivergence(z, possiblePoints[i]);

        if (divergence < minDivergence) {
            minDivergence = divergence;
            minIndex = i;
        }
    }
    return minIndex;
}

/**
 * Get the divergence between an observation and a possible point landmark
 *
 * @param z The observation measurement to be examined
 * @param pt The possible point to check against
 *
 * @return The divergence value
 */
float LocEKF::getDivergence(const Observation& z, const PointLandmark& pt)
{
    const float x = xhat_k_bar(0);
    const float y = xhat_k_bar(1);
    const float h = xhat_k_bar(2);
    float x_b_r = z.getVisDistance() * cos(z.getVisBearing());
    float y_b_r = z.getVisDistance() * sin(z.getVisBearing());

    float sinh, cosh;
    sincosf(h, &sinh, &cosh);

    float x_b  = (pt.x - x) * cosh + (pt.y - y) * sinh;
    float y_b = -(pt.x - x) * sinh + (pt.y - y) * cosh;

    return static_cast<float>(hypot(x_b_r - x_b, y_b_r - y_b));
}

/**
 * Method to ensure that uncertainty does not grow without bound
 */
void LocEKF::limitAPrioriUncert()
{
    // Check x uncertainty
    if(P_k_bar(0,0) > X_UNCERT_MAX) {
        P_k_bar(0,0) = X_UNCERT_MAX;
    }
    // Check y uncertainty
    if(P_k_bar(1,1) > Y_UNCERT_MAX) {
        P_k_bar(1,1) = Y_UNCERT_MAX;
    }
    // Check h uncertainty
    if(P_k_bar(2,2) > H_UNCERT_MAX) {
        P_k_bar(2,2) = H_UNCERT_MAX;
    }

    // We don't want any covariance values getting too large
    for (unsigned int i = 0; i < numStates; ++i) {
        for (unsigned int j = 0; j < numStates; ++j) {
            if(P_k(i,j) > X_UNCERT_MAX) {
                P_k(i,j) = X_UNCERT_MAX;
            }
        }
    }

}

/**
 * Method to ensure that uncertainty does not grow or shrink without bound
 */
void LocEKF::limitPosteriorUncert()
{
    // Check x uncertainty
    if(P_k(0,0) < X_UNCERT_MIN) {
        P_k(0,0) = X_UNCERT_MIN;
        P_k_bar(0,0) = X_UNCERT_MIN;
    }
    // Check y uncertainty
    if(P_k(1,1) < Y_UNCERT_MIN) {
        P_k(1,1) = Y_UNCERT_MIN;
        P_k_bar(1,1) = Y_UNCERT_MIN;

    }
    // Check h uncertainty
    if(P_k(2,2) < H_UNCERT_MIN) {
        P_k(2,2) = H_UNCERT_MIN;
        P_k_bar(2,2) = H_UNCERT_MIN;

    }
    // Check x uncertainty
    if(P_k(0,0) > X_UNCERT_MAX) {
        P_k(0,0) = X_UNCERT_MAX;
        P_k_bar(0,0) = X_UNCERT_MAX;
    }
    // Check y uncertainty
    if(P_k(1,1) > Y_UNCERT_MAX) {
        P_k(1,1) = Y_UNCERT_MAX;
        P_k_bar(1,1) = Y_UNCERT_MAX;
    }
    // Check h uncertainty
    if(P_k(2,2) > H_UNCERT_MAX) {
        P_k(2,2) = H_UNCERT_MAX;
        P_k_bar(2,2) = H_UNCERT_MAX;
    }
}

/**
 * Method to use the estimate ellipse to intelligently clip the pose estimate
 * of the robot using the information of the uncertainty ellipse.
 */
void LocEKF::clipRobotPose()
{
    // Limit our X estimate
    if (xhat_k(0) > X_EST_MAX) {
        StateVector v(numStates);
        v(0) = 1.0f;
        xhat_k = xhat_k - prod(P_k,v)* (inner_prod(v,xhat_k) - X_EST_MAX) /
            inner_prod(v, prod(P_k,v));
    }
    else if (xhat_k(0) < X_EST_MIN) {
        StateVector v(numStates);
        v(0) = 1.0f;
        xhat_k = xhat_k - prod(P_k,v)* (inner_prod(v,xhat_k)) /
            inner_prod(v, prod(P_k,v));
    }

    // Limit our Y estimate
    if (xhat_k(1) < Y_EST_MIN) {
        StateVector v(numStates);
        v(1) = 1.0f;
        xhat_k = xhat_k - prod(P_k,v)* (inner_prod(v,xhat_k)) /
            inner_prod(v, prod(P_k,v));
    }
    else if (xhat_k(1) > Y_EST_MAX) {
        StateVector v(numStates);
        v(1) = 1.0f;
        xhat_k = xhat_k - prod(P_k,v)* (inner_prod(v,xhat_k) - Y_EST_MAX) /
            inner_prod(v, prod(P_k,v));

    }
}



// THIS METHOD IS NOT CURRENTLY BEING USED NOR HAS IT BEEN ADEQUATELY TESTED
// CONSULT NUbots Team Report 2006 FOR MORE INFORMATION
/**
 * Detect if we are in a deadzone and apply the correct changes to keep the pose
 * estimate from drifting
 *
 * @param R The measurement noise covariance matrix
 * @param innovation The measurement divergence
 * @param CPC Predicted measurement variance
 * @param EPS Size of the deadzone
 */
void LocEKF::deadzone(float &R, float &innovation,
                      float CPC, float eps)
{
    float invR = 0.0;
    // Not in a deadzone
    if ((eps < 1.0e-08) || (CPC < 1.0e-08) || (R < 1e-08)) {
        return;
    }

    if ( abs(innovation) > eps) {
        // Decrease the covariance, so that it effects the estimate more
        invR=( abs(innovation) / eps-1) / CPC;
    } else {
        // Decrease the innovations, so that it doesn't effect the estimate
        innovation=0.0;
        invR = 0.25f / (eps*eps) - 1.0f/ CPC;
    }

    // Limit the min Covariance value
    if (invR<1.0e-08) {
        invR=1e-08f;
    }
    // Set the covariance to be the adjusted value
    if ( R < 1.0/invR ) {
        R=1.0f/invR;
    }
}

// Finds the closest point on a line to the robot's position.
// Returns relative coordinates of the point, in the frame of
// reference of the field's coordinate system.
std::pair<float,float>
LocEKF::findClosestLinePointCartesian(LineLandmark l, float x_r,
                                      float y_r, float h_r)
{
    const float x_l = l.dx;
    const float y_l = l.dy;

    const float x_b = l.x1;
    const float y_b = l.y1;

    // Find closest point on the line to the robot (global frame)
    const float x_p = ((x_r - x_b)*x_l + (y_r - y_b)*y_l)*x_l + x_b;
    const float y_p = ((x_r - x_b)*x_l + (y_r - y_b)*y_l)*y_l + y_b;

    // Relativize the closest point
    const float relX_p = x_p - x_r;
    const float relY_p = y_p - y_r;
    return std::pair<float,float>(relX_p, relY_p);
}

void LocEKF::printBeforeUpdateInfo()
{
    cout << "Loc update: " << endl;
    cout << "Before updates: " << *this << endl;
    cout << "\tOdometery is " << lastOdo <<endl;
    cout << "\tObservations are: " << endl;
    for(unsigned int i = 0; i < lastObservations.size(); ++i) {
        cout << "\t\t" << lastObservations[i] <<endl;
    }
}

void LocEKF::printAfterUpdateInfo()
{
    cout << "After updates: " << *this << endl;
    cout << endl;
    cout << endl;
    cout << endl;
}
