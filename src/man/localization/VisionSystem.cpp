#include "VisionSystem.h"

#include "../vision/Hough.h"

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

} // namespace localization
} // namespace man
