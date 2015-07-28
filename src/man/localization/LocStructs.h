/**
 * @brief Collection of utilities for use in localization
 *
 * @author EJ Googins <egoogins@bowdoin.edu>
 * @date   Febraury 2013
 * @author Josh Imhoff <joshimhoff13@gmail.com>
 * @date   June 2015
 */

#pragma once

#include "NBMath.h"
#include "FieldConstants.h"
#include "RobotLocation.pb.h"

#include <vector>
#include <cmath>
#include <iostream>
#include <boost/random.hpp>
#include <boost/random/normal_distribution.hpp>
#include <boost/random/uniform_real.hpp>
#include <boost/math/constants/constants.hpp>

namespace man
{
namespace localization
{

// Parameters for particle filter, motion model, and vision model
struct ParticleFilterParams
{
    // General particle filter parameters
    float numParticles;           // num of particles in swarm

    // Particle injection parameters
    bool  injectionOn;            // augmented MCL with injections if true
    float alphaFast;              // weight factor for fast exponential weight filter
    float alphaSlow;              // weight factor for slow exponential weight filter

    // Motion model parameters
    float odometryXYNoise;        // variance for x,y in MotionSystem when updating
    float odometryHNoise;         // variance for h in MotionSystem when updating

    // Sensor model parameters
    // Line model
    float lineTiltStdev;          // standard deviation of tilt to line (radians)
    float lineBearingStdev;       // standard deviation of bearing to line (radians)
    float lineEndpointStdev;      // standard deviation of endpoints of line (cm)

    // Landmark model
    float landmarkTiltStdev;      // standard deviation of tilt to landmark (radians)
    float landmarkBearingStdev;   // standard deviation of bearing to landmark (radians)
};

// TODO remove
static float sampleNormal(float mean, float sigma) {
    // Seed the random number generator.
    static boost::mt19937 rng(static_cast<unsigned>(std::time(0)));

    boost::normal_distribution<float> dist(mean, sigma);

    boost::variate_generator<boost::mt19937&,
                             boost::normal_distribution<float> > sample(rng, dist);

    return sample();
}

// TODO remove
struct LocNormalParams
{
    LocNormalParams(float sx, float sy, float sh)
        : sigma_x(sx), sigma_y(sy), sigma_h(sh)
        {
        }
    LocNormalParams()
        : sigma_x(.50f), sigma_y(.50f), sigma_h(.1f)
        {
        }

    float sigma_x;
    float sigma_y;
    float sigma_h;
};

// Represents a robot pose reconstructed from observations
// Injections come from calling sample() on a ReconstructedLocation
struct ReconstructedLocation
{
    float x;
    float y;
    float h;

    // NOTE because observations have uncertainity associated with them, 
    //      reconstructed locations should take into account that uncertainity,
    //      thus we center gaussians around the reconstructed pose and sample from
    //      them before injecting particles
    float xSigma;
    float ySigma;
    float hSigma;

    boost::mt19937 rng;

    // Constructor
    // @param x_, the x component of reconstructed robot pose
    // @param y_, the y component of reconstructed robot pose
    // @param h_, the h component of reconstructed robot pose
    // @param xs_, uncertainity in the x component assuming gaussian noise
    // @param ys_, uncertainity in the y component assuming gaussian noise
    // @param hs_, uncertainity in the h component assuming gaussian noise
    // @note xs_, ys_, and hs_ are the standard deviations of the gaussians in 
    //       x, y, and h components that generate noise to be added to reconstructed
    //       robot pose
    ReconstructedLocation(float x_, float y_, float h_, float xs_, float ys_, float hs_)
        : x(x_), y(y_), h(h_), xSigma(xs_), 
          ySigma(ys_), hSigma(hs_), rng(time(0))
    {}

    // Sample a robot pose most often to be used as an injection
    // @returns reconstructed pose with added noise
    // @note xs, ys, and hs are the standard deviations of the gaussians in 
    //       x, y, and h components that generate noise to be added to reconstructed
    //       robot pose
    messages::RobotLocation sample() {
        messages::RobotLocation sampled;

        boost::normal_distribution<> xGaussian(x, xSigma);
        boost::normal_distribution<> yGaussian(y, ySigma);
        boost::normal_distribution<> hGaussian(h, hSigma);

        boost::variate_generator<boost::mt19937&,
                                 boost::normal_distribution<> > xNoise(rng, xGaussian);
        boost::variate_generator<boost::mt19937&,
                                 boost::normal_distribution<> > yNoise(rng, yGaussian);
        boost::variate_generator<boost::mt19937&,
                                 boost::normal_distribution<> > hNoise(rng, hGaussian);

        sampled.set_x(xNoise());
        sampled.set_y(yNoise());
        sampled.set_h(hNoise());

        return sampled;
    }

    // Check if reconstructed robot pose is on field
    // @returns if robot pose is on field
    bool onField() { 
        return (x >= GREEN_PAD_X && x <= FIELD_GREEN_WIDTH - GREEN_PAD_X && 
                y >= GREEN_PAD_Y && y <= FIELD_GREEN_HEIGHT - GREEN_PAD_Y); 
    }
};

} // namespace localization
} // namespace man
