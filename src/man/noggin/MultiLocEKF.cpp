#include "MultiLocEKF.h"
#include <boost/numeric/ublas/io.hpp> // for cout
#include <iostream>

#include "FieldConstants.h"
// #define DEBUG_LOC_EKF_INPUTS
// #define DEBUG_STANDARD_ERROR
using namespace boost::numeric;
using namespace boost;
using namespace std;
using namespace NBMath;
using namespace ekf;

// Parameters
// Measurement conversion form
const float MultiLocEKF::USE_CARTESIAN_DIST = 50.0f;
// Uncertainty
const float MultiLocEKF::BETA_LOC = 1.0f;
const float MultiLocEKF::GAMMA_LOC = 0.1f;
const float MultiLocEKF::BETA_ROT = M_PI_FLOAT/64.0f;
const float MultiLocEKF::GAMMA_ROT = 0.1f;

// Default initialization values
const float MultiLocEKF::INIT_LOC_X = CENTER_FIELD_X;
const float MultiLocEKF::INIT_LOC_Y = CENTER_FIELD_Y;
const float MultiLocEKF::INIT_LOC_H = 0.0f;
const float MultiLocEKF::INIT_BLUE_GOALIE_LOC_X = (FIELD_WHITE_LEFT_SIDELINE_X +
                                              GOALBOX_DEPTH / 2.0f);
const float MultiLocEKF::INIT_BLUE_GOALIE_LOC_Y = CENTER_FIELD_Y;
const float MultiLocEKF::INIT_BLUE_GOALIE_LOC_H = 0.0f;
const float MultiLocEKF::INIT_RED_GOALIE_LOC_X = (FIELD_WHITE_RIGHT_SIDELINE_X -
                                             GOALBOX_DEPTH / 2.0f);
const float MultiLocEKF::INIT_RED_GOALIE_LOC_Y = CENTER_FIELD_Y;
const float MultiLocEKF::INIT_RED_GOALIE_LOC_H = M_PI_FLOAT;
// Uncertainty limits
const float MultiLocEKF::X_UNCERT_MAX = FIELD_WIDTH / 2.0f;
const float MultiLocEKF::Y_UNCERT_MAX = FIELD_HEIGHT / 2.0f;
const float MultiLocEKF::H_UNCERT_MAX = 4*M_PI_FLOAT;
const float MultiLocEKF::X_UNCERT_MIN = 1.0e-6f;
const float MultiLocEKF::Y_UNCERT_MIN = 1.0e-6f;
const float MultiLocEKF::H_UNCERT_MIN = 1.0e-6f;
// Initial estimates
const float MultiLocEKF::INIT_X_UNCERT = X_UNCERT_MAX / 2.0f;
const float MultiLocEKF::INIT_Y_UNCERT = Y_UNCERT_MAX / 2.0f;
const float MultiLocEKF::INIT_H_UNCERT = M_PI_FLOAT * 2.0f;
// Estimate limits
const float MultiLocEKF::X_EST_MIN = 0.0f;
const float MultiLocEKF::Y_EST_MIN = 0.0f;
const float MultiLocEKF::X_EST_MAX = FIELD_GREEN_WIDTH;
const float MultiLocEKF::Y_EST_MAX = FIELD_GREEN_HEIGHT;

const float MultiLocEKF::ERROR_RESET_THRESH = 0.2f;
const float MultiLocEKF::STANDARD_ERROR_THRESH = 6.0f;


/**
 * Calculate the distance and bearing error between an observation
 * (obs) and a landmark (pt).
 *
 * Stores them in pt_dist and pt_bearing
 */
#define CALCULATE_PT_OBS_ERRORS(obs, pt)                                \
    const float x = xhat_k_bar(0);                                      \
    const float y = xhat_k_bar(1);                                      \
    const float h = xhat_k_bar(2);                                      \
                                                                        \
    /* Relative (x,y) from landmark to our position */                  \
    float pt_rel_x = (x - pt.x);                                        \
    float pt_rel_y = (y - pt.y);                                        \
                                                                        \
    float pt_dist = hypotf(pt_rel_x, pt_rel_y);                         \
                                                                        \
    /* Heading from me to landmark */                                   \
    /* Opposite of pt_rel_x needed to get vector direction right  */    \
    float heading_mag = acosf(-pt_rel_x / pt_dist);                     \
                                                                        \
    /* Sign the heading and subtract our heading to get bearing */      \
    float pt_bearing = subPIAngle(copysignf(1.0f, -pt_rel_y) *          \
                                  heading_mag - h);                     \
                                                                        \
    float dist_error = z.getVisDistance() - pt_dist;                    \
    float bearing_error = (subPIAngle(z.getVisBearing() - pt_bearing) / \
                           z.getBearingSD());


/**
 * Calculate the expected distance, bearing, and orientation to a
 * landmark and its errors with an observation.
 *
 * Stores orientation in pt_orientation
 */
#define CALCULATE_CORNER_OBS_ERRORS(obs, pt)                            \
    CALCULATE_PT_OBS_ERRORS(obs, pt);                                   \
                                                                        \
    /* Calculate the heading from the corner to us */                   \
    float heading_from_corner = 180 - heading_mag;                      \
                                                                        \
    float pt_orientation = (heading_from_corner *                       \
                            copysignf(1.0f, pt_rel_y) - pt.angle);      \
                                                                        \
    float orientation_error = subPIAngle(z.getVisOrientation() -        \
                                         pt_orientation);


/**
 * Fills the invariance, Jacobian, and covariance matrix for a point.
 *
 * Must be called after CALCULATE_PT_OBS_ERRORS
 */
#define INCORPORATE_POINT_POLAR(H_k, R_k, V_k){                         \
        V_k(0) = dist_error;                                            \
        V_k(1) = bearing_error;                                         \
                                                                        \
        /* Calculate jacobians. */                                      \
        /* See CALCULATE_PT_OBS_ERRORS macro for variable */            \
        /* derivations.*/                                               \
                                                                        \
        /* Derivatives of distance with respect to x,y,h */             \
        H_k(0,0) = pt_rel_x / pt_dist;                                  \
        H_k(0,1) = pt_rel_y / pt_dist;                                  \
        H_k(0,2) = 0;                                                   \
                                                                        \
        /* Derivatives of bearing with respect to x,y,h */              \
        H_k(1,0) = -pt_rel_y / (pt_dist * pt_dist);                     \
        H_k(1,1) =  pt_rel_x / (pt_dist * pt_dist);                     \
        H_k(1,2) = -1;                                                  \
                                                                        \
        /* Update the measurement covariance matrix */                  \
        /* Indices: (Dist, bearing, orientation) */                     \
        R_k(0,0) = z.getDistanceSD() * z.getDistanceSD();               \
        R_k(0,1) = 0.0f;                                                \
        R_k(0,2) = 0.0f;                                                \
                                                                        \
        R_k(1,0) = 0.0f;                                                \
        R_k(1,1) = z.getBearingSD() * z.getBearingSD();                 \
        R_k(1,2) = 0.0f;                                                \
    }

#ifdef DEBUG_LOC_EKF_INPUTS
#define PRINT_LOC_EKF_INPUTS()                          \
    cout << "\t\t\tR vector is" << R_k << endl;         \
    cout << "\t\t\tH vector is" << H_k << endl;         \
    cout << "\t\t\tV vector is" << V_k << endl;         \
    cout << "\t\t\t\t\t(x,y,h) est is (" << x << ", "   \
         << y << ", " << h << ")" << endl;
#else
#define PRINT_LOC_EKF_INPUTS()
#endif

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
MultiLocEKF::MultiLocEKF(float initX, float initY, float initH,
               float initXUncert,float initYUncert, float initHUncert)
    : TwoMeasurementEKF<PointObservation, dist_bearing_meas_dim,
                        CornerObservation, corner_measurement_dim,
                        MotionModel,
                        loc_ekf_dimension>(BETA_LOC,GAMMA_LOC), LocSystem(),
      lastOdo(0,0,0), lastPointObservations(), lastCornerObservations(),
      useAmbiguous(true), errorLog(error_log_width),
      observationError(false), resetFlag(false)
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
    cout << "Initializing MultiLocEKF with: " << *this << endl;
#endif
}

/**
 * Reset the EKF to a starting configuration
 */
void MultiLocEKF::reset()
{
    resetLocTo(INIT_LOC_X,
               INIT_LOC_Y,
               INIT_LOC_H);
}

/**
 * Reset the EKF to a blue goalie starting configuration
 */
void MultiLocEKF::blueGoalieReset()
{
    resetLocTo(INIT_BLUE_GOALIE_LOC_X,
               INIT_BLUE_GOALIE_LOC_Y,
               INIT_BLUE_GOALIE_LOC_H);

}

/**
 * Reset the EKF to a red goalie starting configuration
 */
void MultiLocEKF::redGoalieReset()
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
void MultiLocEKF::resetLocTo(float x, float y, float h)
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
void MultiLocEKF::updateLocalization(const MotionModel& u,
                                     const std::vector<PointObservation>& pt_z,
                                     const std::vector<CornerObservation>& c_z)
{
#ifdef DEBUG_LOC_EKF_INPUTS
    printBeforeUpdateInfo();
#endif

    // Reset error check
    observationError = false;

    odometryUpdate(u);
    applyObservations(pt_z, c_z);
    endFrame();

#ifdef DEBUG_LOC_EKF_INPUTS
    printAfterUpdateInfo();
#endif
}

// Update expected position based on odometry
void MultiLocEKF::odometryUpdate(const MotionModel& u)
{
    timeUpdate(u);
    limitAPrioriUncert();
    lastOdo = u;
}

/**
 * Apply a whole set of observations from one time frame.
 */
void MultiLocEKF::applyObservations(vector<PointObservation> pt_z,
                                    vector<CornerObservation> c_z)
{
    // Update recently seen observations of both types
    lastCornerObservations = c_z;
    lastPointObservations = pt_z;

    if (! useAmbiguous) {
        removeAmbiguous<PointObservation>(pt_z);
        removeAmbiguous<CornerObservation>(c_z);
    }

    bool shouldCorrect = !pt_z.empty() || !c_z.empty();

    // Our localization needs to be reset
    if (resetFlag){

        // resetFlag = false if we reset loc, still true otherwise
        resetFlag = !resetLoc(pt_z, c_z);

        // We should only run a correction step if we've seen
        // observations, and we are still waiting to correct our Loc
        shouldCorrect = shouldCorrect && resetFlag;
    }

    // Correct step based on the observed stuff
    shouldCorrect ? correctionStep(pt_z, c_z) : noCorrectionStep();

    //limitPosteriorUncert();
}


/**
 * Performs final cleanup at the end of a time step. Clips robot position
 * to be on the field and tests fof NaN values.
 */
void MultiLocEKF::endFrame()
{

    // Clip values if our estimate is off the field
    clipRobotPose();
    if (testForNaNReset()) {
        cout << "MultiLocEKF reset to: "<< *this << endl;
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
TwoMeasurementEKF<PointObservation,
                  dist_bearing_meas_dim,
                  CornerObservation,
                  corner_measurement_dim,
                  MotionModel,
                  loc_ekf_dimension>::StateVector
MultiLocEKF::associateTimeUpdate(MotionModel u)
{
#ifdef DEBUG_LOC_EKF_INPUTS
    cout << "\t\t\tUpdating Odometry of " << u << endl;
#endif

    // Calculate the assumed change in loc position
    // Assume no decrease in loc velocity
    StateVector deltaLoc(loc_ekf_dimension);
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
void MultiLocEKF::incorporateMeasurement(const PointObservation& z,
                                         StateMeasurementMatrix1 &H_k,
                                         MeasurementMatrix1 &R_k,
                                         MeasurementVector1 &V_k)
{
#ifdef DEBUG_LOC_EKF_INPUTS
    cout << "\t\t\tIncorporating measurement " << z << endl;
#endif

    const int obsIndex = findBestLandmark<PointObservation, PointLandmark>(z);

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

    checkStandardError<StateMeasurementMatrix1, MeasurementVector1,
                       MeasurementMatrix1>(P_k, H_k, V_k, &R_k);
}

void MultiLocEKF::incorporateMeasurement(const CornerObservation& z,
                                         StateMeasurementMatrix2 &H_k,
                                         MeasurementMatrix2 &R_k,
                                         MeasurementVector2 &V_k)
{
#ifdef DEBUG_LOC_EKF_INPUTS
    cout << "\t\t\tIncorporating measurement " << z << endl;
#endif
    const int obsIndex = findBestLandmark<CornerObservation, CornerLandmark>(z);

    // No landmark is close enough, don't attempt to use one
    if (obsIndex < 0) {
        R_k(0,0) = DONT_PROCESS_KEY;
        return;
    }

    incorporateCorner(obsIndex, z, H_k, R_k, V_k);

    checkStandardError<StateMeasurementMatrix2,
                       MeasurementVector2,
                       MeasurementMatrix2>(P_k, H_k, V_k, &R_k);
}


void MultiLocEKF::incorporateCartesianMeasurement(int obsIndex,
                                                  const PointObservation& z,
                                                  StateMeasurementMatrix1 &H_k,
                                                  MeasurementMatrix1 &R_k,
                                                  MeasurementVector1 &V_k)
{

#ifdef DEBUG_LOC_EKF_INPUTS
    cout << "\t\t\tUsing cartesian " << endl;
#endif
    // Convert our sighting to cartesian coordinates
    MeasurementVector1 z_x(dist_bearing_meas_dim);
    z_x(0) = z.getVisDistance() * cos(z.getVisBearing());
    z_x(1) = z.getVisDistance() * sin(z.getVisBearing());

    // Get expected values of the post
    const float x_b = z.getPossibilities()[obsIndex].x;
    const float y_b = z.getPossibilities()[obsIndex].y;

    const float x = xhat_k_bar(0);
    const float y = xhat_k_bar(1);
    const float h = xhat_k_bar(2);

    float sinh, cosh;
    sincosf(h, &sinh, &cosh);

    MeasurementVector1 d_x(dist_bearing_meas_dim);
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

    PRINT_LOC_EKF_INPUTS();

}

void MultiLocEKF::incorporatePolarMeasurement(int obsIndex,
                                              const PointObservation& z,
                                              StateMeasurementMatrix1 &H_k,
                                              MeasurementMatrix1 &R_k,
                                              MeasurementVector1 &V_k)
{
#ifdef DEBUG_LOC_EKF_INPUTS
    cout << "\t\t\tUsing polar " << endl;
#endif

    // Get expected values of the post
    PointLandmark bestPossibility = z.getPossibilities()[obsIndex];

    // Calculate the errors
    CALCULATE_PT_OBS_ERRORS(z, bestPossibility);

    INCORPORATE_POINT_POLAR(H_k, R_k, V_k);

    PRINT_LOC_EKF_INPUTS();
}


void MultiLocEKF::incorporateCorner(int index,
                                    const CornerObservation& z,
                                    StateMeasurementMatrix2 &H_k,
                                    MeasurementMatrix2 &R_k,
                                    MeasurementVector2 &V_k)
{
    // Get the observed range, bearing, and orientation
    // Pack into measurement vector

    // Get expected values of the corner
    CornerLandmark bestPossibility = z.getPossibilities()[index];

    // Calculate the errors
    CALCULATE_CORNER_OBS_ERRORS(z, bestPossibility);

    // Uses the same values as a point landmark (dist, bearing)
    INCORPORATE_POINT_POLAR(H_k, R_k, V_k);

    // Has additional dimensions for orientation
    V_k(2) = orientation_error;

    // Derivatives of orientation with respect to x,y,h
    H_k(2,0) = pt_rel_y / (pt_dist * pt_dist);
    H_k(2,1) = -pt_rel_x / (pt_dist * pt_dist);
    H_k(2,2) = 0;

    // Orientation covariences
    R_k(2,0) = 0.0f;
    R_k(2,1) = 0.0f;
    R_k(2,2) = z.getOrientationSD() * z.getOrientationSD();

    PRINT_LOC_EKF_INPUTS();
}

/**
 * Get the divergence between an observation and a possible point landmark
 *
 * @param z The observation measurement to be examined
 * @param pt The possible point to check against
 *
 * @return The divergence value
 */
float MultiLocEKF::getDivergence(const PointObservation& z,
                                 const PointLandmark& pt)
{
    CALCULATE_PT_OBS_ERRORS(z,pt);

    // Normalized errors
    float dist_error_norm    = dist_error    / z.getDistanceSD();
    float bearing_error_norm = bearing_error / z.getBearingSD();

#ifdef DEBUG_DIVERGENCE_CALCULATIONS
    cout << "Normalized distance error: " << dist_error_norm << endl;
    cout << "Normalized bearing error: " << bearing_error_norm << endl;
#endif //DEBUG_DIVERGENCE_CALCULATIONS

    // Euclidean distance
    return (dist_error_norm * dist_error_norm +
            bearing_error_norm * bearing_error_norm);
}

float MultiLocEKF::getDivergence(const CornerObservation& z,
                                 const CornerLandmark& pt)
{
    CALCULATE_CORNER_OBS_ERRORS(z, pt);

    // Normalized errors
    float dist_error_norm        = dist_error        / z.getDistanceSD();
    float bearing_error_norm     = bearing_error     / z.getBearingSD();
    float orientation_error_norm = orientation_error / z.getOrientationSD();

#ifdef DEBUG_DIVERGENCE_CALCULATIONS
    cout << "Normalized distance error: " << dist_error_norm << endl;
    cout << "Normalized bearing error: " << bearing_error_norm << endl;
    cout << "Normalized orientation error: " << orientation_error_norm << endl;
#endif //DEBUG_DIVERGENCE_CALCULATIONS


    return (dist_error_norm * dist_error_norm +
            bearing_error_norm * bearing_error_norm +
            orientation_error_norm * orientation_error_norm);
}


/*
 * Specialized acceptable divergence functions
 */
template<>
float MultiLocEKF::getAcceptableDivergence<CornerLandmark>() {
    // (3 sd's outside)^2 * 3
    return max_corner_divergence;
}

template<>
float MultiLocEKF::getAcceptableDivergence<PointLandmark>() {
    // (3 sd's outside)^2 * 2
    return max_pt_divergence;
}

/**
 * Method to ensure that uncertainty does not grow without bound
 */
void MultiLocEKF::limitAPrioriUncert()
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
    for (unsigned int i = 0; i < loc_ekf_dimension; ++i) {
        for (unsigned int j = 0; j < loc_ekf_dimension; ++j) {
            if(P_k(i,j) > X_UNCERT_MAX) {
                P_k(i,j) = X_UNCERT_MAX;
            }
        }
    }

}

/**
 * Method to ensure that uncertainty does not grow or shrink without bound
 */
void MultiLocEKF::limitPosteriorUncert()
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
void MultiLocEKF::clipRobotPose()
{
    // Limit our X estimate
    if (xhat_k(0) > X_EST_MAX) {
        StateVector v(loc_ekf_dimension);
        v(0) = 1.0f;
        xhat_k = xhat_k - prod(P_k,v)* (inner_prod(v,xhat_k) - X_EST_MAX) /
            inner_prod(v, prod(P_k,v));
    }
    else if (xhat_k(0) < X_EST_MIN) {
        StateVector v(loc_ekf_dimension);
        v(0) = 1.0f;
        xhat_k = xhat_k - prod(P_k,v)* (inner_prod(v,xhat_k)) /
            inner_prod(v, prod(P_k,v));
    }

    // Limit our Y estimate
    if (xhat_k(1) < Y_EST_MIN) {
        StateVector v(loc_ekf_dimension);
        v(1) = 1.0f;
        xhat_k = xhat_k - prod(P_k,v)* (inner_prod(v,xhat_k)) /
            inner_prod(v, prod(P_k,v));
    }
    else if (xhat_k(1) > Y_EST_MAX) {
        StateVector v(loc_ekf_dimension);
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
void MultiLocEKF::deadzone(float &R, float &innovation,
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
MultiLocEKF::findClosestLinePointCartesian(LineLandmark l, float x_r,
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

void MultiLocEKF::printBeforeUpdateInfo()
{
    cout << "Loc update: " << endl;
    cout << "Before updates: " << *this << endl;
    cout << "\tOdometery is " << lastOdo <<endl;
    cout << "\tObservations are: " << endl;
    // for(unsigned int i = 0; i < lastObservations.size(); ++i) {
    //     cout << "\t\t" << lastObservations[i] <<endl;
    // }
}

void MultiLocEKF::printAfterUpdateInfo()
{
    cout << "After updates: " << *this << endl;
    cout << endl;
    cout << endl;
    cout << endl;
}

/**
 * Checks after correction steps and before the state is updated.
 */
void MultiLocEKF::beforeCorrectionFinish()
{
    double pushValue(0.0f);
    if (observationError){
        pushValue = 1.0f;
    }

    // If we've seen to many erroneous frames, let's reset to be safe
    if (errorLog.X(pushValue) > ERROR_RESET_THRESH){
        resetFlag = true;
    }
}


/**
 * Uses unambiguous observations to reset localization to a new
 * position. Used when our localization begins to diverge
 * significantly.
 *
 * @return true if the system was reset from definite observations,
 *         false otherwise
 */
bool MultiLocEKF::resetLoc(const vector<PointObservation> pt_z,
                           const vector<CornerObservation>& c_z)
{
    return (resetLoc(pt_z) || resetLoc(c_z));
}

/**
 * Look for two unambiguous point observations (whatever they are) in
 * order to triangulate our new position
 *
 * @return true if we reset our loc
 */
bool MultiLocEKF::resetLoc(const vector<PointObservation>& z)
{
    vector<PointObservation>::const_iterator i;
    const PointObservation * def_1 = NULL, * def_2 = NULL;
    for (i = z.begin() ; i != z.end(); ++i){

        if (!i->isAmbiguous()){

            // Set the two pointers to definite pt observations
            if (def_1 == NULL){
                def_1 = &(*i);
            } else if (def_2 == NULL){
                def_2 = &(*i);
                break;
            }
        }
    }

    if (def_2 != NULL){
        resetLoc(def_1, def_2);
        return true;
    }
    return false;
}

/**
 * Look for an unambiguous corner observation to use to reset our
 * localization.
 *
 * @return true if we reset our loc
 */
bool MultiLocEKF::resetLoc(const vector<CornerObservation>& z)
{
    // Search corners for one definite observation
    vector<CornerObservation>::const_iterator i;

    for(i = z.begin(); i != z.end(); ++i){

        // Once we find a definite corner, reset to it
        if (!i->isAmbiguous()){
            resetLoc(&(*i));
            return true;
        }
    }
    return false;
}

/**
 * Triangulate and reset our position with two definite
 * PointObservations.
 */
void MultiLocEKF::resetLoc(const PointObservation* pt1,
                           const PointObservation* pt2)
{
    assert(!pt1->isAmbiguous() && !pt2->isAmbiguous());

}

void MultiLocEKF::resetLoc(const CornerObservation* c)
{
    assert(!c->isAmbiguous());

}
