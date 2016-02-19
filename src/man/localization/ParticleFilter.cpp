#include "ParticleFilter.h"

#include "LineModel.h"
#include "DebugConfig.h"
#include "Profiler.h"

#include <algorithm>

namespace man {
namespace localization {

ParticleFilter::ParticleFilter(ParticleFilterParams params_)
    : params(params_),
      rng(),
      poseEstimate(),
      particles(params.numParticles, Particle()),
      swarm(),
      wSlow(0),
      wFast(0)
{
    // Seed random number generator
    rng.seed(std::time(0));

    // Init motion and vision models
    motionModel = new MotionModel(params);
    visionModel = new VisionModel(params);
}

ParticleFilter::~ParticleFilter()
{
    delete motionModel;
    delete visionModel;
}

void ParticleFilter::update(const messages::RobotLocation& odometryInput,
                            messages::Vision&              visionInput,
                            const messages::FilteredBall*  ballInput)
{
    // Motion system update
    PROF_ENTER(P_LOC_MOTION)
    motionModel->update(particles, odometryInput);
    PROF_EXIT(P_LOC_MOTION)

    // Vision system update
    PROF_ENTER(P_LOC_VISION)
    bool updatedVision = visionModel->update(particles, visionInput, ballInput, poseEstimate);
    PROF_EXIT(P_LOC_VISION)

    PROF_ENTER(P_LOC_RESAMPLE)
    // Resample if vision updated
    if (updatedVision) {
        // Update exponential filters used in Augmented MCL
        double wAvg = visionModel->getAvgError();
        wSlow = wSlow + params.alphaSlow*(wAvg - wSlow);
        wFast = wFast + params.alphaFast*(wAvg - wFast);

        // Resample swarm
        resample(ballInput != NULL);
    }
    PROF_EXIT(P_LOC_RESAMPLE)

    // Update filters estimate
    updateEstimate();

    // For debug tools
    updateObservationsForDebug(visionInput); 
}

const messages::ParticleSwarm& ParticleFilter::getCurrentSwarm()
{
    swarm.clear_particle();

    // Fill protobuf with particles in particles vector
    messages::Particle newParticle;
    ParticleIt iter;
    for(iter = particles.begin(); iter != particles.end(); ++iter) {
        newParticle.mutable_loc()->CopyFrom((*iter).getLocation());
        newParticle.set_weight((*iter).getWeight());
        swarm.add_particle()->CopyFrom(newParticle);
    }

    return swarm;
}

void ParticleFilter::resetLocTo(float x, float y, float h,
                                LocNormalParams normalParams)
{
    particles.clear();

    float weight = 1.0f / params.numParticles;
    for (int i = 0; i < params.numParticles; ++i) {
        float pX = sampleNormal(x, normalParams.sigma_x);
        float pY = sampleNormal(y, normalParams.sigma_x);
        float pH = sampleNormal(h, normalParams.sigma_h);
        Particle p(pX, pY, pH, weight);
        particles.push_back(p);
    }
}

void ParticleFilter::resample(bool inSet)
{
    // Map each normalized weight to the corresponding particle
    std::map<float, Particle> cdf;
    float prev = 0.0f;
    ParticleIt iter;
    for (iter = particles.begin(); iter != particles.end(); ++iter) {
        cdf[prev + iter->getWeight()] = (*iter);
        prev += iter->getWeight();
    }

    // Setup random number generator
    boost::uniform_01<boost::mt19937> gen(rng);

    // Setup
    ParticleSet newParticles;
    const std::vector<ReconstructedLocation>& injections = visionModel->getInjections();

    // China 2015 hack
    // If in set and see ball suitable for reconstruction, completely replace 
    // swarm with injections
    //
    // FUTURE WORK, this should not be a special case, should use standard augmented
    //              MCL injection strategy (see below) for goalbox, center 
    //              cirlce, and ball in set
    if (inSet && injections.size()) {
        for (int i = 0; i < params.numParticles; ++i) {
            ReconstructedLocation injection = injections[rand() % injections.size()];
            messages::RobotLocation sample = injection.sample();

            Particle reconstructedParticle(sample.x(), sample.y(), sample.h(), 1/250);
            newParticles.push_back(reconstructedParticle);
        }
    // Augmented MCL
    // Either inject particle or sample with replacement according to the
    // normalized weights, and place in a new particle set
    //
    // NOTE we only consider injecting particles if vision system found 
    //      suitable observations
    } else {
        for(int i = 0; i < params.numParticles; ++i) {
            double randInjectOrSample = gen();
            if (params.injectionOn && injections.size() && randInjectOrSample < std::max<double>(0, 1.0 - (wFast / wSlow))) {
                // Inject particles according to sensor measurements
                ReconstructedLocation injection = injections[rand() % injections.size()];
                messages::RobotLocation sample = injection.sample();

                Particle reconstructedParticle(sample.x(), sample.y(), sample.h(), 1/250);
                newParticles.push_back(reconstructedParticle);
            } else {
                // Resample from this frame's swarm based on scores
                double randSample = gen();

                if (cdf.upper_bound(randSample) == cdf.end())
                    newParticles.push_back(cdf.begin()->second); // NOTE return something that def exists
                else
                    newParticles.push_back(cdf.upper_bound(randSample)->second);
            }
        }
    }

    // Update particles
    particles = newParticles;
}

void ParticleFilter::updateEstimate()
{
    // Estimate is average of x, y, and h components of particle swarm
    float sumX = 0;
    float sumY = 0;
    float sumH = 0;

    ParticleIt iter;
    for(iter = particles.begin(); iter != particles.end(); ++iter) {
        sumX += (*iter).getLocation().x();
        sumY += (*iter).getLocation().y();
        sumH += (*iter).getLocation().h();
    }

    float previousXEstimate = poseEstimate.x();
    float previousYEstimate = poseEstimate.y();
    float previousHEstimate = poseEstimate.h();

    poseEstimate.set_x(sumX/params.numParticles);
    poseEstimate.set_y(sumY/params.numParticles);
    poseEstimate.set_h(NBMath::subPIAngle(sumH/params.numParticles));

    // Uncertainity is based the ratio of a fast exponential filter and a slow
    // exponential filter of average observation probability
    // NOTE the idea comes from Augmented MCL, see resample()
    //
    // FUTURE WORK, better uncertainity metrics would allow for recovery behaviors and
    //              smarter shooting behaviors (e.g. don't kick big when might 
    //              be lost), this is a hard problem but one very worth working on
    poseEstimate.set_uncert(wFast / wSlow);

    // Currently the lost boolean is only true if the robot believes itself to 
    // be off field since by the rules this is impossible
    //
    // FUTURE WORK, better lost metrics would allow for recovery behaviors and
    //              smarter shooting behaviors (e.g. don't kick big when might 
    //              be lost), this is a hard problem but one very worth working on
    bool offField = !(poseEstimate.x() >= 0 && poseEstimate.x() <= FIELD_GREEN_WIDTH && 
                      poseEstimate.y() >= 0 && poseEstimate.y() <= FIELD_GREEN_HEIGHT); 
    poseEstimate.set_lost(offField);
}

void ParticleFilter::updateObservationsForDebug(messages::Vision& vision)
{
    // (1) Lines
    LineModel lineSystem(params);
    lineSystem.setDebug(false);
    for (int i = 0; i < vision.line_size(); i++) {
        // Get line
        messages::FieldLine& field = *vision.mutable_line(i);

        // Set correspondence and scores
        if (!LineModel::shouldUse(vision.line(i), poseEstimate)) {
            // Lines that the particle filter did not use are given -1 as ID
            field.set_id(0);
        } else {
            // Otherwise line system handles classification and scoring
            LocLineID id = lineSystem.matchLine(field, poseEstimate);
            field.set_prob(lineSystem.scoreLine(field, poseEstimate));
            field.set_correspondence(static_cast<int>(id));
        }

        // Project lines onto the field
        vision::GeoLine projected = LineModel::relRobotToAbsolute(vision.line(i), poseEstimate);
        messages::HoughLine& hough = *field.mutable_inner();

        hough.set_r(projected.r());
        hough.set_t(projected.t());
        hough.set_ep0(projected.ep0());
        hough.set_ep1(projected.ep1());
    }

    // (2) Corners
    LandmarkModel landmarkSystem(params);
    landmarkSystem.setDebug(false);
    for (int i = 0; i < vision.corner_size(); i++) {
        // Get corner
        messages::Corner& corner = *vision.mutable_corner(i);

        // Set correspondence and score
        LandmarkID id = std::get<0>(landmarkSystem.matchCorner(corner, poseEstimate));
        corner.set_prob(landmarkSystem.scoreCorner(corner, poseEstimate));
        corner.set_correspondence(static_cast<int>(id));

        // Project corner onto the field
        messages::RobotLocation cornerRel;
        cornerRel.set_x(corner.x());
        cornerRel.set_y(corner.y());

        messages::RobotLocation cornerAbs = LandmarkModel::relRobotToAbsolute(cornerRel, poseEstimate);
        corner.set_x(cornerAbs.x());
        corner.set_y(cornerAbs.y());
    }

    // (3) Center circle
    messages::CenterCircle& circle = *vision.mutable_circle();

    // Set score
    circle.set_prob(landmarkSystem.scoreCircle(circle, poseEstimate));

    // Project circle onto the field
    messages::RobotLocation circleRel;
    circleRel.set_x(circle.x());
    circleRel.set_y(circle.y());

    messages::RobotLocation circleAbs = LandmarkModel::relRobotToAbsolute(circleRel, poseEstimate);
    circle.set_x(circleAbs.x());
    circle.set_y(circleAbs.y());

    // (4) Ball
    // FUTURE WORK, would be better if filtered ball was used here, as it is used
    //              in VisionModel, not the visual ball
    messages::VBall& ball = *vision.mutable_ball();

    // Project ball onto the field
    double ballRelX, ballRelY;
    vision::polarToCartesian(ball.distance(), ball.bearing(), ballRelX, ballRelY);

    messages::RobotLocation ballRel;
    ballRel.set_x(ballRelX);
    ballRel.set_y(ballRelY);

    messages::RobotLocation ballAbs = LandmarkModel::relRobotToAbsolute(ballRel, poseEstimate);
    ball.set_x(ballAbs.x());
    ball.set_y(ballAbs.y());
}

} // namespace localization
} // namespace man
