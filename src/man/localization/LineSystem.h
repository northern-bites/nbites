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
#include "../vision/Hough.h"

#include <map>
#include <vector>

namespace man {
namespace localization {

// TODO rename LineID as VisionLineID
// NOTE comments map LocLineID to color found in LocSwarmView
enum class LocLineID {
    NotMatched = 0,  // black
    OurEndline,      // blue
    TheirEndline,    // red
    OurMidline,      // gray
    TheirMidline,    // gray
    OurTopGoalbox,   // magenta
    TheirTopGoalbox, // orange
    RightSideline,   // cyan
    LeftSideline     // pink
};

class LineSystem {
public:
    LineSystem();
    ~LineSystem();

    LocLineID matchObservation(const messages::FieldLine& observation, const messages::RobotLocation& loc);
    double scoreObservation(const messages::FieldLine& observation, const messages::RobotLocation& loc);
    messages::RobotLocation reconstructFromMidpoint(LocLineID id, const messages::FieldLine& observation);
    messages::RobotLocation reconstructWoEndpoints(LocLineID id, const messages::FieldLine& observation);
    void setDebug(bool debug_) { debug = debug_; }

    static vision::GeoLine relRobotToAbsolute(const messages::FieldLine& observation, const messages::RobotLocation& loc);
    static bool shouldUse(const messages::FieldLine& observation); 

private:
    void addLine(LocLineID id, float r, float t, float ep0, float ep1);

    std::map<LocLineID, vision::GeoLine> lines;
    std::map<vision::LineID, std::vector<LocLineID>> visionToLocIDs;

    bool debug;
};

} // namespace localization
} // namespace man
