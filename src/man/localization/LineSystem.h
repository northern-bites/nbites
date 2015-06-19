/**
 * @brief A class responsible for maintaing knowedge of lines on the field
 *        and comparing projectinos and observations
 *
 * @author EJ Googins <egoogins@bowdoin.edu>
 * @date   June 2013
 */

#pragma once

#include "Particle.h"
#include "Vision.pb.h"
#include "FieldConstants.h"
#include "../vision/Homography.h"

namespace man {
namespace localization {

class LineSystem {
public:
    LineSystem();
    ~LineSystem();

    double scoreObservation(const messages::FieldLine& observation, const Particle& particle);
    static void projectOntoField(messages::FieldLine& observation, const Particle& particle);

private:
    void addLine(float r, float t, float ep0, float ep1);
    static vision::GeoLine fromRelRobotToGlobal(const messages::FieldLine& relRobotLine, const Particle& particle);

    std::vector<vision::GeoLine> lines;
};

} // namespace localization
} // namespace man
