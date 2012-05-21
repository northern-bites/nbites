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

    ParticleFilter::ParticleFilter(int particles, float w, float h,
				   boost::shared_ptr<MotionModel> motion, 
				   boost::shared_ptr<SensorModel> sensor)
	: numParticles(particles), width(w), height(h), xEstimate(0.0f),
	  yEstimate(0.0f), hEstimate(0.0f), averageWeight(0.0f),
	  wFast(0.0f), wSlow(0.0f)
    {
	motionModel = motion;
	sensorModel = sensor;

	boost::mt19937 rng;
	rng.seed(std::time(0));

        boost::uniform_real<float> xBounds(0.0f, width);
	boost::uniform_real<float> yBounds(0.0f, height);
	boost::uniform_real<float> angleBounds(0, 2.0f*boost::math::constants::pi<float>());

	boost::variate_generator<boost::mt19937&, 
				 boost::uniform_real<float> > xGen(rng, xBounds);
	boost::variate_generator<boost::mt19937&,
				 boost::uniform_real<float> > yGen(rng, yBounds);
	boost::variate_generator<boost::mt19937&,
				 boost::uniform_real<float> > angleGen(rng, angleBounds);

	// Assign uniform weight.
	float weight = 1.0f/(particles*1.0f);
#ifdef DEBUG_LOCALIZATION
	std::cout << "Weight = " << weight << "." << std::endl;
#endif

	for(int i = 0; i < particles; ++i)
	{
	    LocalizationParticle p(Location(xGen(), yGen(), angleGen()), weight);

	    this->particles.push_back(p);

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

	if(sensorModel->hasUpdated())
	{
	    resample();
	    Location estimate = this->getBestParticle().getLocation();
	    // if(xEstimate - estimate.x > 0.001f || 
	    //    yEstimate - estimate.y > 0.001f ||
	    //    hEstimate - estimate.heading > 0.001f)
	    // {
	    //   // std::cout << "New guess at (" <<
	    //   // 	estimate.x << ", " <<
	    //   // 	estimate.y << ", " <<
	    //   // 	estimate.heading << ")" << std::endl;
	    // }

	    xEstimate = estimate.x;
	    yEstimate = estimate.y;
	    hEstimate = estimate.heading;
	    //std::cout << "Resample complete." << std::endl;
	}
	else
	{
	    std::cout << "No sensor update." << std::endl;
	}
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

	averageWeight = sum/(numParticles*1.0f);

	for(iter = particles.begin(); iter != particles.end(); ++iter)
	{
	    float weight = (*iter).getWeight();
	    (*iter).setWeight(weight/sum);
	}

	// @todo add these as parameters
	const float ALPHA_SLOW = 0.05f;
	const float ALPHA_FAST = 0.2f;

	// Update exponential filters for long-term and short-term weights.
	wSlow = wSlow + ALPHA_SLOW*(averageWeight - wSlow);
	wFast = wFast + ALPHA_FAST*(averageWeight - wFast);

	float injectionProb = std::max(0.0f, 1.0f - wFast/wSlow);
	if(injectionProb > 0)
	    std::cout << injectionProb << std::endl;

	// Map each normalized weight to the corresponding particle.
	std::map<float, LocalizationParticle> cdf;

	float prev = 0.0f;
	for(iter = particles.begin(); iter != particles.end(); ++iter)
	{
	    LocalizationParticle particle = (*iter);
	    cdf[prev + particle.getWeight()] = particle;
	    prev += particle.getWeight();
	}

	boost::mt19937 rng;
	rng.seed(static_cast<unsigned>(std::time(0)));
	boost::uniform_01<boost::mt19937> gen(rng);
	
	float rand;
	ParticleSet newParticles;
	int numParticlesInjected = 0;
	// Sample numParticles particles with replacement according to the
	// normalized weights, and place them in a new particle set.
	for(int i = 0; i < numParticles; ++i)
	{
	    rand = (float)gen();

	    if(rand <= injectionProb)
		numParticlesInjected++;

	    newParticles.push_back(cdf.upper_bound(rand)->second);
	}

	particles = newParticles;

	if(numParticlesInjected > 0)
	{
	    std::cout << "Injected " << numParticlesInjected << " random particles." 
		      << std::endl;
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

    void ParticleFilter::reset()
    {
	// @todo
    }

    void ParticleFilter::resetLocTo(float x, float y, float h)
    {
	// @todo
    }
}
