#include "ParticleFilter.h"

#include "LineSystem.h"
#include "DebugConfig.h"

#include <algorithm>

namespace man {
namespace localization {

ParticleFilter::ParticleFilter(ParticleFilterParams params)
    : parameters(params),
      setResetTransition(0),
      wSlow(0),
      wFast(0)
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
    badFrame = false;
    errorMagnitude = .8f;
}

ParticleFilter::~ParticleFilter()
{
    delete motionSystem;
    delete visionSystem;
}

void ParticleFilter::update(const messages::RobotLocation& odometryInput,
                            messages::FieldLines&          linesInput,
                            messages::Corners&             cornersInput,
                            const messages::FilteredBall*  ballInput)
{
    // Motion system and vision system update step
    motionSystem->update(particles, odometryInput, errorMagnitude);
    bool updatedVision = visionSystem->update(particles, linesInput, cornersInput, ballInput, poseEstimate);

    // Resample if vision updated
    if(updatedVision) {
        double wAvg = visionSystem->getAvgError();
        wSlow = wSlow + parameters.alphaSlow*(wAvg - wSlow);
        wFast = wFast + parameters.alphaFast*(wAvg - wFast);

        // Ad hoc method to determine if lost based on exponential filters
        // NOTE if set, behaviors may change action of robot to recover localization
        // NOTE not currently used
        if (1.0 - (wFast / wSlow) > parameters.lostThreshold)
            lost = true;
        else
            lost = false;

        resample();
    }

    // Update filters estimate
    updateEstimate();

    // For debug tools, project lines and corners onto field, set IDs, etc.
    updateFieldForDebug(linesInput, cornersInput); 
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

    poseEstimate.set_uncert(wFast / wSlow);

    bool offField = !(poseEstimate.x() >= 0 && poseEstimate.x() <= FIELD_GREEN_WIDTH && 
                      poseEstimate.y() >= 0 && poseEstimate.y() <= FIELD_GREEN_HEIGHT); 
    poseEstimate.set_lost(offField);

    // double variance = 0;
    // for(iter = particles.begin(); iter != particles.end(); ++iter)
    // {

    //     variance += pow(poseEstimate.h() - (*iter).getLocation().h(), 2);
    // }

    // std::cout << variance << std::endl;
}

void ParticleFilter::updateFieldForDebug(messages::FieldLines& lines,
                                         messages::Corners& corners)
{
    LineSystem lineSystem;
    lineSystem.setDebug(false);
    for (int i = 0; i < lines.line_size(); i++) {
        // Get line
        messages::FieldLine& field = *lines.mutable_line(i);

        // Set correspondence and scores
        if (!LineSystem::shouldUse(lines.line(i))) {
            // Lines that the particle filter did not use are given -1 as ID
            field.set_id(0);
        } else {
            // Otherwise line system handles classification and scoring
            LocLineID id = lineSystem.matchLine(field, poseEstimate);
            field.set_prob(lineSystem.scoreLine(field, poseEstimate));
            field.set_correspondence(static_cast<int>(id));
        }

        // Project lines onto the field
        vision::GeoLine projected = LineSystem::relRobotToAbsolute(lines.line(i), poseEstimate);
        messages::HoughLine& hough = *field.mutable_inner();

        hough.set_r(projected.r());
        hough.set_t(projected.t());
        hough.set_ep0(projected.ep0());
        hough.set_ep1(projected.ep1());
    }

    LandmarkSystem landmarkSystem;
    landmarkSystem.setDebug(false);
    for (int i = 0; i < corners.corner_size(); i++) {
        // Get corner
        messages::Corner& corner = *corners.mutable_corner(i);

        messages::RobotLocation cornerRel;
        cornerRel.set_x(corner.x());
        cornerRel.set_y(corner.y());

        // Set correspondence and scores
        LandmarkID id = std::get<0>(landmarkSystem.matchCorner(corner, poseEstimate));
        corner.set_prob(landmarkSystem.scoreCorner(corner, poseEstimate));
        corner.set_correspondence(static_cast<int>(id));

        // Project corner onto the field
        messages::RobotLocation cornerAbs = LandmarkSystem::relRobotToAbsolute(cornerRel, poseEstimate);
        corner.set_x(cornerAbs.x());
        corner.set_y(cornerAbs.y());
    }
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

/*
 * @brief The following are all of the resetLoc functions
 */
void ParticleFilter::resetLoc()
{
    framesSinceReset = 0;
#ifdef DEBUG_LOC
    std::cout << "WTF: LOC IS RESETTING!" << std::endl;
#endif
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
    framesSinceReset = 0;
#ifdef DEBUG_LOC
    std::cout << "WTF: LOC IS RESETTING to a pose!" << std::endl;
#endif

    // HACK HACK HACK - If told to reset to negative x,y,h then flip
    if (x<0 && y<0 && h<0)
    {
        flipLoc();
        return;
    }


    // Reset the estimate
    poseEstimate.set_x(x);
    poseEstimate.set_y(y);
    poseEstimate.set_h(NBMath::subPIAngle(h));

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
    framesSinceReset = 0;
#ifdef DEBUG_LOC
    std::cout << "WTF: LOC IS RESETTING to two locations!" << std::endl;
#endif

    // Reset the estimates.
    poseEstimate.set_x(x);
    poseEstimate.set_y(y);
    poseEstimate.set_h(NBMath::subPIAngle(h));

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
    framesSinceReset = 0;
#ifdef DEBUG_LOC
    std::cout << "WTF: LOC IS RESETTING to a SIDE!" << std::endl;
#endif
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
        cdf[prev + iter->getWeight()] = (*iter);
        prev += iter->getWeight();
    }

    // Setup random number generator
    boost::mt19937 rng;
    rng.seed(static_cast<unsigned>(std::time(0)));
    boost::uniform_01<boost::mt19937> gen(rng);

    // Setup
    ParticleSet newParticles;
    const std::vector<ReconstructedLocation>& injections = visionSystem->getInjections();

    // Either inject particle or sample with replacement according to the
    // normalized weights, and place in a new particle set
    //
    // NOTE we only consider injecting particles if vision system found 
    //      suitable observations
    int ni = 0;
    for(int i = 0; i < parameters.numParticles; ++i) {
        double randInjectOrSample = gen();
        if (injections.size() && randInjectOrSample < std::max<double>(0, 1.0 - (wFast / wSlow))) {
            // Inject particles according to sensor measurements
            ReconstructedLocation injection = injections[rand() % injections.size()];
            messages::RobotLocation sample = injection.sample();
            ni++;

            Particle reconstructedParticle(sample.x(), sample.y(), sample.h(), 1/250);
            newParticles.push_back(reconstructedParticle);
        } else {
            // Resample from this frame's swarm based on scores
            double randSample = gen();

            if (cdf.upper_bound(randSample) == cdf.end())
                newParticles.push_back(cdf.begin()->second); // NOTE return something that DEF exists
            else
                newParticles.push_back(cdf.upper_bound(randSample)->second);
        }
    }

    // std::cout << "TEST" << std::endl;
    // std::cout << 1.0 - (wFast / wSlow) << std::endl;
    // std::cout << wFast << std::endl;
    // std::cout << wSlow << std::endl;
    // std::cout << ni << std::endl;

    // Update particles
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
