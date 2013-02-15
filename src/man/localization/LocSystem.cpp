#include "LocSystem.h"

namespace man
{
    namespace localization
    {
    LocSystem::LocSystem(MotionModel motionModel_,
                         SensorModel sensorModel_) : onOpposingSide(false),
                                                     motionModel(motionModel_),
                                                     sensorModel(sensorModel_) {}
    LocSystem::~LocSystem(){}

    void LocSystem::updateLocalization(memory::proto::PMotion motionInput,
                                       memory::proto::PVision visionInput)
    {
        
    }









    Particle ParticleFilter::getBestParticle()
    {
        // Sort the particles in ascending order.
        std::sort(particles.begin(), particles.end());

        // The last particle should have the greatest weight.
        return particles[particles.size()-1];
    }

    template <class T>
    T square(T x) {
        return x*x;
    }

    std::vector<float> ParticleFilter::findParticleSD() const
    {
        man::localization::ParticleSet particles = this->getParticles();

        std::vector<float> sd(3, 0.0f);
        float mean_x = 0.0f, mean_y = 0.0f, mean_h = 0.0f;
        man::localization::ParticleIt iter;
        for(iter = particles.begin(); iter != particles.end(); ++iter)
        {
            mean_x += (*iter).getLocation().x();
            mean_y += (*iter).getLocation().y();
            mean_h += (*iter).getLocation().h();
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
            sd[0] += square((*iter).getLocation().x() - mean_x);
            sd[1] += square((*iter).getLocation().y() - mean_y);
            sd[2] += square((*iter).getLocation().h() - mean_h);
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
            memory::proto::RobotLocation randomLocation;
            randomLocation.set_x(xGen());
            randomLocation.set_y(yGen());
            randomLocation.set_h(angleGen());
            Particle p(randomLocation, weight);

            particles.push_back(p);
        }
    }

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

        //averageWeight = sum/(((float)parameters.numParticles)*1.0f);

        for(iter = particles.begin(); iter != particles.end(); ++iter)
        {
            float weight = (*iter).getWeight();
            (*iter).setWeight(weight/sum);
        }

        // Map each normalized weight to the corresponding particle.
        std::map<float, Particle> cdf;

        float prev = 0.0f;
        for(iter = particles.begin(); iter != particles.end(); ++iter)
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
        for(int i = 0; i < parameters.numParticles; ++i)
        {
        rand = (float)gen();
        newParticles.push_back(cdf.upper_bound(rand)->second);
        }

        particles = newParticles;
    }

    void ParticleFilter::updateLocalization(/* @todo */)
    {
        // @todo
    }

    void ParticleFilter::updateMotionModel()
    {
        // @todo
    }

    void ParticleFilter::updateVisionModel()
    {
        // @todo
    }

    } // namespace localization
} // namespace man
