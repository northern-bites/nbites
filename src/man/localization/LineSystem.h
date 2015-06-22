/**
 * @brief A class responsible for maintaing knowedge of lines on the field
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

namespace man {
namespace localization {

// TODO rename LineID as VisionLineID
enum class LocLineID {
    NotMatched,
    OurEndline,
    TheirEndline,
    OurMidline,
    TheirMidline,
    OurTopGoalbox,
    TheirTopGoalbox,
    RightSideline,
    LeftSideline
};

class LineSystem {
public:
    LineSystem();
    ~LineSystem();

    LocLineID matchObservation(const messages::FieldLine& observation, const messages::RobotLocation& loc);
    double scoreObservation(const messages::FieldLine& observation, const messages::RobotLocation& loc);
    messages::RobotLocation reconstructPosition(LocLineID id, const messages::FieldLine& observation);

    static vision::GeoLine relRobotToAbsolute(const messages::FieldLine& observation, const messages::RobotLocation& loc);
    static bool shouldUse(const messages::FieldLine& observation); 

private:
    void addLine(LocLineID id, float r, float t, float ep0, float ep1);

    std::map<LocLineID, vision::GeoLine> lines;
};

} // namespace localization
} // namespace man
