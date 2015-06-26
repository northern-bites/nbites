/**
 * @brief A class responsible for maintaing knowedge of corners
 *        and comparing projections and observations.
 *
 * @author EJ Googins <egoogins@bowdoin.edu>
 * @date   June 2013
 * @author Josh Imhoff <joshimhoff13@gmail.com>
 * @date   June 2015
 */

#pragma once

#include "Particle.h"
#include "Vision.pb.h"
#include "FieldConstants.h"
#include "../vision/Homography.h"
#include "../vision/Hough.h"

#include <map>
#include <vector>

namespace man {
namespace localization {

// TODO rename CornerID as VisionCornerID
enum class LocCornerID {
};

class CornerSystem {
public:
    CornerSystem();
    ~CornerSystem();

    LocCornerID matchObservation(const messages::Corner& observation, const messages::RobotLocation& loc);
    double scoreObservation(const messages::Corner& observation, const messages::RobotLocation& loc);

    static vision::Corner relRobotToAbsolute(const messages::Corner& observation, const messages::RobotLocation& loc);

private:
    void addCorner(vision::CornerID id, double x, double y);

    std::map<vision::CornerID, vision::Corner> corners;
};

} // namespace localization
} // namespace man

