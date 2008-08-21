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
    // Initialize estimate to the center of the field
    setXEst(CENTER_FIELD_X);
    setYEst(CENTER_FIELD_Y);
    setHEst(OPP_GOAL_HEADING);

    // Initialize uncertainty to maximum values
    setXUncert(MAX_UNCERT_X);
    setYUncert(MAX_UNCERT_Y);
    setHUncert(MAX_UNCERT_H);

    // Initialize particles to be equally spread about the field...
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
        // Determine the number of copies of this particle to add
        int count = M * X_bar_t[m].weight / totalWeights;

        // Add the particles to the resampled posterior
        for (int i = 0; i < count; ++i) {
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
float MCL::updateMeasurementModel(vector<Observation> z_t, PoseEst x_t,
                                  int m)
{
    // Give the particle a weight of 1 to begin with
    float w = 1;
    // Expected dist and bearing
    float d_hat;
    float a_hat;
    // Residuals of distance and bearing observations
    float r_d;
    float r_a;
    // Similarity of observation and expectation
    float s_d;
    float s_a;

    // Determine the likelihood of each observation
    for (unsigned int i = 0; i < z_t.size(); ++i) {

        // If unambiguous
        if (not z_t[i].isAmbiguous()) {
            // Determine expected distance to the landmark
            d_hat = sqrt( pow(z_t.getX() - x_t.x, 2) +
                          pow(z_t.getY() - x_t.y, 2));
            // Expected bearing
            a_hat = sub180Angle( atan2(z_t.getY() - x_t.y,
                                       z_t.getX() - x_t.x) * RAD_TO_DEG -
                                 90.0 - x_t.h);
            // Calculate residuals
            r_d = z_t.getDist() - d_hat;
            r_a = z_t.getBearing() - a_hat;

            // Calculate the similarity of the observation and expectation
            // Takes the form e^(-r_d^2/SD(d)^2)
            s_d = exp(-pow(r_d,2) / pow(z_d.getDistSD(), 2));
            s_a = exp(-pow(r_a,2) / pow(z_d.getBearingSD(), 2));

            // Update the weight of the particle
            w *= (s_d*s_a);

        } else { // If ambiguous
            // Determine divergence & likelihood
        }
    }

    return w;
}

/**
 * Method to update the robot pose and uncertainty estimates.
 * Calculates the weighted mean and biased standard deviations of the particles.
 */
void MCL::updateOdometery()
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
