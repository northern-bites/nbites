/**
 * MCL.cpp
 *
 * File houses the main parts of the Monte Carlo Localization system
 *
 * @author Tucker Hermans
 */

#include "MCL.h"

/**
 * Initializes the sampel sets so that the first update works appropriately
 */
MCL::MCL()
{
    // Initialize particles to be randomly spread about the field...
    srand(time(NULL));
    for (int m = 0; m < M; ++m) {
        Particle p_m;
        PoseEst x_m;
        // X bounded by width of the field
        x_m.x = float(rand() % int(FIELD_WIDTH));
        // Y bounded by height of the field
        x_m.y = float(rand() % int(FIELD_HEIGHT));
        // H between +-180
        x_m.h = float((rand() % 360)-180);
        p_m.pose = x_m;
        p_m.weight = 1;
        X_t.push_back(x_m);
    }

    updateEstimates();
}

MCL::~MCL()
{
}

/**
 * Method updates the set of particles and estimates the robots position.
 * Called every frame.
 *
 * @param X_t_1 The set of particles from the previous update.
 * @param u_t The motion (odometery) change since the last update.
 * @param z_t The set of landmark observations in the current frame.
 *
 * @return The set of particles representing the estimate of the current frame.
 */
void MCL::updateLocalization(MotionModel u_t, vector<Observation> z_t)
{
    // Initialize the current set of particles
    vector<Particle> X_t_1 = X_t;
    X_t = NULL;
    vector<Particle> X_bar_t; // A priori estimates
    float totalWeights = 0; // Must sum all weights for future use

    // Run through the particles
    for (int m = 0; m < M; ++m) {
        Particle x_t_m;
        // Update motion model for the particle
        x_t_m.pose = updateOdometery(u_t, X_t_1[m].pose);

        // Update measurement model
        x_t_m.weight = updateMeasurementModel(z_t, x_t, m);

        // Add the particle to the current frame set
        X_bar_t.push_back(x_t_m);
    }

    // Resample the particles
    for (int m = 0; m < M; ++m) {
        // Select particles with probability associated with their weight
        if(X_bar_t[m].weight > (rand() / (float(RAND_MAX)+1))) {
            X_t.push_back(X_bar_t[m]);
        }
    }

    // Update pose and uncertainty estimates
    updateEstimates();
}

/**
 * Method updates the robot pose based on motion changes since the last frame.
 *
 * @param u_t The motion change since the last update.
 * @param x_t The robot pose estimate to be updated.
 * @return The new estimated robot pose.
 */
poseEst MCL::updateOdometery(MotionModel u_t, PoseEst x_t)
{
    // Translate the relative change into the global coordinate system
    float deltaX, deltaY, deltaH;
    float calcFromAngle = (x_t.h + 90.) * DEG_TO_RAD;
    deltaX = u_t.deltaF * cos(calcFromAngle) - u_t.deltaL * sin(calcFromAngle);
    deltaY = u_t.deltaF * sin(calcFromAngle) - u_t.deltaL * cos(calcFromAngle);
    deltaH = deltaR; // Rotational change is the same as heading change

    // Add the change to the current pose estimate
    x_t.x += deltaX;
    x_t.y += deltaY;
    x_t.h += deltaH;

    return x_t;
}

/**
 * Method determines the weight of a particle based on the current landmark
 * observations.
 *
 * @param z_t The landmark observations for the current frame.
 * @param x_t The a priori estimate of the robot pose.
 * @param m The particle ID
 * @return The particle weight
 */
float MCL::updateMeasurementModel(vector<Observation> z_t, PoseEst x_t)
{
    // Give the particle a weight of 1 to begin with
    float w = 1;

    // Determine the likelihood of each observation
    for (unsigned int i = 0; i < z_t.size(); ++i) {

        // Determine the most likely match
        float p = 0; // Combined probability of observation
        float pMax = -1; // Maximum combined probability

        // Loop through all possible landmarks
        // If the observation is distinct, there will only be one possibility
        for (unsigned int j = 0; j < z_t[i].possibilities.size(); ++j) {
            if (z_t[i].isLine()) {
                p = determineLineWeight(z_t[i], x_t, z_t.possibilities[j]);
            } else {
                p = determinePointWeight(z_t[i], x_t, z_t.possibilities[j]);
            }

            if( p > pMax) {
                pMax = p;
            }
        }
        w *= pMax;
    }

    return w; // The combined weight of all observations
}

/**
 * Method to update the robot pose and uncertainty estimates.
 * Calculates the weighted mean and biased standard deviations of the particles.
 */
void MCL::updateEstimates()
{
    PoseEst wMeans = {0.,0.,0.};
    float weightSum = 0.;
    PoseEst bSDs = {0., 0., 0.};

    // Calculate the weighted mean
    for (unsigned int i = 0; i < X_t.size(); ++i) {
        // Sum the values
        wMeans.x += X_t[i].pose.x*X_t[i].weight;
        wMeans.y += X_t[i].pose.y*X_t[i].weight;
        wMeans.h += X_t[i].pose.h*X_t[i].weight;

        // Sum the weights
        weightSum += X_t[i].weight;
    }

    wMeans.x /= weightSum;
    wMeans.y /= weightSum;
    wMeans.h /= weightSum;

    // Calculate the biased variances
    for (unsigned int i=0; i < X_t.size(); ++i) {
        bSDs.x += X_t[i].weight * pow((X_t[i].pose.x - wMeans.x), 2.);
        bSDs.y += X_t[i].weight * pow((X_t[i].pose.y - wMeans.y), 2.);
        bSDs.h += X_t[i].weight * pow((X_t[i].pose.h - wMeans.h), 2.);
    }

    bSDs.x /= weightSum;
    bSDs.x = sqrt(bSDs.x)

    bSDs.y /= weightSum;
    bSDs.y = sqrt(bSDs.y)

    bSDS.h /= weightSum;
    bSDs.h = sqrt(bSDs.h)

    // Set class variables to reflect newly calculated values
    curEst = wMeans;
    curUncert = bSDs;
}

//Helpers

/**
 * Determine the weight to compound the particle weight by for a single
 * observation of an landmark point
 *
 * @param z    the observation to determine the weight of
 * @param x_t  the a priori estimate of the robot pose.
 * @param l    the landmark to be used as basis for the observation
 * @return     the probability of the observation
 */
float MCL::determinePointWeight(Observation z, PoseEst x_t, Landmark l)
{
    // Expected dist and bearing
    float d_hat;
    float a_hat;
    // Residuals of distance and bearing observations
    float r_d;
    float r_a;

    // Determine expected distance to the landmark
    d_hat = sqrt( pow(z.getX() - x_t.x, 2) +
                  pow(z.getY() - x_t.y, 2));
    // Expected bearing
    a_hat = sub180Angle( atan2(z.getY() - x_t.y, z.getX() - x_t.x) *
                         RAD_TO_DEG - 90.0 - x_t.h);
    // Calculate residuals
    r_d = z.getVisDist() - d_hat;
    r_a = z.getVisBearing() - a_hat;

    return getSimilarity(r_d, r_a, z);
}

/**
 * Determine the simalirty between the observed and expected of a line.
 *
 * @param z    the observation to determine the weight of
 * @param x_t  the a priori estimate of the robot pose.
 * @param l    the landmark to be used as basis for the observation
 * @return     the probability of the observation
 */
float MCL::determineLineWeight(Observation z, PoseEst x_t, LocLandmark l)
{
    // Distance and bearing for expected point
    float d_hat;
    float a_hat;

    // Residuals of distance and bearing observations
    float r_d;
    float r_a;

    // Determine nearest expected point on the line
    // NOTE:  This is currently too naive, we need to extrapolate the entire
    // line from the vision inforamtion and find the closest expected distance
    point<float, float> pt_hat;
    if (z.isVertical()) {
        pt_hat.x = l.getX1();
        pt_hat.y = x_t.getY();

        // Make sure we aren't outside the line
        if ( pt_hat.y < l.getY1()) {
            pt_hat.y = l.getY1();
        } else if ( pt_hat.y > l.getY2()) {
            pt_hat.y = l.getY2();
        }
    } else { // Line is horizontal
        pt_hat.x = x_t.getX();
        pt_hat.y = l.getY1();

        // Make sure we aren't outside the line
        if ( pt_hat.x < l.getX1()) {
            pt_hat.x = l.getX1();
        } else if ( pt_hat.x > l.getX2()) {
            pt_hat.x = l.getX2();
        }
    }

    // Get dist and bearing to expected point
    d_hat = sqrt( pow(pt_hat.x - x_t.x, 2) +
                  pow(pt_hat.y - x_t.y, 2));

    // Expected bearing
    a_hat = sub180Angle( atan2(pt_hat.y - x_t.y, pt_hat.x - x_t.x) *
                         RAD_TO_DEG - 90.0 - x_t.h);

    // Calculate residuals
    r_d = fabs(z.getVisDist() - d_hat);
    r_a = fabs(z.getVisBearing() - a_hat);

    return getSimilarity(r_d, r_a, z);
}

/**
 * Determine the similarity of an observation to a landmark location
 *
 * @param r_d     The difference between the expected and observed distance
 * @param r_a     The difference between the expected and observed bearing
 * @param sigma_d The standard deviation of the distance measurement
 * @param sigma_a The standard deviation of the bearing measurement
 * @return        The combined similarity of the landmark observation
 */
float MCL::getSimilarity(float r_d, float r_a, float sigma_d,
                          float sigma_a)
{
    // Similarity of observation and expectation
    float s_d;
    float s_a;

    // Calculate the similarity of the observation and expectation
    // Takes the form e^(-r_d^2/SD(d)^2)
    s_d = exp(-pow(r_d,2) / pow(sigma_d, 2));
    s_a = exp(-pow(r_a,2) / pow(sigma_a, 2));

    // Update the weight of the particle
    // We multiple the weight till now with the combined probability of
    // this iterations sighting
    return s_d*s_a;
}
