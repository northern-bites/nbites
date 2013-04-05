#include "ParticleFilter.h"

namespace man
{
    namespace localization
    {
    ParticleFilter::ParticleFilter(ParticleFilterParams params)
        : parameters(params),
          estimateUncertainty(3, 0.0f)
    {
        motionSystem = new MotionSystem(params.odometryXYNoise,
                                        params.odometryHNoise);
        visionSystem = new VisionSystem;

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
            messages::RobotLocation randomLocation;
            randomLocation.set_x(xGen());
            randomLocation.set_y(yGen());
            randomLocation.set_h(angleGen());
            Particle p(randomLocation, weight);
            particles.push_back(p);
        }
    }

    ParticleFilter::~ParticleFilter()
    {
        delete motionSystem;
        delete visionSystem;
    }

    void ParticleFilter::update(messages::RobotLocation& odometryInput,
                                messages::VisionField& visionInput)
    {
        lastMotionTimestamp = (float) motionInput.timestamp();
        motionSystem->update(particles, odometryInput);

        // Update the Vision Model
        // set updated vision to determine if resampling necessary
        updatedVision = visionSystem->update(particles, visionInput);

        // Resample if vision update
        if(updatedVision)
        {
            resample();
            updatedVision = false;
        }

        // Update filters estimate
        updateEstimate();

        // std::cout<<"Time:\t" << motionInput.timestamp() <<"\n";
        // std::cout<<"(x,y,h)\t("<< getXEst() << " , " << getYEst()
        //          <<" , " <<getHEst()<< " )\n\n";

        /**
         * The following is to stop particles going off field
         * Commented out until known if necessary (K.I.S.S.)
         */
        // // Check if the mean has gone out of bounds. If so,
        // // reset to the closest point in bounds with appropriate
        // // uncertainty.
        // bool resetInBounds = false;

        // if(poseEstimate.x() < 0)
        // {
        //     resetInBounds = true;
        //     poseEstimate.set_x(0);
        // }

        // else if(poseEstimate.x() > parameters.fieldWidth)
        // {
        //     resetInBounds = true;
        //     poseEstimate.set_x(parameters.fieldWidth);
        // }

        // if(poseEstimate.y() < 0)
        // {
        //     resetInBounds = true;
        //     poseEstimate.set_y(0);
        // }
        // else if(poseEstimate.y() > parameters.fieldHeight)
        // {
        //     resetInBounds = true;
        //     poseEstimate.set_y(parameters.fieldHeight);
        // }

        // // Only reset if one of the location coordinates is
        // // out of bounds; avoids unnecessary resets.
        // if(resetInBounds)
        // {
        //     /*
        //      * @TODO Actually reset to a location here
        //      */

        //     std::cout << "Resetting to (" << poseEstimate.x()
        //               << ", " << poseEstimate.y() << ", "
        //               << poseEstimate.h() << ")." << std::endl;
        // }

    }

    /**
     *@brief  Updates the filters estimate of the robots position
     */
    void ParticleFilter::updateEstimate()
    {
        float sumX = 0;
        float sumY = 0;
        float sumH = 0;

        ParticleIt iter;
        for(iter = particles.begin(); iter != particles.end(); ++iter)
        {
            sumX += (*iter).getLocation().x();
            sumY += (*iter).getLocation().y();
            sumH += (*iter).getLocation().h();
        }

        float previousXEstimate = poseEstimate.x();
        float previousYEstimate = poseEstimate.y();
        float previousHEstimate = poseEstimate.h();

        poseEstimate.set_x(sumX/parameters.numParticles);
        poseEstimate.set_y(sumY/parameters.numParticles);
        poseEstimate.set_h(sumH/parameters.numParticles);

        estimateUncertainty = findParticleSD();
    }

    /**
     * @brief  Return the particle which best predicted
     *         the most recent observations
     */
    Particle ParticleFilter::getBestParticle()
    {
        // Sort the particles in ascending order.
        std::sort(particles.begin(), particles.end());

        // The last particle should have the greatest weight.
        return particles[particles.size()-1];
    }

    /**
     * @brief  Calculate the standard deviation of the particle swarm
     *         NOTE: Best measure of a particle filters uncertainty
     */
    std::vector<float> ParticleFilter::findParticleSD()
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
            sd[0] += NBMath::square((*iter).getLocation().x() - mean_x);
            sd[1] += NBMath::square((*iter).getLocation().y() - mean_y);
            sd[2] += NBMath::square((*iter).getLocation().h() - mean_h);
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

    /*
     * @brief The following are all of the resetLoc functions
     */
    void ParticleFilter::resetLoc()
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
            messages::RobotLocation randomLocation;
            randomLocation.set_x(xGen());
            randomLocation.set_y(yGen());
            randomLocation.set_h(angleGen());
            Particle p(randomLocation, weight);

            particles.push_back(p);
        }
    }

    void ParticleFilter::resetLocTo(float x, float y, float h,
                                    LocNormalParams params)
    {
        // Reset the estimate
        poseEstimate.set_x(x);
        poseEstimate.set_y(y);
        poseEstimate.set_h(h);

        particles.clear();

        float weight = 1.0f/parameters.numParticles;

        for(int i = 0; i < parameters.numParticles; ++i)
        {
            // Get the new particles x,y, and h
            float pX = sampleNormal(x, params.sigma_x);
            float pY = sampleNormal(y, params.sigma_x);
            float pH = sampleNormal(h, params.sigma_h);

            Particle p(pX, pY, pH, weight);

            particles.push_back(p);
        }

    }

    /**
     * Overloaded resetLocTo, resets Loc to 2 possible Locations
     *
     * @param x The first Locations x-coordinate.
     * @param y The first Locations y-coordinate.
     * @param h The first Locations heading (radians).
     * @param x_ The second Locations x-coordinate.
     * @param y_ The second Locations y-coordinate.
     * @param h_ The second Locations heading (radians).
     * @param params1 The parameters specifying how the particles will
     * be normally sampled about the mean (x, y, h).
     * @param params2 The parameters specifying how the particles will
     * be normally sampled about the mean (x_, y_, h_).
     */
    void ParticleFilter::resetLocTo(float x, float y, float h,
                                    float x_, float y_, float h_,
                                    LocNormalParams params1,
                                    LocNormalParams params2)
    {
        // Reset the estimates.
        poseEstimate.set_x(x);
        poseEstimate.set_y(y);
        poseEstimate.set_h(h);

       particles.clear();
       float weight = 1.0f/parameters.numParticles;

       for(int i = 0; i < (parameters.numParticles / 2); ++i)
       {
           // Get the new particles x,y, and h
           float pX = sampleNormal(x, params1.sigma_x);
           float pY = sampleNormal(y, params1.sigma_y);
           float pH = sampleNormal(h, params1.sigma_h);

           Particle p(pX, pY, pH, weight);

           particles.push_back(p);
       }

       for(int i = 0; i < ((parameters.numParticles + 1) / 2); ++i)
       {
           // Get the new particles x,y, and h
           float pX = sampleNormal(x_, params2.sigma_x);
           float pY = sampleNormal(y_, params2.sigma_y);
           float pH = sampleNormal(h_, params2.sigma_h);

           Particle p(pX, pY, pH, weight);

           particles.push_back(p);
       }
    }


    void ParticleFilter::resetLocToSide(bool blueSide)
    {
        // Clear the existing particles.
        particles.clear();

        float xLowerBound = 0.0f, xUpperBound = 0.0f;
        // HACK replace constants!
        float yLowerBound = FIELD_WHITE_BOTTOM_SIDELINE_Y, yUpperBound = FIELD_WHITE_TOP_SIDELINE_Y;
        float heading = 0.0f;

        boost::mt19937 rng;
        rng.seed(std::time(0));

        if(blueSide)
        {
            xLowerBound = FIELD_WHITE_LEFT_SIDELINE_X;
            xUpperBound = MIDFIELD_X;
        }
        else
        {
            xLowerBound = MIDFIELD_X;
            xUpperBound = FIELD_WHITE_RIGHT_SIDELINE_X;
            heading = M_PI_FLOAT;
        }

        boost::uniform_real<float> xBounds(xLowerBound,
                                           xUpperBound);
        boost::uniform_real<float> yBounds(yLowerBound,
                                           yUpperBound);

        boost::variate_generator<boost::mt19937&,
                                boost::uniform_real<float> > xGen(rng, xBounds);
        boost::variate_generator<boost::mt19937&,
                                boost::uniform_real<float> > yGen(rng, yBounds);

        // Assign uniform weight.
        float weight = 1.0f/(((float)parameters.numParticles)*1.0f);

        for(int i = 0; i < parameters.numParticles; ++i)
        {
            Particle p(xGen(), yGen(), heading, weight);

            particles.push_back(p);
        }
    }

    /**
     * @brief  Resample the particles based on vision observations
     *         NOTE: Assume given a swarm with normalized weights
     */
    void ParticleFilter::resample()
    {
        // Map each normalized weight to the corresponding particle.
        std::map<float, Particle> cdf;

        float prev = 0.0f;
        ParticleIt iter;
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

        // FUN IDEA: Create a particle that equals the belief

        // ***TEMP*** This is used for testing, to only select the BEST particle
        // newParticles.clear();
        // Particle best;
        // float bestWeight =0.f;
        // for(iter = particles.begin(); iter != particles.end(); ++iter)
        // {
        //     Particle particle = (*iter);

        //     if(particle.getWeight() > bestWeight)
        //     {
        //         best = particle;
        //         bestWeight = particle.getWeight();
        //     }
        // }
        // for (int i=0; i<parameters.numParticles; i++)
        //     newParticles.push_back(best);
        // ***TEMP*** always choose best particle

        particles = newParticles;
    }
    } // namespace localization
} // namespace man
