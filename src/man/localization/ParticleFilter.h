/**
 * Implementation of a particle filter localization
 * system for robot self-localization.
 *
 * @author Ellis Ratner <eratner@bowdoin.edu>
 * @date   May 2012 (updated January 2013)
 */
#pragma once

#include "LocalizationModule.h"
#include "SensorModel.h"
#include "MotionModel.h"
#include "Particle.h"
#include "../man/memory/protos/Common.pb.h"

#include <vector>
#include <iostream>
#include <map>
#include <boost/shared_ptr.hpp>
#include <boost/random.hpp>
#include <boost/random/normal_distribution.hpp>
#include <boost/random/uniform_real.hpp>
#include <boost/math/constants/constants.hpp>

namespace man
{
    namespace localization
    {
	// @todo move this to a common file!
	/**
	 * @brief  Samples a Gaussian normal distribution of specified
	 *         mean and standard deviation (sigma.)
	 * @param  mean the mean of the data.
	 * @param  sigma the standard deviation of the data.
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

	/**
	 * @struct ParticleFilterParams
	 * @brief Parameters used for the particle filter.
	 */
	struct ParticleFilterParams
	{
	    float fieldHeight_;        //! Field height.
	    float fieldWidth_;         //! Field width.
	    float numParticles_;       //! Size of particle population.
	    float alpha_fast_;         //! Weight factor for fast exponential weight filter.
	    float alpha_slow_;         //! Weight factor for slow exponential weight filter.
	};

	static const ParticleFilterParams DEFAULT_PARAMS =
	{
	    FIELD_GREEN_HEIGHT,
	    FIELD_GREEN_WIDTH,
	    200,
	    0.2f,
	    0.05f
	};
	
	/**
	 * @class ParticleFilter
	 * @brief The main particle filter localization class. Handles
	 *        functionality for constructing a posterior belief
	 *        based on a prior belief function as well as latest
	 *        sensor and control data.
	 */
	class ParticleFilter : public LocalizationModule
	{

	public:
	    ParticleFilter(boost::shared_ptr<MotionModel> motionModel,
			   boost::shared_ptr<SensorModel> sensorModel,
			   ParticleFilterParams parameters = DEFAULT_PARAMS);

	    ~ParticleFilter();

	    /**
	     * @brief Runs a single iteration of the particle filter algorithm, 
	     *        incorperating motion and sensor data according to the motionUpdate
	     *        and sensorUpdate flags, respectively.
	     */
	    void filter(bool motionUpdate = true, bool sensorUpdate = true);

	    ParticleSet getParticles() const { return particles_; }

	    /**
	     * @brief Returns the particle with the highest weight in the set
	     *        (i.e., the "best" particle.)
	     */
	    Particle getBestParticle();

	    /**
	     * @brief Find the standard deviation of the particle set. This is 
	     *        a useful metric for determining the error in the current
	     *        estimate, or the rate of change of error over time.
	     */
	    std::vector<float> findParticleSD() const;

	    void resetLocalization();

	private:
	    /**
	     * @brief Resamples (with replacement) the particle population according
	     *        to the normalized weights of the particles. 
	     */
	    void resample();

	    /**
	     * @brief Update localization using the particle filter algorithm 
	              to incorperate motion and sensor data. 
	     */
	    void updateLocalization(/* @todo */);

	    ParticleFilterParams parameters_;

	    man::memory::protos::RobotLocation poseEstimate_;

	    std::vector<float> standardDeviations_;

	    ParticleSet particles_;
	    boost::shared_ptr<MotionModel> motionModel_;
	    boost::shared_ptr<SensorModel> sensorModel_;
	};
    } // namespace localization
} // namespace man
