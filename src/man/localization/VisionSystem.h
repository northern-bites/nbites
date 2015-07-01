/**
 * @brief  A class responsible for scoring the particle swarm based on
 *         observations from the vision system and handling particle injection.
 *
 * @author EJ Googins <egoogins@bowdoin.edu>
 * @date   February 2013
 * @author Josh Imhoff <joshimhoff13@gmail.com>
 * @date   June 2015
 */

#pragma once

#include "NBMath.h"
#include "LineSystem.h"
#include "LandmarkSystem.h"
#include "LocStructs.h"
#include "FieldConstants.h"
#include "BallModel.pb.h"

#include <vector>
#include <list>
#include <map>
#include <boost/math/distributions.hpp>

namespace man {
namespace localization {

class VisionSystem
{
public:
    VisionSystem();
    ~VisionSystem();

    bool update(ParticleSet& particles,
                const messages::FieldLines& lines,
                const messages::Corners& corners,
                const messages::FilteredBall* ball,
                const messages::RobotLocation& lastEstimate);

    const std::vector<ReconstructedLocation>& getInjections() { return injections; }
    int getNumObservations() const { return numObservations; }
    double getAvgError() const { return avgError; }

private:
    LineSystem* lineSystem;
    LandmarkSystem* landmarkSystem;
    std::vector<ReconstructedLocation> injections;

    int numObservations;
    double avgError;
};

} // namespace localization
} // namespace man
