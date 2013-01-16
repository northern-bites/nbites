#include "ParticleFilter.h"

namespace man
{
    namespace localization
    {
	ParticleFilter::ParticleFilter(boost::shared_ptr<MotionModel> motionModel,
				       boost::shared_ptr<SensorModel> sensorModel,
				       ParticleFilterParams params)
	    : parameters(params), standardDeviations(3, 0.0f)
	{
	    motionModel_ = motionModel;
	    sensorModel_ = sensorModel;

	    boost::mt19937 rng;
	    rng.seed(std::time(0));

	    boost::uniform_real<float> xBounds(0.0f,
					       (float) parameters_.fieldWidth_);
	    boost::uniform_real<float> yBounds(0.0f,
					       (float) parameters_.fieldHeight_);
	    boost::uniform_real<float> angleBounds(0,
						   2.0f*boost::math::constants::pi<float>());

	    boost::variate_generator<boost::mt19937&,
				     boost::uniform_real<float> > xGen(rng, xBounds);
	    boost::variate_generator<boost::mt19937&,
				     boost::uniform_real<float> > yGen(rng, yBounds);
	    boost::variate_generator<boost::mt19937&,
				     boost::uniform_real<float> > angleGen(rng, angleBounds);

	    // Assign uniform weight.
	    float weight = 1.0f/(((float)parameters.numParticles_)*1.0f);

	    for(int i = 0; i < parameters.numParticles_; ++i)
	    {
		Particle p(man::memory::protos::RobotLocation(xGen(), yGen(), angleGen()),
				       weight);
		particles.push_back(p);
	    }
	}

	ParticleFilter::~ParticleFilter()
	{

	}

	void ParticleFilter::filter(bool motionUpdate, bool sensorUpdate)
	{
	    if(motionUpdate && motionModel)
	    {
		particles_ = motionModel_->update(particles_);
	    }

	    if(sensorUpdate && sensorModel)
	    {
		particles_ = sensorModel_->update(particles_);
	    }

	    // Only resample if the sensor model has updated particle weights.
	    if(sensorModel_->hasUpdated())
	    {
		resample();
	    }
	    else
	    {
		//std::cout << "No sensor update." << std::endl;
	    }

	    // Update estimates.
	    float sumX = 0;
	    float sumY = 0;
	    float sumH = 0;

	    ParticleIt iter;
	    for(iter = particles.begin(); iter != particles.end(); ++iter)
	    {
		man::memory::protos::RobotLocation l = (*iter).getLocation();
		sumX += l.getX();
		sumY += l.getY();
		sumH += l.getH();
	    }

	    float previousXEstimate = xEstimate;
	    float previousYEstimate = yEstimate;
	    float previousHEstimate = hEstimate;

	    xEstimate = sumX/parameters.numParticles;
	    yEstimate = sumY/parameters.numParticles;
	    hEstimate = sumH/parameters.numParticles;

	    standardDeviations = findParticleSD();

	    // Check if the mean has gone out of bounds. If so, 
	    // reset to the closest point in bounds with appropriate
	    // uncertainty.
	    bool resetInBounds = false;

	    if(xEstimate < 0)
	    {
		resetInBounds = true;
		xEstimate = 0;
	    }
	    else if(xEstimate > parameters_.fieldWidth_)
	    {
		resetInBounds = true;
		xEstimate = parameters_.fieldWidth_;
	    }

	    if(yEstimate < 0)
	    {
		resetInBounds = true;
		yEstimate = 0;
	    }
	    else if(yEstimate > parameters_.fieldHeight_)
	    {
		resetInBounds = true;
		yEstimate = parameters_.fieldHeight_;
	    }

	    // Only reset if one of the location coordinates is
	    // out of bounds; avoids unnecessary resets.
	    if(resetInBounds)
	    {
		std::cout << "Resetting to (" << xEstimate
			  << ", " << yEstimate << ", "
			  << hEstimate << ")." << std::endl;
	    }
	}

	Particle ParticleFilter::getBestParticle()
	{
	    // Sort the particles in ascending order.
	    std::sort(particles_.begin(), particles_.end());

	    // The last particle should have the greatest weight.
	    return particles_[particles_.size()-1];
	}

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

	    if(parameters.numParticles == 0)
	    {
		std::cout << "Invalid number of particles!" << std::endl;
		return sd;
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

	    sd[0] /= parameters.numParticles;
	    sd[1] /= parameters.numParticles;
	    sd[2] /= parameters.numParticles;

	    // Convert variances into standard deviations.
	    sd[0] = std::sqrt(sd[0]);
	    sd[1] = std::sqrt(sd[1]);
	    sd[2] = std::sqrt(sd[2]);

	    return sd;
	}

	void ParticleFilter::resetLocalization()
	{
	    // Clear the existing particles.
	    particles.clear();

	    boost::mt19937 rng;
	    rng.seed(std::time(0));

	    boost::uniform_real<float> xBounds(0.0f,
					       (float) parameters_.fieldWidth_);
	    boost::uniform_real<float> yBounds(0.0f,
					       (float) parameters_.fieldHeight_);
	    boost::uniform_real<float> angleBounds(0,
						   2.0f*boost::math::constants::pi<float>());

	    boost::variate_generator<boost::mt19937&,
				     boost::uniform_real<float> > xGen(rng, xBounds);
	    boost::variate_generator<boost::mt19937&,
				     boost::uniform_real<float> > yGen(rng, yBounds);
	    boost::variate_generator<boost::mt19937&,
				     boost::uniform_real<float> > angleGen(rng, angleBounds);

	    // Assign uniform weight.
	    float weight = 1.0f/(((float)parameters_.numParticles_)*1.0f);

	    for(int i = 0; i < parameters_.numParticles_; ++i)
	    {
		Particle p(Location(xGen(), yGen(), angleGen()),
				       weight);

		particles.push_back(p);
	    }
	}

	void ParticleFilter::resample()
	{
	    // Normalize the particle weights, and find the average weight.
	    float sum = 0.0f;
	    ParticleIt iter;
	    for(iter = particles_.begin(); iter != particles_.end(); ++iter)
		sum += (*iter).getWeight();

	    if(sum == 0)
	    {
		std::cout << "\n\n\nZERO SUM!\n\n\n" << std::endl;
		return;
	    }

	    averageWeight = sum/(((float)parameters_.numParticles_)*1.0f);

	    for(iter = particles_.begin(); iter != particles_.end(); ++iter)
	    {
		float weight = (*iter).getWeight();
		(*iter).setWeight(weight/sum);
	    }

	    // Map each normalized weight to the corresponding particle.
	    std::map<float, Particle> cdf;

	    float prev = 0.0f;
	    for(iter = particles_.begin(); iter != particles_.end(); ++iter)
	    {
		Particle particle = (*iter);

		cdf[prev + particle.getWeight()] = particle;
		prev += particle.getWeight();
	    }

	    boost::mt19937 rng;
	    rng.seed(static_cast<unsigned>(std::time(0)));
	    boost::uniform_01<boost::mt19937> gen(rng);

	    float rand;
	    ParticleSet newParticles;
	    // Sample numParticles particles with replacement according to the
	    // normalized weights, and place them in a new particle set.
	    for(int i = 0; i < parameters_.numParticles_; ++i)
	    {
		rand = (float)gen();
		newParticles.push_back(cdf.upper_bound(rand)->second);
	    }

	    particles_ = newParticles;
	}

	void ParticleFilter::updateLocalization(/* @todo */)
	{
	    // @todo
	}
    } // namespace localization
} // namespace man
