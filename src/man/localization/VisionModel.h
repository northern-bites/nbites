/**
 * @brief  A class responsible for scoring the particle swarm based on
 *         observations from the vision system and handling particle injection
 *
 * @author EJ Googins <egoogins@bowdoin.edu>
 * @date   February 2013
 * @author Josh Imhoff <joshimhoff13@gmail.com>
 * @date   June 2015
 */

#pragma once

#include "NBMath.h"
#include "LineModel.h"
#include "LandmarkModel.h"
#include "LocStructs.h"
#include "FieldConstants.h"

#include <vector>
#include <list>
#include <map>
#include <boost/math/distributions.hpp>

namespace man {
namespace localization {

class VisionModel
{
public:
    // Constructor
    // @param params_, the particle filter params, including for sensor model
    VisionModel(const struct ParticleFilterParams& params_);

    // Destructor
    ~VisionModel();

    // Scores particles according to observations from vision
    // @param particles, the set of particles that represent localization belief
    // @param vision, observations from the vision system
    // @param ball, the filtered ball from the balltrack module, also an observation
    //              when robot is in set
    // @param lastEstimate, the localization estimate made by the loc system on
    //                      the last frame, used in reconstructing pose from observations
    // @returns whether or not particle scores were updated
    bool update(ParticleSet& particles,
                const messages::Vision& vision,
                const messages::FilteredBall* ball,
                const messages::RobotLocation& lastEstimate);

    // Get list of robot poses reconstructed from observations to be used in
    // particle injection step of augmented MCL
    // @returns vector of reconstructed poses
    const std::vector<ReconstructedLocation>& getInjections() { return injections; }
    
    // Get number of observations used in scoring particles
    // @returns the number of observations
    int getNumObservations() const { return numObservations; }

    // Get average probability or error of particle in swarm
    // @returns the average probability
    double getAvgError() const { return avgError; }

private:
    const struct ParticleFilterParams& params;
    
    LineModel* lineSystem;
    LandmarkModel* landmarkSystem;

    std::vector<ReconstructedLocation> injections;

    int numObservations;
    double avgError;
};

} // namespace localization
} // namespace man
