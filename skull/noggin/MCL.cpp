/**
 * MCL.cpp
 *
 * File houses the main parts of the Monte Carlo Localization system
 *
 * @author Tucker Hermans
 */


/**
 * Initializes the sampel sets so that the first update works appropriately
 */
MCL::MCL()
{
    // Initialize estimate to the center of the field
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
 * @param z_t The set of landmark sightings in the current frame.
 *
 * @return The set of particles representing the estimate of the current frame.
 */
vector<Particle> MCL::updateLoc(vector<Particle> X_t_1, MotionModel u_t,
                                vector<Measurement> z_t)
{
    // Initialize the current set of particles
    vector<Particle> X_bar_t; // A priori estimates
    vector<Particle> X_t; // Posterior estimates

    // Run through the particles
    for (int m = 0; m < M; ++m) {
        Particle X_m;
        // Update motion model for the particle
        x_t_m.pose = updateOdometery(u_t, X_t_1[m].pose);

        // Update measurement model
        x_t_m.weight = updateMeasurementModel(z_t, x_t, m);

        // Add the particle to the current frame set
        X_bar_t.push_back(x_t_m);
    }

    // Resample the particles
    for (int m = 0; m < M; ++m) {
        // Draw
        int I = X_bar_t[m].weight;

        // Add to particle set
        for (int i = 0; i < I; ++i) {
            X_t.push_back(X_bar_t[m])
        }
    }

    // Set estimates to weighted means

    return X_t;
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
 * @param z_t The landmark sightings for the current frame.
 * @param x_t The a priori estimate of the robot pose.
 * @param m The particle ID
 * @return The particle weight
 */
float MCL::updateMeasurementModel(vector<Measurement> z_t, PoseEst x_t,
                                  int m)
{
}
