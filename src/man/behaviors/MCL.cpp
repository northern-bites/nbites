/**
 * MCL.cpp
 *
 * File houses the main parts of the Monte Carlo Localization system
 *
 * @author Tucker Hermans
 */

#include "MCL.h"
#include "NBMath.h"
#include <time.h> // for srand(time(NULL))
#include <cstdlib> // for MAX_RAND
using namespace std;
#define MAX_CHANGE_X 5.0f
#define MAX_CHANGE_Y 5.0f
#define MAX_CHANGE_H M_PI_FLOAT / 16.0f
#define MAX_CHANGE_F 5.0f
#define MAX_CHANGE_L 5.0f
#define MAX_CHANGE_R M_PI_FLOAT / 16.0f
#define UNIFORM_1_NEG_1 (2.0f*(static_cast<float>(rand()) / (static_cast<float>(RAND_MAX)+1)) - 1.0f)

/**
 * Initializes the sampel sets so that the first update works appropriately
 */
MCL::MCL(int _M) : useBest(false), lastOdo(0,0,0), frameCounter(0), M(_M)
{
    // Initialize particles to be randomly spread about the field...
    srand(time(NULL));
    for (int m = 0; m < M; ++m) {
        //Particle p_m;
        // X bounded by width of the field
        // Y bounded by height of the field
        // H between +-pi
        PoseEst x_m(static_cast<float>(rand() % static_cast<int>(FIELD_WIDTH)),
                    static_cast<float>(rand() % static_cast<int>(FIELD_HEIGHT)),
                    UNIFORM_1_NEG_1 * (M_PI_FLOAT / 2.0f));
        Particle p_m(x_m, 1.0f);
        X_t.push_back(p_m);
    }

    updateEstimates();
}

MCL::~MCL()
{
}

/**
 * Method to reset the particle filter to an initial state.
 */
void MCL::reset()
{
    frameCounter = 0;

    for (int m = 0; m < M; ++m) {
        //Particle p_m;
        // X bounded by width of the field
        // Y bounded by height of the field
        // H between +-pi
        PoseEst x_m(static_cast<float>(rand() % static_cast<int>(FIELD_WIDTH)),
                    static_cast<float>(rand() % static_cast<int>(FIELD_HEIGHT)),
                    UNIFORM_1_NEG_1 * (M_PI_FLOAT / 2.0f));
        Particle p_m(x_m, 1.0f);
        X_t.push_back(p_m);
    }

    updateEstimates();
}

/**
 * Method updates the set of particles and estimates the robots position.
 * Called every frame.
 *
 * @param u_t The motion (odometery) change since the last update.
 * @param z_t The set of landmark observations in the current frame.
 * @param resample Should we resample during this update
 * @return The set of particles representing the estimate of the current frame.
 */
void MCL::updateLocalization(MotionModel u_t, vector<Observation> z_t)
{
    frameCounter++;
    // Set the current particles to be of time minus one.
    vector<Particle> X_t_1 = X_t;
    // Clar the current set
    X_t.clear();
    vector<Particle> X_bar_t; // A priori estimates
    float totalWeights = 0.; // Must sum all weights for future use

    // Run through the particles
    for (int m = 0; m < M; ++m) {
        Particle x_t_m;

        // Update motion model for the particle
        x_t_m.pose = updateMotionModel(X_t_1[m].pose, u_t);
        // Update measurement model
        x_t_m.weight = updateMeasurementModel(z_t, x_t_m.pose);
        totalWeights += x_t_m.weight;
        // Add the particle to the current frame set
        X_bar_t.push_back(x_t_m);
    }

    // Resample the particles
    if (frameCounter % 1 == 0) {
        resample(&X_bar_t, totalWeights);
    } else {
        noResample(&X_bar_t);
    }

    // Update pose and uncertainty estimates
    updateEstimates();

}

/**
 * Update a particle's pose based on the last motion model.
 * We sample the pose with noise proportional to the odometery update.
 *
 * @param x_t_1 The robot pose from the pervious position distribution
 * @param u_t The odometry update from the last frame
 *
 * @return A new robot pose sampled on the odometry update
 */
PoseEst MCL::updateMotionModel(PoseEst x_t, MotionModel u_t)
{
    lastOdo = u_t;
    u_t.deltaF -= sampleNormalDistribution(fabs(u_t.deltaF));
    u_t.deltaL -= sampleNormalDistribution(fabs(u_t.deltaL));
    u_t.deltaR -= sampleNormalDistribution(fabs(u_t.deltaR));

    // u_t.deltaF -= sampleTriangularDistribution(fabs(u_t.deltaF));
    // u_t.deltaL -= sampleTriangularDistribution(fabs(u_t.deltaL));
    // u_t.deltaR -= sampleTriangularDistribution(fabs(u_t.deltaR));

    x_t += u_t;

    x_t.h = NBMath::subPIAngle(x_t.h);

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

    //cout << "New Particle" << endl;
    // Determine the likelihood of each observation
    for (unsigned int i = 0; i < z_t.size(); ++i) {

        // Determine the most likely match
        float p = 0; // Combined probability of observation
        float pMax = -1; // Maximum combined probability

        // Get local copies of our possible landmarks
        vector<LineLandmark> possibleLines = z_t[i].getLinePossibilities();
        vector<PointLandmark> possiblePoints = z_t[i].getPointPossibilities();

        // Loop through all possible landmarks
        // If the observation is distinct, there will only be one possibility
        for (unsigned int j = 0; j < z_t[i].getNumPossibilities(); ++j) {
            if (z_t[i].isLine()) {
                p = determineLineWeight(z_t[i], x_t, possibleLines[j]);
            } else {
                p = determinePointWeight(z_t[i], x_t, possiblePoints[j]);
            }

            //cout << "p is: " << p << " ";
            if( p > pMax) {
                pMax = p;
            }
        }
        //cout << "pMax is: " << pMax << endl;
        w *= pMax;
    }

    return w; // The combined weight of all observations
}

/**
 * Method to resample the particles based on a straight proportion of their
 * weights. Adds copies of the paritcle jittered proportional to the weight
 * of the particle
 *
 * @param X_bar_t the set of particles before being resampled
 * @param totalWeights the totalWeights of the particle set X_bar_t
 */
void MCL::resample(std::vector<Particle> * X_bar_t, float totalWeights) {
    for (int m = 0; m < M; ++m) {
        // Normalize the particle weights
        (*X_bar_t)[m].weight /= totalWeights;

        int count = int(round(float(M) * (*X_bar_t)[m].weight));
        for (int i = 0; i < count; ++i) {
            // Random walk the particles
            X_t.push_back(randomWalkParticle((*X_bar_t)[m]));
            //X_t.push_back(X_bar_t[m]);
        }
    }
}

void MCL::lowVarianceResample(std::vector<Particle> * X_bar_t,
                              float totalWeights) {
    float r = ((static_cast<float>(rand()) /
				static_cast<float>(RAND_MAX)) *
			   (1.0f/static_cast<float>(M) ));
    float c = (*X_bar_t)[0].weight / totalWeights;
    int i = 0;
    for (int m = 0; m < M; ++m) {

        float U = r = static_cast<float>(m) * (1.0f/static_cast<float>(M));

        // Normalize the particle weights
        (*X_bar_t)[m].weight /= totalWeights;

        while ( U > c) {
            i++;
            c += (*X_bar_t)[m].weight;
        }
        X_t.push_back(randomWalkParticle((*X_bar_t)[i]));
    }
}
/**
 * Prepare for the next update step without resampling the particles
 *
 * @param X_bar_t the set of updated particles
 */
void MCL::noResample(std::vector<Particle> * X_bar_t) {
    X_t = *X_bar_t;
}

/**
 * Method to update the robot pose and uncertainty estimates.
 * Calculates the weighted mean and biased standard deviations of the particles.
 */
void MCL::updateEstimates()
{
    float weightSum = 0.;
    PoseEst wMeans(0.,0.,0.);
    PoseEst bSDs(0., 0., 0.);
    PoseEst best(0.,0.,0.);
    float maxWeight = 0;

    // Calculate the weighted mean
    for (unsigned int i = 0; i < X_t.size(); ++i) {
        // Sum the values
        wMeans.x += X_t[i].pose.x*X_t[i].weight;
        wMeans.y += X_t[i].pose.y*X_t[i].weight;
        wMeans.h += X_t[i].pose.h*X_t[i].weight;
        // Sum the weights
        weightSum += X_t[i].weight;

        if (X_t[i].weight > maxWeight) {
            maxWeight = X_t[i].weight;
            best = X_t[i].pose;
        }
    }

    wMeans.x /= weightSum;
    wMeans.y /= weightSum;
    wMeans.h /= weightSum;

    wMeans.h = NBMath::subPIAngle(wMeans.h);

    // Calculate the biased variances
    for (unsigned int i=0; i < X_t.size(); ++i) {
        bSDs.x += X_t[i].weight *
            (X_t[i].pose.x - wMeans.x)*
            (X_t[i].pose.x - wMeans.x);
        bSDs.y += X_t[i].weight *
            (X_t[i].pose.y - wMeans.y)*
            (X_t[i].pose.y - wMeans.y);
        bSDs.h += X_t[i].weight *
            (X_t[i].pose.h - wMeans.h)*
            (X_t[i].pose.h - wMeans.h);
    }

    bSDs.x /= weightSum;
    bSDs.x = sqrt(bSDs.x);

    bSDs.y /= weightSum;
    bSDs.y = sqrt(bSDs.y);

    bSDs.h /= weightSum;
    bSDs.h = sqrt(bSDs.h);

    // Set class variables to reflect newly calculated values
    curEst = wMeans;
    curBest = best;
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
float MCL::determinePointWeight(Observation z, PoseEst x_t, PointLandmark pt)
{
    // Expected dist and bearing
    float d_hat;
    float a_hat;
    // Residuals of distance and bearing observations
    float r_d;
    float r_a;

    // Determine expected distance to the landmark
    d_hat = static_cast<float>(hypot(pt.x - x_t.x, pt.y - x_t.y));
    // Expected bearing
    a_hat = atan2(pt.y - x_t.y, pt.x - x_t.x) - x_t.h;

    // Calculate residuals
    r_d = z.getVisDistance() - d_hat;
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
float MCL::determineLineWeight(Observation z, PoseEst x_t, LineLandmark line)
{
    // Distance and bearing for expected point
    float d_hat;
    float a_hat;

    // Residuals of distance and bearing observations
    float r_d;
    float r_a;

    // Nearest point on the line
    PointLandmark pt;
    // Slopes
    float m;

    if (line.x2 - line.x1 != 0) { // Check if the line is vertical
        m = (line.y2 - line.y1) / (line.x2 - line.x1);

        if (m != 0) { // Line is on a slope
            pt.x = (line.y1 - x_t.y + m*line.x1 + m*x_t.x) *
                (m / (2*m + 1));
            pt.y = m * (pt.x - line.x1) + line.y1;
        } else { // Line is horizontal; ortho is vertical
            pt.x = x_t.x;
            pt.y = line.y1;
        }
    } else { // Line is vertical
        pt.x = line.x1;
        pt.y = x_t.y;
    }

    // Check if the intersecting point is on the line
    if( ((line.x1 < line.x2) && (pt.x < line.x1 || pt.x > line.x2)) ||
        ((line.x1 > line.x2) && (pt.x > line.x1 || pt.x < line.x2)) ||
        ((line.y1 < line.y2) && (pt.y < line.y1 || pt.y > line.y2)) ||
        ((line.y1 > line.y2) && (pt.y > line.y1 || pt.y < line.y2))) {
        // Point is outside the bound of the bounds of the line segment
        float d_1 = static_cast<float>(hypot(line.x1 - x_t.x, line.y1 - x_t.y));
        float d_2 = static_cast<float>(hypot(line.x2 - x_t.x, line.y2 - x_t.y));
        if (d_1 < d_2) {
            d_hat = d_1;
            a_hat = atan2(line.y1 - x_t.y, line.x1 - x_t.x) - x_t.h;
        } else {
            d_hat = d_2;
            a_hat = atan2(line.y2 - x_t.y, line.x2 - x_t.x) - x_t.h;
        }

    } else {

        // Determine nearest expected point on the line
        d_hat = static_cast<float>(hypot(pt.x - x_t.x, pt.y - x_t.y));
        // Expected bearing
        a_hat = atan2(pt.y - x_t.y, pt.x - x_t.x) - x_t.h;
    }

    // Calculate residuals
    r_d = fabs(z.getVisDistance() - d_hat);
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
float MCL::getSimilarity(float r_d, float r_a, Observation &z)
{
    // Similarity of observation and expectation
    float s_d_a;
    float sigma_d = z.getDistanceSD();
    float sigma_a = z.getBearingSD();
    // Calculate the similarity of the observation and expectation
    s_d_a = exp((-(r_d*r_d) / (sigma_d*sigma_d))
                -((r_a*r_a) / (sigma_a*sigma_a)));

    if (s_d_a < MIN_SIMILARITY) {
        s_d_a = MIN_SIMILARITY;
    }
    return s_d_a;
}

/**
 * Move a particle randomly in the x, y, and h directions proportional
 * to its weight, within a certian bounds.
 *
 * @param p The particle to be random walked
 *
 * @return The walked particle
 */
Particle MCL::randomWalkParticle(Particle p)
{
    p.pose.x += sampleNormalDistribution(MAX_CHANGE_X * (1.0f - p.weight));
    p.pose.y += sampleNormalDistribution(MAX_CHANGE_Y * (1.0f - p.weight));
    p.pose.h += sampleNormalDistribution(MAX_CHANGE_H * (1.0f - p.weight));

    p.pose.h = NBMath::subPIAngle(p.pose.h);

    return p;
}

float MCL::sampleNormalDistribution(float sd)
{
    float samp = 0;
    for(int i = 0; i < 12; i++) {
        samp += (2*(static_cast<float>(rand()) /
					static_cast<float>(RAND_MAX)) * sd) - sd;
    }
    return 0.5f*samp;
}

float MCL::sampleTriangularDistribution(float sd)
{
    return std::sqrt(6.0f)*0.5f * ((2.0f*sd*(static_cast<float>(rand()) /
								   static_cast<float>(RAND_MAX))) - sd +
                            (2.0f*sd*(static_cast<float>(rand()) /
								   static_cast<float>(RAND_MAX))) - sd);
}

// Particle
Particle::Particle(PoseEst _pose, float _weight) :
    pose(_pose), weight(_weight)
{
}

Particle::Particle(const Particle& other) :
    pose(other.pose), weight(other.weight)
{
}

Particle::Particle(){}
