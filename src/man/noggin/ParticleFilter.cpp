#include "ParticleFilter.h"

namespace PF
{
    /**
     * Used to compare two particles by weight, primarily for
     * use with the sorting algorithm.
     *
     * @param first the first particle.
     * @param second the second particle.
     * @return weight of first < weight of second ? true : false
     */
    bool operator <(const LocalizationParticle& first,
                    const LocalizationParticle& second)
    {
        float w1 = first.getWeight();
        float w2 = second.getWeight();

        return (w1 < w2) ? true : false;
    }

    ParticleFilter::ParticleFilter(boost::shared_ptr<MotionModel> motion,
                                   boost::shared_ptr<SensorModel> sensor,
                                   MLocalization::ptr mLocalization,
                                   ParticleFilterParams params)
        : parameters(params), xEstimate(0.0f), yEstimate(0.0f),
          hEstimate(0.0f), averageWeight(0.0f), wFast(0.0f),
          wSlow(0.0f),
          memoryProvider(&ParticleFilter::updateMemory, this, mLocalization)
    {
        motionModel = motion;
        sensorModel = sensor;

        boost::mt19937 rng;
        rng.seed(std::time(0));

        boost::uniform_real<float> xBounds(0.0f,
                                   (float) parameters.fieldWidth);
        boost::uniform_real<float> yBounds(0.0f,
                                   (float) parameters.fieldHeight);
        boost::uniform_real<float> angleBounds(0,
                                   2.0f*boost::math::constants::pi<float>());

        boost::variate_generator<boost::mt19937&,
                             boost::uniform_real<float> > xGen(rng, xBounds);
        boost::variate_generator<boost::mt19937&,
                             boost::uniform_real<float> > yGen(rng, yBounds);
        boost::variate_generator<boost::mt19937&,
                      boost::uniform_real<float> > angleGen(rng, angleBounds);

    // Assign uniform weight.
        float weight = 1.0f/(((float)parameters.numParticles)*1.0f);

        #ifdef DEBUG_LOCALIZATION
        std::cout << "Weight = " << weight << "." << std::endl;
        #endif

        for(int i = 0; i < parameters.numParticles; ++i)
        {
            LocalizationParticle p(Location(xGen(), yGen(), angleGen()),
                                   weight);

            particles.push_back(p);

           #ifdef DEBUG_LOCALIZATION
            std::cout << "Creating particle " << i+1 << " with coordinates ("
                      << p.getLocation().x << ", " << p.getLocation().y << ", "
                      << p.getLocation().angle << ") and weight"
                      << p.getWeight() << "." << std::endl;
           #endif
        }
    }

    ParticleFilter::~ParticleFilter()
    {

    }

    void ParticleFilter::run(bool motionUpdate, bool sensorUpdate)
    {
        if(motionUpdate && motionModel)
        {
            particles = motionModel->update(particles);
        }

        if(sensorUpdate && sensorModel)
        {
            particles = sensorModel->update(particles);
        }

        // Only resample if the sensor model has updated particle weights.
        if(sensorModel->hasUpdated())
        {
            resample();
        }
        else
        {
            //std::cout << "No sensor update." << std::endl;
        }

        // Update estimates.

        // Use the robust mean...

        float sumX = 0;
        float sumY = 0;
        float sumH = 0;

        ParticleIt iter;
        for(iter = particles.begin(); iter != particles.end(); ++iter)
        {
            Location l = (*iter).getLocation();
            sumX += l.x;
            sumY += l.y;
            sumH += l.heading;
        }

        xEstimate = sumX/parameters.numParticles;
        yEstimate = sumY/parameters.numParticles;
        hEstimate = sumH/parameters.numParticles;

	// Check if the mean has gone out of bounds. If so, 
	// reset to the closest point in bounds with appropriate
	// uncertainty.
	bool resetInBounds = false;

	if(xEstimate < 0)
	{
	    resetInBounds = true;
	    xEstimate = 0;
	}
	else if(xEstimate > parameters.fieldWidth)
	{
	    resetInBounds = true;
	    xEstimate = parameters.fieldWidth;
	}

	if(yEstimate < 0)
	{
	    resetInBounds = true;
	    yEstimate = 0;
	}
	else if(yEstimate > parameters.fieldHeight)
	{
	    resetInBounds = true;
	    yEstimate = parameters.fieldHeight;
	}

	// Only reset if one of the location coordinates is
	// out of bounds; avoids unnecessary resets.
	if(resetInBounds)
	{
	    std::cout << "Resetting to (" << xEstimate
		      << ", " << yEstimate << ", "
		      << hEstimate << ")." << std::endl;

	    resetLocTo(xEstimate, yEstimate, hEstimate,
		       LocNormalParams());
	}

        memoryProvider.updateMemory();
    }

    /**
     * Finds the "best" particle (the particle with the highest
     * weight) from the current set, and returns it.
     * @return a LocalizationParticle that has the greatest weight
     *         in the current set.
     */
    LocalizationParticle ParticleFilter::getBestParticle()
    {
        // Sort the particles in ascending order.
        std::sort(particles.begin(), particles.end());

        // The last particle should have the greatest weight.
        return particles[particles.size()-1];
    }

    /**
     * A resampling algorithm to construct the posterior belief distribution
     * from the prior belief. The "fittest" particles survive and are
     * the most prevalent in the resulting set of particles. Replaces the
     * existing particle set in the filter with a newly generated one.
     */
    void ParticleFilter::resample()
    {
        // Normalize the particle weights, and find the average weight.
        float sum = 0.0f;
        ParticleIt iter;
        for(iter = particles.begin(); iter != particles.end(); ++iter)
            sum += (*iter).getWeight();

        if(sum == 0)
        {
            std::cout << "\n\n\nZERO SUM!\n\n\n" << std::endl;
            return;
        }

        averageWeight = sum/(((float)parameters.numParticles)*1.0f);

        for(iter = particles.begin(); iter != particles.end(); ++iter)
        {
            float weight = (*iter).getWeight();
            (*iter).setWeight(weight/sum);
        }

//         // Update exponential filters for long-term and short-term weights.
//         wSlow = wSlow + parameters.alpha_slow*(averageWeight - wSlow);
//         wFast = wFast + parameters.alpha_fast*(averageWeight - wFast);

//         float injectionProb = std::max(0.0f, 1.0f - wFast/wSlow);

//         float confidence = 1.0f - wFast/wSlow;
// //        std::cout << "The confidence of the particles swarm is: "
// //                  << confidence << std::endl;



        //if(injectionProb > 0)
        //    std::cout << injectionProb << std::endl;

        // Map each normalized weight to the corresponding particle.
        std::map<float, LocalizationParticle> cdf;

        float prev = 0.0f;
        for(iter = particles.begin(); iter != particles.end(); ++iter)
        {
            LocalizationParticle particle = (*iter);

            cdf[prev + particle.getWeight()] = particle;
            prev += particle.getWeight();
        }

        // std::cout << "sum = " << sum << std::endl;
        // std::cout << "tot = " << prev << std::endl;

        boost::mt19937 rng;
        rng.seed(static_cast<unsigned>(std::time(0)));
        boost::uniform_01<boost::mt19937> gen(rng);

        // For random particle injection.
        // boost::uniform_real<float> xBounds(0.0f, width);
        // boost::uniform_real<float> yBounds(0.0f, height);
        // boost::uniform_real<float> angleBounds(0,
        //                         2.0f*boost::math::constants::pi<float>());

        // boost::variate_generator<boost::mt19937&,
        //            boost::uniform_real<float> > xGen(rng, xBounds);
        // boost::variate_generator<boost::mt19937&,
        //               boost::uniform_real<float> > yGen(rng, yBounds);
        // boost::variate_generator<boost::mt19937&,
        //              boost::uniform_real<float> > angleGen(rng, angleBounds);

        float rand;
        ParticleSet newParticles;
        int numParticlesInjected = 0;
        // Sample numParticles particles with replacement according to the
        // normalized weights, and place them in a new particle set.
        for(int i = 0; i < parameters.numParticles; ++i)
        {
            rand = (float)gen();

//            if(rand <= injectionProb)
//                numParticlesInjected++;
            //{
            //LocalizationParticle p(Location(xGen(), yGen(), angleGen()),
            //                       0.0f);
            //newParticles.push_back(p);
            //}
            //else
            newParticles.push_back(cdf.upper_bound(rand)->second);
        }

        particles = newParticles;

        if(numParticlesInjected > 0)
        {
            //std::cout << "Injected " << numParticlesInjected
            //                         << " random particles."
            //        << std::endl;
        }

#ifdef DEBUG_LOCALIZATION
        LocalizationParticle best = getBestParticle();
        std::cout << "Best particle: " << best << std::endl;

        std::cout << "Resampled particles: " << std::endl;
        for(iter = particles.begin(); iter != particles.end(); ++iter)
        {
            std::cout << "Particle (" << (*iter).getLocation().x << ", "
                      << (*iter).getLocation().y << ", "
                      << (*iter).getLocation().heading
                      << ") with weight " << (*iter).getWeight() << std::endl;
        }
#endif

    }

    ::MotionModel ParticleFilter::getLastOdo() const { return motionModel->getLastOdometry(); }

    /**
     * Clears the existing particle set and generates a new,
     * uniform distribution of particles bounded by the
     * dimensions of the field.
     */
    void ParticleFilter::reset()
    {
        // Clear the existing particles.
        particles.clear();

        boost::mt19937 rng;
        rng.seed(std::time(0));

        boost::uniform_real<float> xBounds(0.0f,
                                      (float) parameters.fieldWidth);
        boost::uniform_real<float> yBounds(0.0f,
                                      (float) parameters.fieldHeight);
        boost::uniform_real<float> angleBounds(0,
                                      2.0f*boost::math::constants::pi<float>());

        boost::variate_generator<boost::mt19937&,
                                boost::uniform_real<float> > xGen(rng, xBounds);
        boost::variate_generator<boost::mt19937&,
                                boost::uniform_real<float> > yGen(rng, yBounds);
        boost::variate_generator<boost::mt19937&,
                        boost::uniform_real<float> > angleGen(rng, angleBounds);

        // Assign uniform weight.
        float weight = 1.0f/(((float)parameters.numParticles)*1.0f);

        for(int i = 0; i < parameters.numParticles; ++i)
        {
            LocalizationParticle p(Location(xGen(), yGen(), angleGen()),
                                   weight);

            particles.push_back(p);
        }
    }

    template <class T>
    T square(T x) {
        return x*x;
    }

    /**
     * Resets localization to the given x, y, and heading.
     *
     * @param x      The x-coordinate.
     * @param y      The y-coordinate.
     * @param h      The heading (radians).
     * @param params The parameters specifying how the particles will
     *               be sampled about the mean (x, y, h).
     */
    void ParticleFilter::resetLocTo(float x, float y, float h,
				    LocNormalParams params)
    {
        // Reset the estimates.
        xEstimate = x;
        yEstimate = y;
        hEstimate = h;

        particles.clear();

	float weight = 1.0f/parameters.numParticles;

        for(int i = 0; i < parameters.numParticles; ++i)
        {
            // Get the new particles x,y, and h
            float pX = sampleNormal(x, params.sigma_x);
            float pY = sampleNormal(y, params.sigma_x);
            float pH = sampleNormal(h, params.sigma_h);

            LocalizationParticle p(Location(pX, pY, pH), weight);

            particles.push_back(p);
        }
    }

    /**
     * Overloaded resetLocTo, resets Loc to 2 possible Locations
     *
     * @param x       The first Locations x-coordinate.
     * @param y       The first Locations y-coordinate.
     * @param h       The first Locations heading (radians).
     * @param x_      The second Locations x-coordinate.
     * @param y_      The second Locations y-coordinate.
     * @param h_      The second Locations heading (radians).
     * @param params1 The parameters specifying how the particles will
     *                be normally sampled about the mean (x, y, h).
     * @param params2 The parameters specifying how the particles will
     *                be normally sampled about the mean (x_, y_, h_).
     */
    void ParticleFilter::resetLocTo(float x, float y, float h,
                                    float x_, float y_, float h_,
				    LocNormalParams params1,
				    LocNormalParams params2)
    {
       // Reset the estimates. Choose the first location for convention
       xEstimate = x;
       yEstimate = y;
       hEstimate = h;

       particles.clear();

       float weight = 1.0f/parameters.numParticles;

       for(int i = 0; i < (parameters.numParticles / 2); ++i)
       {
           // Get the new particles x,y, and h
           float pX = sampleNormal(x, params1.sigma_x);
           float pY = sampleNormal(y, params1.sigma_y);
           float pH = sampleNormal(h, params1.sigma_h);

           LocalizationParticle p(Location(pX, pY, pH), weight);

           particles.push_back(p);
       }

       for(int i = 0; i < ((parameters.numParticles + 1) / 2); ++i)
       {
           // Get the new particles x,y, and h
           float pX = sampleNormal(x_, params2.sigma_x);
           float pY = sampleNormal(y_, params2.sigma_y);
           float pH = sampleNormal(h_, params2.sigma_h);

	   LocalizationParticle p(Location(pX, pY, pH), weight);

           particles.push_back(p);
       }
    }

    void ParticleFilter::updateMemory(MLocalization::ptr mLoc) const
    {
        using namespace man::memory::proto;

        mLoc->get()->mutable_location()->set_x(this->getXEst());
        mLoc->get()->mutable_location()->set_y(this->getYEst());
        mLoc->get()->mutable_location()->set_h(this->getHEst());

        mLoc->get()->clear_particles();

        // Get the particles, and update the protobuf accordingly.
        PF::ParticleSet particles = this->getParticles();

        PLoc::Particle *particle;
        for(PF::ParticleIt iter = particles.begin(); iter != particles.end(); ++iter)
        {
            particle = mLoc->get()->add_particles();
            particle->set_x((*iter).getLocation().x);
            particle->set_y((*iter).getLocation().y);
            particle->set_h((*iter).getLocation().heading);
            particle->set_w((*iter).getWeight());
        }
    }

    /**
     * Calculates the standard deviation of the particle cluster.
     *
     * @return sd A vector whose indices 0, 1, 2 correspond to the
     *            standard deviations of the x, y, and heading
     *            standard deviations respectively.
     */
    std::vector<float> ParticleFilter::findParticleSD() const
    {
	PF::ParticleSet particles = this->getParticles();

	std::vector<float> sd(3, 0.0f);
	float mean_x = 0.0f, mean_y = 0.0f, mean_h = 0.0f;
	ParticleIt iter = particles.begin();
	for(; iter != particles.end(); ++iter)
	{
	    mean_x += (*iter).getLocation().x;
	    mean_y += (*iter).getLocation().y;
	    mean_h += (*iter).getLocation().heading;
	}

	mean_x /= parameters.numParticles;
	mean_y /= parameters.numParticles;
	mean_h /= parameters.numParticles;

	// Calculate the standard deviation:
	// \sigma_x = \sqrt{\frac{1}{N}\sum_{i=0}^{N-1}(x_i - \bar{x})^2}
	// where x_i stands for either the x, y, or heading of 
	// the ith particle.
	for(iter = particles.begin(); iter != particles.end(); ++iter)
	{
	    sd[0] += square((*iter).getLocation().x       - mean_x);
	    sd[1] += square((*iter).getLocation().y       - mean_y);
	    sd[2] += square((*iter).getLocation().heading - mean_h); 
	}
	sd[0] /= (1/parameters.numParticles);
	sd[1] /= (1/parameters.numParticles);
	sd[2] /= (1/parameters.numParticles);

	// Convert variances into standard deviations.

	sd[0] = std::sqrt(sd[0]);
	sd[1] = std::sqrt(sd[1]);
	sd[2] = std::sqrt(sd[2]);

	return sd;
    }
}


