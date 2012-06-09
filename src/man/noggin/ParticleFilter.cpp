#include "ParticleFilter.h"

namespace PF
{
    /** 
     * Constructor
     */
    LocalizationParticle::LocalizationParticle(Location l, float w)
	: weight(w), location(l)
    { }

    LocalizationParticle::LocalizationParticle()
	: weight(0.0f), location()
    { }

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
	                           ParticleFilterParams params)
	: parameters(params), xEstimate(0.0f), yEstimate(0.0f), 
	  hEstimate(0.0f), averageWeight(0.0f), wFast(0.0f), 
	  wSlow(0.0f)
    {
	motionModel = motion;
	sensorModel = sensor;

	boost::mt19937 rng;
	rng.seed(std::time(0));

        boost::uniform_real<float> xBounds(0.0f, parameters.fieldWidth);
	boost::uniform_real<float> yBounds(0.0f, parameters.fieldHeight);
	boost::uniform_real<float> angleBounds(0, 2.0f*boost::math::constants::pi<float>());

	boost::variate_generator<boost::mt19937&, 
				 boost::uniform_real<float> > xGen(rng, xBounds);
	boost::variate_generator<boost::mt19937&,
				 boost::uniform_real<float> > yGen(rng, yBounds);
	boost::variate_generator<boost::mt19937&,
				 boost::uniform_real<float> > angleGen(rng, angleBounds);

	// Assign uniform weight.
	float weight = 1.0f/(parameters.numParticles*1.0f);

#ifdef DEBUG_LOCALIZATION
	std::cout << "Weight = " << weight << "." << std::endl;
#endif

	for(int i = 0; i < parameters.numParticles; ++i)
	{
	    LocalizationParticle p(Location(xGen(), yGen(), angleGen()), weight);

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
	Location estimate = this->getBestParticle().getLocation();

	xEstimate = estimate.x;
	yEstimate = estimate.y;
	hEstimate = estimate.heading;
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
	    //std::cout << "\n\n\nZERO SUM!\n\n\n" << std::endl;
	    reset();
	    return;
	}

	averageWeight = sum/(parameters.numParticles*1.0f);

	for(iter = particles.begin(); iter != particles.end(); ++iter)
	{
	    float weight = (*iter).getWeight();
	    (*iter).setWeight(weight/sum);
	}

	 // Update exponential filters for long-term and short-term weights.
	 wSlow = wSlow + parameters.alpha_slow*(averageWeight - wSlow);
	 wFast = wFast + parameters.alpha_fast*(averageWeight - wFast);

	 float injectionProb = std::max(0.0f, 1.0f - wFast/wSlow);
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
	// boost::uniform_real<float> angleBounds(0, 2.0f*boost::math::constants::pi<float>());

	// boost::variate_generator<boost::mt19937&, 
	// 			 boost::uniform_real<float> > xGen(rng, xBounds);
	// boost::variate_generator<boost::mt19937&,
	// 			 boost::uniform_real<float> > yGen(rng, yBounds);
	// boost::variate_generator<boost::mt19937&,
	// 			 boost::uniform_real<float> > angleGen(rng, angleBounds);

	float rand;
	ParticleSet newParticles;
	int numParticlesInjected = 0;
	// Sample numParticles particles with replacement according to the
	// normalized weights, and place them in a new particle set.
	for(int i = 0; i < parameters.numParticles; ++i)
	{
	    rand = (float)gen();

	    if(rand <= injectionProb)
		numParticlesInjected++;
		//{
		//LocalizationParticle p(Location(xGen(), yGen(), angleGen()), 0.0f);
		//newParticles.push_back(p);
		//}
	    //else
		newParticles.push_back(cdf.upper_bound(rand)->second);
	}

	particles = newParticles;

	if(numParticlesInjected > 0)
	{
	    //std::cout << "Injected " << numParticlesInjected << " random particles." 
	    //	      << std::endl;
	}

#ifdef DEBUG_LOCALIZATION
	LocalizationParticle best = getBestParticle();
	std::cout << "Best particle: " << best << std::endl;

	std::cout << "Resampled particles: " << std::endl;
	for(iter = particles.begin(); iter != particles.end(); ++iter)
	{
	    std::cout << "Particle (" << (*iter).getLocation().x << ", "
		      << (*iter).getLocation().y << ", " << (*iter).getLocation().heading
		      << ") with weight " << (*iter).getWeight() << std::endl;
	}
#endif

    }

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

        boost::uniform_real<float> xBounds(0.0f, parameters.fieldWidth);
	boost::uniform_real<float> yBounds(0.0f, parameters.fieldHeight);
	boost::uniform_real<float> angleBounds(0, 2.0f*boost::math::constants::pi<float>());

	boost::variate_generator<boost::mt19937&, 
				 boost::uniform_real<float> > xGen(rng, xBounds);
	boost::variate_generator<boost::mt19937&,
				 boost::uniform_real<float> > yGen(rng, yBounds);
	boost::variate_generator<boost::mt19937&,
				 boost::uniform_real<float> > angleGen(rng, angleBounds);

	// Assign uniform weight.
	float weight = 1.0f/(parameters.numParticles*1.0f);

	for(int i = 0; i < parameters.numParticles; ++i)
	{
	    LocalizationParticle p(Location(xGen(), yGen(), angleGen()), weight);

	    particles.push_back(p);
	}
    }

    /**
     * Resets localization to the given x, y, and heading.
     *
     * @param x the x-coordinate.
     * @param y the y-coordinate.
     * @param h the heading (radians).
     */
    void ParticleFilter::resetLocTo(float x, float y, float h)
    {
	const float SIGMA_RESET_X = 15.0f;
	const float SIGMA_RESET_Y = 15.0f;
	const float SIGMA_RESET_H = 0.2f;

	// Reset the estimates. 
	xEstimate = x;
	yEstimate = y;
	hEstimate = h;

	particles.clear();

	// @todo assign weights properly
	float weight = 1.0f/(parameters.numParticles*1.0f);

	for(int i = 0; i < parameters.numParticles; ++i)
	{
	    LocalizationParticle p(Location(sampleNormal(x, SIGMA_RESET_X),
				            sampleNormal(y, SIGMA_RESET_Y),
				            sampleNormal(h, SIGMA_RESET_H)), weight);

	    particles.push_back(p);
	}
    }
}
