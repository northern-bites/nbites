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
    numObservations += corners.corner_size();
    if (ball != NULL) numObservations++;

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
        for (int i = 0; i < corners.corner_size(); i++)
            curParticleError = curParticleError*landmarkSystem->scoreCorner(corners.corner(i), particle->getLocation());

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
    // Find line close to ball, should be the midline since in set
    // TODO how to determine line to use
    // TODO refactor reconstruction code to use landmark system
    if (ball != NULL) {
        for (int i = 0; i < lines.line_size(); i++) {
            const messages::FieldLine& field = lines.line(i);
            const messages::HoughLine& inner = field.inner();

            // Skip short lines, probably in center circle
            if (!LineSystem::shouldUse(field))
                    continue;

            // Polar to cartesian
            double ballRelX = ball->distance() * cos(ball->bearing());
            double ballRelY = ball->distance() * sin(ball->bearing());

            // Rotate line to loc rel robot coordinate system 
            vision::GeoLine line;
            line.set(inner.r(), inner.t(), inner.ep0(), inner.ep1());
            line.translateRotate(0, 0, -(M_PI / 2));

            // Project ball onto line, find distance to line
            double distToLine = line.qDist(ballRelX, ballRelY);

            // If close, found the midline, reconstruct from the midline and the ball
            if (distToLine < 30) {
                // Recontruct x and h from midline and y from ball
                messages::RobotLocation fromLine = lineSystem->reconstructWoEndpoints(LocLineID::OurMidline, field);
                messages::RobotLocation fromLineAndBall = fromLine;

                // Rotate to absolute coordinate system
                double ballAbsX, ballAbsY;
                vision::translateRotate(ballRelX, ballRelY, 0, 0, fromLine.h(), ballAbsX, ballAbsY);
                fromLineAndBall.set_y(CENTER_FIELD_Y - ballAbsY);

                // Add injection and return
                ReconstructedLocation reconstructed(fromLineAndBall.x(), fromLineAndBall.y(), fromLineAndBall.h());
                injections.push_back(reconstructed);
            }
        }
    }

    // Weights were adjusted so return true
    return true;
}

} // namespace localization
} // namespace man
