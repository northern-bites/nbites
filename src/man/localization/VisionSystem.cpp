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
}

VisionSystem::~VisionSystem() 
{
    delete lineSystem;
}

bool VisionSystem::update(ParticleSet& particles,
                          const messages::FieldLines& lines,
                          const messages::Corners& corners)
{
    numObservations = 0;
    avgError = 0;

    // Check that there are valid observations
    for (int i = 0; i < lines.line_size(); i++) {
        if (!LineSystem::shouldUse(lines.line(i)))
            continue;
        numObservations++;
    }
    if (numObservations == 0)
        return false;

    // Loop over particles and adjust weights
    ParticleIt iter;
    double totalWeight = 0;
    for(iter = particles.begin(); iter != particles.end(); iter++) {
        Particle* particle = &(*iter);
        float curParticleError = 0;

        // Score particles according to how well they match with detected lines
        bool firstLine = true;
        for (int i = 0; i < lines.line_size(); i++) {
            if (!LineSystem::shouldUse(lines.line(i)))
                continue;

            if (firstLine) {
                curParticleError = lineSystem->scoreObservation(lines.line(i), particle->getLocation());
                firstLine = false;
            } else
                curParticleError = curParticleError*lineSystem->scoreObservation(lines.line(i), particle->getLocation());
        }

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

    // Weights were adjusted so return true
    return true;
}

bool VisionSystem::update(ParticleSet& particles,
                          const messages::FieldLines& lines,
                          const messages::Corners& corners,
                          const messages::FilteredBall& ball)
{
    // Call overloaded update
    bool ret = update(particles, lines, corners);
    if (!ret) return false;

    // Particle injections 
    // (2) Reconstruct pose from ball in set
    // Find line close to ball, should be the midline since in set
    // TODO midline check, closest dist to line below some threshold, length
    for (int i = 0; i < lines.line_size(); i++) {
        const messages::FieldLine& field = lines.line(i);
        const messages::HoughLine& inner = field.inner();

        // Skip short lines, probably in center circle
        if (!LineSystem::shouldUse(field))
                continue;

        // Polar to cartesian
        double ballRelX = ball.distance() * cos(ball.bearing());
        double ballRelY = ball.distance() * sin(ball.bearing());

        std::cout << "TEST" << std::endl;
        std::cout << ball.distance() << std::endl;
        std::cout << ball.bearing() << std::endl;

        std::cout << ballRelX << std::endl;
        std::cout << ballRelY << std::endl;

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

            std::cout << "RESULTS" << std::endl;
            std::cout << ballAbsX << std::endl;
            std::cout << ballAbsY << std::endl;

            // Add injection and return
            // TODO remove hack once landmark system is in place
            for (int i = 0; i < 300; i++) {
                ReconstructedLocation reconstructed(fromLineAndBall.x(), fromLineAndBall.y(), fromLineAndBall.h());
                injections.push_back(reconstructed);
            }
            return true;
        }
    }
}

} // namespace localization
} // namespace man
