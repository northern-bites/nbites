#include "VisionSystem.h"

#include "../vision/Hough.h"

#include <limits>

namespace man {
namespace localization {

VisionSystem::VisionSystem() 
    : injections(), numObservations(0), lowestError(0), avgError(0), weightedAvgError(0)
{
    lineSystem = new LineSystem;
}

VisionSystem::~VisionSystem() 
{
    delete lineSystem;
}

bool VisionSystem::update(ParticleSet& particles,
                          const messages::FieldLines& lines)
{
    numObservations = 0;
    lowestError = std::numeric_limits<float>::max();
    avgError = 0;
    weightedAvgError = 0;
 
    float totalWeight = 0.0f;
    float sumParticleError = 0.f;

    numObservations = lines.line_size();
    if (numObservations == 0)
        return false;

    // Loop over particles and adjust weights
    ParticleIt iter;
    for(iter = particles.begin(); iter != particles.end(); iter++) {
        Particle* particle = &(*iter);
        float curParticleError = 0;

        // Score particles according to how well they match with detected lines
        for (int i = 0; i < lines.line_size(); i++) {
            if (!LineSystem::shouldUse(lines.line(i)))
                continue;
            curParticleError += lineSystem->scoreObservation(lines.line(i), particle->getLocation());
        }

        // Set the particle's weight (no golf scores)
        // TODO divide by zero
        float avgErr = curParticleError / static_cast<float>(numObservations);
        particle->setWeight(1 / avgErr);
        totalWeight += particle->getWeight();

        // Update the total swarm error
        sumParticleError += avgErr;
        particle->setError(avgErr);
        if (avgErr < lowestError)
            lowestError = avgErr;
    }

    // Normalize the particle weights and calculate the weighted avg error
    weightedAvgError = 0.f;
    for(iter = particles.begin(); iter != particles.end(); iter++)
    {
        Particle* particle = &(*iter);
        particle->normalizeWeight(totalWeight);
        weightedAvgError += particle->getWeight() * particle->getError();
    }

    // Calculate avgError by dividing the total by the number of particles
    avgError = sumParticleError / static_cast<float>(particles.size());
    lowestError = lowestError;

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
                messages::RobotLocation pose = lineSystem->reconstructPosition(ids[i], field);
                ReconstructedLocation reconstructed(pose.x(), pose.y(), pose.h(), ids[i] == LocLineID::OurTopGoalbox);

                // Sanity check, reconstruction must be on field
                if( (reconstructed.x >= 0 && reconstructed.y <= FIELD_GREEN_WIDTH) &&
                    (reconstructed.y >= 0 && reconstructed.y <= FIELD_GREEN_HEIGHT)  )
                    injections.push_back(reconstructed);
            }
        }
    }

    // Weights were adjusted so return true
    return true;
}

} // namespace localization
} // namespace man
