#include "VisionSystem.h"

#include "../vision/Hough.h"
#include "FieldConstants.h"

#include <limits>

namespace man {
namespace localization {

VisionSystem::VisionSystem() 
    : injections(), numObservations(0), avgError(0)
{
    lineSystem = new LineSystem;
    landmarkSystem = new LandmarkSystem;
}

VisionSystem::~VisionSystem() 
{
    delete lineSystem;
    delete landmarkSystem;
}

bool VisionSystem::update(ParticleSet& particles,
                          const messages::FieldLines& lines,
                          const messages::Corners& corners,
                          const messages::FilteredBall* ball)
{
    numObservations = 0;
    avgError = 0;

    // Count observations
    for (int i = 0; i < lines.line_size(); i++) {
        if (!LineSystem::shouldUse(lines.line(i)))
            continue;
        numObservations++;
    }
    // numObservations += corners.corner_size();
    if (ball != NULL) 
        numObservations++;

    // If no observations, return without updating weights
    if (!numObservations)
        return false;

    // Loop over particles and adjust weights
    ParticleIt iter;
    double totalWeight = 0;
    for(iter = particles.begin(); iter != particles.end(); iter++) {
        Particle* particle = &(*iter);
        float curParticleError = 1;

        // Score particle from line observations
        for (int i = 0; i < lines.line_size(); i++) {
            if (!LineSystem::shouldUse(lines.line(i)))
                continue;
            curParticleError = curParticleError*lineSystem->scoreObservation(lines.line(i), particle->getLocation());
        }

        // Score particle from corner observations
        // for (int i = 0; i < corners.corner_size(); i++)
        //     curParticleError = curParticleError*landmarkSystem->scoreCorner(corners.corner(i), particle->getLocation());

        // Score particle from ball observation if in game set
        if (ball != NULL)
            curParticleError = curParticleError*landmarkSystem->scoreBallInSet(*ball, particle->getLocation());

        // Set the particle's weight
        particle->setWeight(curParticleError);
        totalWeight += particle->getWeight();
    }

    // Normalize the particle weights
    for(iter = particles.begin(); iter != particles.end(); iter++) {
        Particle* particle = &(*iter);
        particle->normalizeWeight(totalWeight);
    }

    // Calculate error metric for particle filter
    avgError = totalWeight / static_cast<float>(particles.size());

    // Particle injections
    // (1) Reconstruct pose by finding the midpoint of the top goalbox
    injections.clear();
    for (int i = 0; i < lines.line_size(); i++) {
        const messages::FieldLine& field = lines.line(i);
        if (field.id() == static_cast<int>(vision::LineID::TopGoalbox)) {
            const messages::HoughLine& inner = field.inner();

            // NOTE particle filter makes sure we only inject particles on
            //      the side of the field that our estimate is on
            std::vector<LocLineID> ids { LocLineID::OurTopGoalbox, LocLineID::TheirTopGoalbox };
            for (int i = 0; i < ids.size(); i++) {
                messages::RobotLocation pose = lineSystem->reconstructFromMidpoint(ids[i], field);
                ReconstructedLocation reconstructed(pose.x(), pose.y(), pose.h());
                injections.push_back(reconstructed);
            }
        }
    }

    // (2) Reconstruct pose from ball in set
    if (ball != NULL) {
        messages::FieldLine midline;
        double minDist = std::numeric_limits<double>::max();

        // Ball, polar to cartesian
        double ballRelX, ballRelY;
        vision::polarToCartesian(ball->distance(), ball->bearing(), ballRelX, ballRelY);

        // Find line that is closest to the ball, should be midline since in set
        for (int i = 0; i < lines.line_size(); i++) {
            const messages::FieldLine& field = lines.line(i);
            const messages::HoughLine& inner = field.inner();

            // Rotate line to loc rel robot coordinate system 
            vision::GeoLine line;
            line.set(inner.r(), inner.t(), inner.ep0(), inner.ep1());
            line.translateRotate(0, 0, -(M_PI / 2));

            // Project ball onto line, find distance to line
            double distToLine = fabs(line.pDist(ballRelX, ballRelY));

            // Check for min distance
            if (minDist > distToLine) {
                midline = field;
                minDist = distToLine;
            }
        }

        // If sufficiently close, found the midline, reconstruct location
        if (minDist < 60) {
            // Recontruct x and h from midline and y from ball
            messages::RobotLocation fromLine = lineSystem->reconstructWoEndpoints(LocLineID::OurMidline, midline);
            messages::RobotLocation fromLineAndBall = fromLine;

            // Rotate to absolute coordinate system
            double ballAbsX, ballAbsY;
            vision::translateRotate(ballRelX, ballRelY, 0, 0, fromLine.h(), ballAbsX, ballAbsY);
            fromLineAndBall.set_y(CENTER_FIELD_Y - ballAbsY);

            // Add injection and return
            ReconstructedLocation reconstructed(fromLineAndBall.x(), fromLineAndBall.y(), fromLineAndBall.h());
            injections.push_back(reconstructed);

            // std::cout << "Injecting in set!" << std::endl;
            // std::cout << fromLineAndBall.x() << std::endl;
            // std::cout << fromLineAndBall.y() << std::endl;
            // std::cout << fromLineAndBall.h() << std::endl;
        }
    }

    // Weights were adjusted so return true
    return true;
}

} // namespace localization
} // namespace man
