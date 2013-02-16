/**
 * @brief Some useful funcitons and structs for the localization system
 *
 * @author EJ Googins <egoogins@bowdoin.edu>
 * @date   Febraury 2013
 */

#pragma once

#include <vector>
#include <cmath>

#include "Common.pb.h"
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
     * Contains a two-dimensional spatial vector defined
     * by a magnitude and direction (position vector).
     */
    struct RelVector
    {
        RelVector(float mag = 0.0f, float dir = 0.0f)
            : magnitude(mag), direction(dir)
            { }

        float magnitude;
        float direction;

        friend std::ostream& operator<<(std::ostream& out, RelVector v)
            {
                out << "magnitude: " << v.magnitude << ", "
                    << "direciton: " << v.direction << "\n";
                return out;
            }
    };


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

    // /**
    //  * Finds the position vector to the point (x, y) in the
    //  * specified coordinate frame.
    //  * @param origin for the vector
    //  * @param x x-coordinate.
    //  * @param y y-coordinate.
    //  * @return the position vector from the origin to the point (x', y').
    //  */
    // static RelVector getRelativeVector(messages::RobotLocation origin,
    //                                        float x, float y)
    // {

    //     float dx = x - origin.x();
    //     float dy = y - origin.y();

    //     float magnitude = std::sqrt(dx*dx + dy*dy);

    //     float sinh, cosh;
    //     sincosf(-origin.h(), &sinh, &cosh);

    //     float x_prime = cosh * dx - sinh * dy;
    //     float y_prime = sinh * dx + cosh * dy;

    //     float bearing = NBMath::safe_atan2(y_prime, x_prime);

    //     return RelVector(magnitude, bearing);
    // }


        // A struct for storing the Loc Parameters
        struct LocNormalParams
        {
            LocNormalParams(float sx, float sy, float sh)
                : sigma_x(sx), sigma_y(sy), sigma_h(sh)
                {
                }

            LocNormalParams()
                : sigma_x(15.0), sigma_y(15.0), sigma_h(1.0)
                {
                }

            float sigma_x;
            float sigma_y;
            float sigma_h;
        };


    } // namespace localization
} // namespace man
