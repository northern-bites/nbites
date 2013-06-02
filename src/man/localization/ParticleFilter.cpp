#include "ParticleFilter.h"

namespace man {
namespace localization {

ParticleFilter::ParticleFilter(ParticleFilterParams params)
    : parameters(params)
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

    lost = false;
}

ParticleFilter::~ParticleFilter()
{
    delete motionSystem;
    delete visionSystem;
}

void ParticleFilter::update(const messages::RobotLocation& odometryInput,
                            const messages::VisionField& visionInput)
{
    motionSystem->update(particles, odometryInput);

    // Update the Vision Model
    // set updated vision to determine if resampling necessary
    updatedVision = visionSystem->update(particles, visionInput);

    // Resample if vision update
    if(updatedVision)
    {
        resample();
        updatedVision = false;

        //If shitty swarm according to vision, expand search
        lost = (visionSystem->getLowestError() > LOST_THRESHOLD);
        errorMagnitude = visionSystem->getLowestError()*ALPHA
                             + errorMagnitude*(1-ALPHA);
        if (errorMagnitude > 300)
            errorMagnitude = 300;
        // if (lost)
        //     std::cout << "LOST! In the moment" << std::endl;

        // if (errorMagnitude > LOST_THRESHOLD)
        //     std::cout << "LOST FOR A FUCKING WHILLLEEEEE" << std::endl;

        // std::cout << "Not weighted avg error:\t" << visionSystem->getAvgError() << std::endl;
        // std::cout << "weighted avg error:\t" << visionSystem->getWeightedAvgError() << std::endl;
        // std::cout << "Best Particle error:\t" << visionSystem->getLowestError() << std::endl << std::endl;

    }

    // Update filters estimate
    updateEstimate();
}

/**
 *@brief  Updates the filters estimate of the robots position
 *        by averaging all particles
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
    poseEstimate.set_h(NBMath::subPIAngle(sumH/parameters.numParticles));

    poseEstimate.set_uncert(errorMagnitude);
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

float ParticleFilter::getMagnitudeError()
{
    // Idea: no need to think about different SD's for x, y, h since
    //       that's not how we evaluate particles
    //       Instead return the filtered value of avg error per obsv

    // Think of as a circle with radius errorMagnitude that grows and
    // shrinks as the filter gets better and worse observations
    return errorMagnitude;
}

// /**
//  * @brief  Calculate the standard deviation of the particle swarm
//  *         NOTE: Best measure of a particle filters uncertainty
//  */
//  std::vector<float> ParticleFilter::findParticleSD()
//  {
//      man::localization::ParticleSet particles = this->getParticles();

//      std::vector<float> sd(3, 0.0f);
//      float mean_x = 0.0f, mean_y = 0.0f, mean_h = 0.0f;
//      man::localization::ParticleIt iter;
//      for(iter = particles.begin(); iter != particles.end(); ++iter)
//      {
//          mean_x += (*iter).getLocation().x();
//          mean_y += (*iter).getLocation().y();
//          mean_h += (*iter).getLocation().h();
//      }

//      if(parameters.numParticles == 0)
//      {
//          std::cout << "Invalid number of particles!" << std::endl;
//          return sd;
//      }

//      mean_x /= parameters.numParticles;
//      mean_y /= parameters.numParticles;
//      mean_h /= parameters.numParticles;

//      // Calculate the standard deviation:
//      // \sigma_x = \sqrt{\frac{1}{N}\sum_{i=0}^{N-1}(x_i - \bar{x})^2}
//      // where x_i stands for either the x, y, or heading of
//      // the ith particle.
//      for(iter = particles.begin(); iter != particles.end(); ++iter)
//      {
//          sd[0] += NBMath::square((*iter).getLocation().x() - mean_x);
//          sd[1] += NBMath::square((*iter).getLocation().y() - mean_y);
//          sd[2] += NBMath::square((*iter).getLocation().h() - mean_h);
//      }

//      sd[0] /= parameters.numParticles;
//      sd[1] /= parameters.numParticles;
//      sd[2] /= parameters.numParticles;

//      // Convert variances into standard deviations.
//      sd[0] = std::sqrt(sd[0]);
//      sd[1] = std::sqrt(sd[1]);
//      sd[2] = std::sqrt(sd[2]);

//      return sd;
//  }

/*
 * @brief The following are all of the resetLoc functions
 */
void ParticleFilter::resetLoc()
{
    std::cout << "WTF: LOC IS RESETTING! 1" << std::endl;

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
    std::cout << "WTF: LOC IS RESETTING! 2" << std::endl;
    // HACK HACK HACK - If told to reset to negative x,y,h then flip
    if (x<0 && y<0 && h<0)
    {
        flipLoc();
        return;
    }


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
    std::cout << "WTF: LOC IS RESETTING! 3" << std::endl;
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
    std::cout << "WTF: LOC IS RESETTING! 1" << std::endl;
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
    //std::cout << "In resample" << std::endl;
    // Map each normalized weight to the corresponding particle.
    std::map<float, Particle> cdf;

    float prev = 0.0f;
    ParticleIt iter;
    for(iter = particles.begin(); iter != particles.end(); ++iter)
    {
        cdf[prev + iter->getWeight()] = (*iter);
        prev += iter->getWeight();
    }

    boost::mt19937 rng;
    rng.seed(static_cast<unsigned>(std::time(0)));
    boost::uniform_01<boost::mt19937> gen(rng);

    float rand;
    ParticleSet newParticles;

    // First add reconstructed particles from corner observations
    int numReconParticlesAdded = 0;
    if (lost && (errorMagnitude > LOST_THRESHOLD)
        && visionSystem->getLastNumObsv() > 1 && !nearMidField())
    {
        std::list<ReconstructedLocation> reconLocs = visionSystem->getReconstructedLocations();
        std::list<ReconstructedLocation>::const_iterator recLocIt;
        for (recLocIt = reconLocs.begin();
             recLocIt != reconLocs.end();
             recLocIt ++)
        {
            if ((*recLocIt).defSide == onDefendingSide())
            {
                Particle reconstructedParticle((*recLocIt).x,
                                               (*recLocIt).y,
                                               (*recLocIt).h,
                                               1.f/250.f);

                newParticles.push_back(reconstructedParticle);
            }
            numReconParticlesAdded++;
        }

        std::cout << "Injected " << numReconParticlesAdded << " particles" << std::endl;
    }

    // Sample numParticles particles with replacement according to the
    // normalized weights, and place them in a new particle set.
    for(int i = 0; i < (parameters.numParticles - (float)numReconParticlesAdded); ++i)
    {
        rand = (float)gen();
        newParticles.push_back(cdf.upper_bound(rand)->second);
    }

    particles = newParticles;
}

const messages::ParticleSwarm& ParticleFilter::getCurrentSwarm()
{
    // Clear the repeated particle field
    swarm.clear_particle();

    messages::Particle newParticle;
    ParticleIt iter;
    for(iter = particles.begin(); iter != particles.end(); ++iter)
    {
        newParticle.mutable_loc()->CopyFrom((*iter).getLocation());
        newParticle.set_weight((*iter).getWeight());
        swarm.add_particle()->CopyFrom(newParticle);
    }

    return swarm;
}

messages::RobotLocation ParticleFilter::getMirrorLocation(messages::RobotLocation loc)
{
    float newX = FIELD_GREEN_WIDTH - loc.x();  // Calc new X
    float newY = FIELD_GREEN_HEIGHT - loc.y(); // Calc new Y
    float newH = NBMath::subPIAngle(loc.h() + M_PI_FLOAT); //Flip the heading

    messages::RobotLocation mirroredLocation;
    mirroredLocation.set_x(newX);
    mirroredLocation.set_y(newY);
    mirroredLocation.set_h(newH);

    return mirroredLocation;
}

/*
 * @brief The following are all of the resetLoc functions
 */
void ParticleFilter::flipLoc()
{
    // Flip every particle
    ParticleIt iter;
    messages::RobotLocation flippedLocation;
    for(iter = particles.begin(); iter != particles.end(); iter++)
    {
        flippedLocation.CopyFrom(getMirrorLocation((*iter).getLocation()));
        (*iter).setLocation(flippedLocation);
    }
}

} // namespace localization
} // namespace man
