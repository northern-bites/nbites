/**
 * @brief Some useful funcitons and structs for the localization system
 *
 * @author EJ Googins <egoogins@bowdoin.edu>
 * @date   Febraury 2013
 */

#pragma once

#include <vector>
#include <cmath>

#include "NBMath.h"

#include <boost/random.hpp>
#include <boost/random/normal_distribution.hpp>
#include <boost/random/uniform_real.hpp>
#include <boost/math/constants/constants.hpp>

namespace man
{
    namespace localization
    {
    /**
     * @struct ParticleFilterParams
     * @brief Parameters used for the particle filter.
     */
    struct ParticleFilterParams
    {
        float fieldHeight;        //! Field height.
        float fieldWidth;         //! Field width.
        float numParticles;       //! Size of particle population.
        float alpha_fast;         //! Weight factor for fast exponential weight filter.
        float alpha_slow;         //! Weight factor for slow exponential weight filter.
        float odometryXYNoise;    //! Variance for x,y in MotionSystem when updating.
        float odometryHNoise;     //! Variance for h in MotionSystem when updating.
        // Need to add how much we prefer best particles
    };



    /**
     * Samples a Gaussian normal distribution of specified
     * mean and standard deviation (sigma.)
     * @param mean the mean of the data.
     * @param sigma the standard deviation of the data.
     * @return A random sample of the specified normal
     *         distribution.
     */
    static float sampleNormal(float mean, float sigma)
    {
        // Seed the random number generator.
        static boost::mt19937 rng(static_cast<unsigned>(std::time(0)));

        boost::normal_distribution<float> dist(mean, sigma);

        boost::variate_generator<boost::mt19937&,
                       boost::normal_distribution<float> > sample(rng, dist);

        return sample();
    }

    // A struct for storing the Loc Parameters
    struct LocNormalParams
    {
        LocNormalParams(float sx, float sy, float sh)
            : sigma_x(sx), sigma_y(sy), sigma_h(sh)
            {
            }
        LocNormalParams()
            : sigma_x(10.0f), sigma_y(10.0f), sigma_h(.8f)
            {
            }

        float sigma_x;
        float sigma_y;
        float sigma_h;
    };


    } // namespace localization
} // namespace man
