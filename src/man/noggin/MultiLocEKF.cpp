#include "MultiLocEKF.h"
#include <boost/numeric/ublas/io.hpp> // for cout
#include <iostream>

#include "FieldConstants.h"

using namespace boost::numeric;
using namespace boost;
using namespace std;
using namespace NBMath;
using namespace ekf;

// Parameters
// Measurement conversion form
const float MultiLocEKF::USE_CARTESIAN_DIST = 50.0f;
// Uncertainty
const float MultiLocEKF::BETA_LOC = 0.0f;
const float MultiLocEKF::GAMMA_LOC = 0.3f;
const float MultiLocEKF::BETA_ROT = 0.0f; //M_PI_FLOAT/256.0f;
const float MultiLocEKF::GAMMA_ROT = 0.3f;

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
const float MultiLocEKF::H_EST_MIN = -M_PI_FLOAT;
const float MultiLocEKF::X_EST_MAX = FIELD_GREEN_WIDTH;
const float MultiLocEKF::Y_EST_MAX = FIELD_GREEN_HEIGHT;
const float MultiLocEKF::H_EST_MAX = M_PI_FLOAT;

const float MultiLocEKF::ERROR_RESET_THRESH = 0.6f;
const float MultiLocEKF::STANDARD_ERROR_THRESH = 6.0f;


/**
 * Calculate the distance and bearing error between an observation
 * (obs) and a landmark (pt).
 *
 * Stores them in pt_dist and pt_bearing
 */
#define CALCULATE_PT_OBS_ERRORS(obs, pt)                                \
    const float x = xhat_k_bar(x_index);                                \
    const float y = xhat_k_bar(y_index);                                \
    const float h = xhat_k_bar(h_index);                                \
                                                                        \
    /* Relative (x,y) from landmark to our position */                  \
    float pt_rel_x = (x - pt.x);                                        \
    float pt_rel_y = (y - pt.y);                                        \
                                                                        \
    float pt_dist = hypotf(pt_rel_x, pt_rel_y);                         \
                                                                        \
    /* Heading from me to landmark */                                   \
    /* Opposite of pt_rel_x needed to get vector direction right  */    \
    float heading_mag = acosf(clip(-pt_rel_x / pt_dist, -1.0f, 1.0f));  \
                                                                        \
    /* Sign the heading and subtract our heading to get bearing */      \
    float pt_bearing = subPIAngle(copysignf(1.0f, -pt_rel_y) *          \
                                  heading_mag - h);                     \
                                                                        \
    float dist_error = z.getVisDistance() - pt_dist;                    \
    float bearing_error = subPIAngle(z.getVisBearing() - pt_bearing);


/**
 * Calculate the expected distance, bearing, and orientation to a
 * landmark and its errors with an observation.
 *
 * Stores orientation in pt_orientation
 */
#define CALCULATE_CORNER_OBS_ERRORS(obs, pt)                            \
    CALCULATE_PT_OBS_ERRORS(obs, pt);                                   \
                                                                        \
    /* Calculate dot product of distance vector to corner's zero  */    \
    float dot_prod = (pt_rel_x * cos(pt.angle) +                        \
                      pt_rel_y * sin(pt.angle) );                       \
                                                                        \
    /* Calculate sign based on corner's listed angle */                 \
    float dot_sign = (-pt_rel_x * sin(pt.angle) +                       \
                      pt_rel_y * cos(pt.angle) );                       \
                                                                        \
    /* Acos always returns positive, so we have the magnitude of our */ \
    /* orientation to the corner */                                     \
    float mag_orientation = acosf(clip(dot_prod/pt_dist, -1.0f, 1.0f)); \
    float pt_orientation = copysignf(1.0f,dot_sign) * mag_orientation;  \
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
        /* Indices: (Dist, bearing) */                                  \
        R_k(0,0) = z.getDistanceSD() * z.getDistanceSD();               \
        R_k(0,1) = 0.0f;                                                \
                                                                        \
        R_k(1,0) = 0.0f;                                                \
        R_k(1,1) = z.getBearingSD() * z.getBearingSD();                 \
    }

// Print the vectors and matrices used in the correction steps
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
      lastOdometry(0,0,0), lastPointObservations(), lastCornerObservations(),
      useAmbiguous(true), errorLog(error_log_width),
      resetFlag(false)
{
    // These jacobian values are unchanging and independent of the
    // motion updates, so we initialize them here.
    A_k(0,0) = 1.0;
    A_k(0,1) = 0.0;

    A_k(1,0) = 0.0;
    A_k(1,1) = 1.0;

    A_k(2,0) = 0.0;
    A_k(2,1) = 0.0;
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
    for (int i=0; i < loc_ekf_dimension; ++i){
        xhat_k(i) = xhat_k_bar(i) = 0.0f;
        for (int j=0; j < loc_ekf_dimension; ++j){
            P_k(i,j) = P_k_bar(i,j) = 0.0f;
        }
    }
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
    DeltaMotionModel deltaOdometry;
    if (u.isValid()) {
        deltaOdometry = u - lastOdometry;
    } // if odometry is invalid leave deltaOdometry be (0, 0, 0)
    timeUpdate(deltaOdometry);
    limitAPrioriUncert();
    lastOdometry = u;
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

    if (resetFlag){

        // resetFlag = false if we reset loc, still true otherwise
        resetFlag = !resetLoc(pt_z, c_z);

        if (!resetFlag){
            errorLog.Reset();
        }

        // We should only run a correction step if we've seen
        // observations, and we are still waiting to correct our Loc
        shouldCorrect = shouldCorrect && resetFlag;
    }

    // Correct step based on the observations
    shouldCorrect ? correctionStep(pt_z, c_z) : noCorrectionStep();

    limitPosteriorUncert();
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
        cout << "MultiLocEKF reset to: " << endl << *this << endl;
        cout << "\tLast odo is: " << lastOdometry << endl;
        cout << endl;
    }
}


/**
 * Method incorporate the expected change in loc position from the last
 * frame.  Updates the values of the covariance matrix Q_k and the jacobian
 * A_k.
 *
 * @param deltaOdometry The motion model of the last frame.  Ignored for the loc.
 * @return The expected change in loc position (x,y, xVelocity, yVelocity)
 */
TwoMeasurementEKF<PointObservation,
                  dist_bearing_meas_dim,
                  CornerObservation,
                  corner_measurement_dim,
                  MotionModel,
                  loc_ekf_dimension>::StateVector
MultiLocEKF::associateTimeUpdate(const DeltaMotionModel& u)
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

    deltaLoc(0) = u.x * cosh - u.y * sinh;
    deltaLoc(1) = u.x * sinh + u.y * cosh;
    deltaLoc(2) = u.theta;

    // Derivatives of motion updates re:x,y,h
    // Other values are set in the constructor and are unchanging
    A_k(0,2) = -u.x * sinh - u.y * cosh;
    A_k(1,2) = u.x * cosh - u.y * sinh;

    return deltaLoc;
}

/**
 * Method to deal with incorporating a PointObservation into the EKF
 *
 * @param z the measurement to be incorporated
 * @param H_k the jacobian associated with the measurement, to be filled
 * @param R_k the covariance matrix of the measurement, to be filled
 * @param V_k the measurement invariance, to be filled
 *
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

    // If the observation is close, we use a more stable technique for
    // incorporating it
    if (z.getVisDistance() < USE_CARTESIAN_DIST) {
        incorporateCartesianMeasurement( obsIndex, z, H_k, R_k, V_k);
    } else {
        incorporatePolarMeasurement( obsIndex, z, H_k, R_k, V_k);
    }

    checkStandardError<StateMeasurementMatrix1, MeasurementVector1,
                       MeasurementMatrix1>(P_k, H_k, V_k, &R_k);
}


/**
 * Method to deal with incorporating a Corner Observation into the EKF
 *
 * @param z the measurement to be incorporated
 * @param H_k the jacobian associated with the measurement, to be filled
 * @param R_k the covariance matrix of the measurement, to be filled
 * @param V_k the measurement invariance, to be filled
 *
 */
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


/**
 * Convert the (normall polar) measurement to cartesian for a more
 * stable jacobian and small distances
 */
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

/**
 * Incorporate the Point Observation in its native polar form
 */
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

/**
 * Incorporate a corner into the EKF.
 *
 *     Corners have distance, bearing *and* orientation!
 */
void MultiLocEKF::incorporateCorner(int index,
                                    const CornerObservation& z,
                                    StateMeasurementMatrix2 &H_k,
                                    MeasurementMatrix2 &R_k,
                                    MeasurementVector2 &V_k)
{
    // The corner which fits the data best
    CornerLandmark c = z.getPossibilities()[index];

    // Calculate the errors
    CALCULATE_CORNER_OBS_ERRORS(z, c);

    // Uses the same values as a point landmark (dist, bearing)
    INCORPORATE_POINT_POLAR(H_k, R_k, V_k);

    // Has additional dimensions for orientation
    V_k(2) = orientation_error;

    // The jacobian's denomitator
    // Clipped to prevent sqrt of negative
    float denom = 1 - ((dot_prod * dot_prod) / (pt_dist*pt_dist));

    if (denom < 0){
        denom = 0.001f;
    } else {
        denom = sqrtf(denom);
    }

    float sign = copysignf(1.0f, dot_sign);

    // Derivatives of orientation with respect to x,y,h
    //
    // Derived using SAGE math software (Linux), from
    // the equations used to find the orientation
    H_k(2,0) = sign * (dot_prod * pt_rel_x/
                       pow(pt_dist,3) -
                       cos(c.angle)/pt_dist) / denom;

    H_k(2,1) = sign * (dot_prod*pt_rel_y/
                       pow(pt_dist,3) -
                       sin(c.angle)/pt_dist) / denom;
    H_k(2,2) = 0;

    // Orientation covariences
    // last column and last row indices
    R_k(0,2) = 0.0f;
    R_k(1,2) = 0.0f;

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

/**
 * Returns the normalized Euclidean error between the given
 * observation and landmark
 */
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
 * Specialized acceptable divergence functions. Templated to allow for
 * a different "acceptable divergence" for different landmark types,
 * since they have different dimensions.
 */
template<>
inline float MultiLocEKF::getAcceptableDivergence<CornerLandmark>() {
    // (3 sd's outside)^2 * 3
    return max_corner_divergence;
}

template<>
inline float MultiLocEKF::getAcceptableDivergence<PointLandmark>() {
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
    P_k(0,0) = P_k_bar(0,0) = clip(P_k(0,0), X_UNCERT_MAX);

    // Check y uncertainty
    P_k(1,1) = P_k_bar(1,1) = clip(P_k(1,1), Y_UNCERT_MAX);

    // Check h uncertainty
    P_k(2,2) = P_k_bar(2,2) = clip(P_k(2,2), H_UNCERT_MAX);
}

/**
 * Very simple clipping to the edges of the field.
 */
void MultiLocEKF::clipRobotPose()
{
    // Check x uncertainty
    xhat_k(0) =
        xhat_k_bar(0) =
        clip(xhat_k(0), X_EST_MIN, X_EST_MAX);

    // Check y uncertainty
    xhat_k(1) =
        xhat_k_bar(1) =
        clip(xhat_k(1), Y_EST_MIN, Y_EST_MAX);

    // Check h uncertainty
    xhat_k(2) =
        xhat_k_bar(2) =
        		NBMath::subPIAngle(xhat_k(2));
    //    clip(xhat_k(2), H_EST_MIN, H_EST_MAX);
}

void MultiLocEKF::printBeforeUpdateInfo()
{
    cout << "Loc update: " << endl;
    cout << "Before updates: " << *this << endl;
    cout << "\tOdometery is " << lastOdometry <<endl;
    cout << "\tPoint Observations are: " << endl;
    for(unsigned int i = 0; i < lastPointObservations.size(); ++i) {
        cout << "\t\t" << lastPointObservations[i] <<endl;
    }
    cout << "\tCorner Observations are: " << endl;
    for(unsigned int i = 0; i < lastCornerObservations.size(); ++i) {
        cout << "\t\t" << lastCornerObservations[i] <<endl;
    }
}

void MultiLocEKF::printAfterUpdateInfo()
{
    cout << "After updates: " << *this << endl;
    cout << endl;
    cout << endl;
    cout << endl;
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

#ifdef DEBUG_ERROR_LOG
    cout << "Resetting localization with points: "
         << *def_1 << " and " << *def_2 << endl;
#endif

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

#ifdef DEBUG_ERROR_LOG
        cout << "Resetting localization with corner: "
             << *i << endl;
#endif
        return true;
        }
    }
    return false;
}

/**
 * Triangulate and reset our position with two definite
 * PointObservations.
 */
void MultiLocEKF::resetLoc(const PointObservation* obs1,
                           const PointObservation* obs2)
{
    assert(!obs1->isAmbiguous() && !obs2->isAmbiguous());

    PointLandmark pt1 = obs1->getPossibilities().front();
    PointLandmark pt2 = obs2->getPossibilities().front();

    // Components of vector from (pt1 -> pt2)
    float i = pt2.x - pt1.x;
    float j = pt2.y - pt1.y;

    /**
     * Using Law of Sines
     *      c         b         a
     *   ------- = ------- = -------
     *    sin(C)    sin(B)    sin(A)
     *
     *     self   b
     *        +---------+ pt2  ^
     *        |C      A/       |
     *        |       /        |
     *        |      /         |
     *      a |     / c        |
     *        |    /        i  |   (x goes up, like on the field.
     *        |   /            |            Heading of zero is up)
     *        |B /             |
     *        | /              |
     *        +/               _
     *      pt1
     *            j
     *       |----------->
     */

    float sideA = obs1->getVisDistance();
    float sideB = obs2->getVisDistance();
    float sideC = hypotf(i,j);

    // Calculate angle between vector (pt1,pt2) and north (zero heading)
    float ptVecHeading = copysignf(1.0f, j) * acosf(clip(i/sideC, -1.0f, 1.0f));

    float angleC = abs(subPIAngle(obs1->getVisBearing() -
                                  obs2->getVisBearing()));

    // Clamp the input to asin to within (-1 , 1) due to measurement
    // inaccuracies. This prevents a nan return from asin.
    float angleB = asinf( clip( (sideB * sin(angleC)) /sideC, -1.0f, 1.0f));

     // Swap sign of angle B to place us on the correct side of the
    // line (pt1 -> pt2)
    if (NBMath::subPIAngle(obs1->getVisBearing() - obs2->getVisBearing()) > 0){
        angleB = -angleB;
    }

    // x_hat, y_hat are x and y in coord frame with x axis pointed
    // from (pt1 -> pt2) and y perpendicular
    float x_hat = sideA * cos(angleB);
    float y_hat = sideA * sin(angleB);

    // Transform to global x and y coordinates
    float newX = x_hat * cos(ptVecHeading) - y_hat * sin(ptVecHeading) + pt1.x;
    float newY = x_hat * sin(ptVecHeading) + y_hat * cos(ptVecHeading) + pt1.y;

    // Heading of line (self -> pt2)
    // Clamp the input to (-1,1) to prevent illegal trig call and a nan return
    float headingPt2 = acosf(clip( (pt2.x - newX)/sideB, -1.0f, 1.0f) );

    // Sign based on y direction of vector (self -> pt2)
    float signedHeadingPt2 = copysignf(1.0f, pt2.y - newY) * headingPt2;

    // New global heading
    float newH = NBMath::subPIAngle(signedHeadingPt2 - obs2->getVisBearing());

    xhat_k_bar(x_index) = newX;
    xhat_k_bar(y_index) = newY;
    xhat_k_bar(h_index) = newH;

    P_k_bar(0,0) = obs1->getDistanceSD() * obs1->getDistanceSD();
    P_k_bar(0,1) = 0.0f;
    P_k_bar(0,2) = 0.0f;

    P_k_bar(1,0) = 0.0f;
    P_k_bar(1,1) = obs1->getDistanceSD() * obs1->getDistanceSD();
    P_k_bar(1,2) = 0.0f;

    P_k_bar(2,0) = 0.0f;
    P_k_bar(2,1) = 0.0f;
    P_k_bar(2,2) = obs1->getBearingSD() * obs1->getBearingSD();
}

void MultiLocEKF::resetLoc(const CornerObservation* c)
{
    assert(!c->isAmbiguous());

    CornerLandmark pt = c->getPossibilities().front();

    // Orientation around north vector
    float theta = NBMath::subPIAngle(pt.angle + c->getVisOrientation());

    xhat_k_bar(x_index) = pt.x + c->getVisDistance() * cos(theta);
    xhat_k_bar(y_index) = pt.y + c->getVisDistance() * sin(theta);
    xhat_k_bar(h_index) = NBMath::subPIAngle(pt.angle
                                             - M_PI_FLOAT
                                             + c->getVisOrientation()
                                             - c->getVisBearing());


    float distVariance = c->getDistanceSD() * c->getDistanceSD();
    float bearingVariance = c->getBearingSD() * c->getBearingSD();
    float orientationVariance = c->getOrientationSD() * c->getOrientationSD();

    P_k_bar(0,0) = distVariance * 2;
    P_k_bar(0,1) = 0.0f;
    P_k_bar(0,2) = 0.0f;

    P_k_bar(1,0) = 0.0f;
    P_k_bar(1,1) = distVariance * 2;
    P_k_bar(1,2) = 0.0f;

    P_k_bar(2,0) = 0.0f;
    P_k_bar(2,1) = 0.0f;
    P_k_bar(2,2) = bearingVariance * 2;
}
