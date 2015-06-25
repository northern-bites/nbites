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
                          const messages::FieldLines& lines,
                          const messages::Corners& corners)
{
    numObservations = 0;
    lowestError = std::numeric_limits<float>::max();
    avgError = 0;
    weightedAvgError = 0;
    Particle* best = NULL;
 
    float totalWeight = 0.0f;
    float sumParticleError = 0.f;

    for (int i = 0; i < lines.line_size(); i++) {
        if (!LineSystem::shouldUse(lines.line(i)))
            continue;
        numObservations++;
    }

    if (numObservations == 0)
        return false;

    // Loop over particles and adjust weights
    ParticleIt iter;
    for(iter = particles.begin(); iter != particles.end(); iter++) {
        Particle* particle = &(*iter);
        float curParticleError = 0;

        // Score particles according to how well they match with detected lines
        bool firstError = true;
        for (int i = 0; i < lines.line_size(); i++) {
            if (!LineSystem::shouldUse(lines.line(i)))
                continue;

            if (firstError) {
                curParticleError = lineSystem->scoreObservation(lines.line(i), particle->getLocation());
                firstError = false;
            } else
                curParticleError = curParticleError*lineSystem->scoreObservation(lines.line(i), particle->getLocation());
        }

        // Set the particle's weight (no golf scores)
        float avgErr = curParticleError / static_cast<float>(numObservations);
        // std::cout << curParticleError << std::endl;
        particle->setWeight(curParticleError);
        totalWeight += particle->getWeight();

        // Update the total swarm error
        sumParticleError += avgErr;
        particle->setError(avgErr);
        if (curParticleError < lowestError) {
            lowestError = curParticleError;
            best = particle;
        }
    }

    // Normalize the particle weights and calculate the weighted avg error
    weightedAvgError = 0.f;
    for(iter = particles.begin(); iter != particles.end(); iter++) {
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
    // for (int i = 0; i < lines.line_size(); i++) {
    //     const messages::FieldLine& field = lines.line(i);
    //     if (field.id() == static_cast<int>(vision::LineID::TopGoalbox)) {
    //         const messages::HoughLine& inner = field.inner();

    //         // NOTE particle filter makes sure we only inject particles on
    //         //      the side of the field that our estimate is on
    //         std::vector<LocLineID> ids { LocLineID::OurTopGoalbox, LocLineID::TheirTopGoalbox };
    //         for (int i = 0; i < ids.size(); i++) {
    //             messages::RobotLocation pose = lineSystem->reconstructFromMidpoint(ids[i], field);
    //             ReconstructedLocation reconstructed(pose.x(), pose.y(), pose.h());
    //             injections.push_back(reconstructed);
    //         }
    //     }
    // }

    // Weights were adjusted so return true
    return true;
}

} // namespace localization
} // namespace man
