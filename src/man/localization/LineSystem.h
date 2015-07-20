/**
 * @brief A class responsible for maintaing knowedge of lines on the field
 *        and comparing projections and observations.
 *
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
    NotMatched = 0,    // black
    OurEndline,        // blue
    TheirEndline,      // red
    OurMidline,        // gray
    TheirMidline,      // gray
    OurTopGoalbox,     // magenta
    TheirTopGoalbox,   // orange
    RightSideline,     // cyan
    LeftSideline,      // pink
    OurRightGoalbox,   // yellow
    OurLeftGoalbox,    // green
    TheirRightGoalbox, // white
    TheirLeftGoalbox   // dark gray
};

class LineSystem {
public:
    // Constructor
    LineSystem();

    // Destructor
    ~LineSystem() {}

    // Find corresponding line in map
    // @param observation, the line observation from the vision system in robot relative coords
    // @param loc, pose hypothesis used to map the observation to absolute coords
    // @returns the id of the line in the map
    LocLineID matchLine(const messages::FieldLine& observation, const messages::RobotLocation& loc);

    // Find corresponding line in map and return probability of correspondence
    // @param observation, the line observation from the vision system in robot relative coords
    // @param loc, pose hypothesis used to map the observation to absolute coords
    // @returns the probability of correspondence
    double scoreLine(const messages::FieldLine& observation, const messages::RobotLocation& loc);

    // Reconstructs pose on field from line observation with known correspondence
    // @param id, the id of the line observation to reconstruct from
    // @param observation, the line observation from the vision system in robot relative coords
    // @returns the reconstructed robot pose
    // @note assumes that the entire line is seen by robot and thus can reconstruct
    //       complete location
    messages::RobotLocation reconstructFromMidpoint(LocLineID id, const messages::FieldLine& observation);

    // Reconstructs pose on field from line observation with known correspondence
    // @param id, the id of the line observation to reconstruct from
    // @param observation, the line observation from the vision system in robot relative coords
    // @returns the reconstructed robot pose
    // @note does not assume that the entire line is seen by the robot, so only
    //       can partially reconstruct position from line, parts of pose that cannot
    //       be reconstructed (either x or y) are set to -1
    messages::RobotLocation reconstructWoEndpoints(LocLineID id, const messages::FieldLine& observation);
    
    // Set debug mode, enables print lines
    // @param debug_, true if debug mode
    void setDebug(bool debug_) { debug = debug_; }

    // Check if line is suitable to use for localization
    // @param observation, the line observation from the vision system in robot relative coords
    // @param loc, pose hypothesis used to map the observation to absolute coords
    // @returns whether or not kick should be used for localization
    static bool shouldUse(const messages::FieldLine& observation, const messages::RobotLocation& loc); 

    // Convert observation from relative robot coords to absolute coords
    // @param observation, the observation in robot relative coords
    // @param loc, pose hypothesis used to map the observation to absolute coords
    // @returns the observation in absolute coords
    static vision::GeoLine relRobotToAbsolute(const messages::FieldLine& observation, const messages::RobotLocation& loc);

private:
    // Return probability of correspondence between observation and corresponding line
    // @param observation, the observation from the vision system in robot relative coords
    // @param correspondingLine, the corresponding line to score observation against
    // @param loc, pose hypothesis used to map the observation to absolute coords
    // @param wz0, the height of the camera used to find observation
    // @returns the probability of correspondence
    // @note matchLine and scoreLine call on this method to do math involved in line scoring
    double scoreObservation(const vision::GeoLine& observation, const vision::GeoLine& correspondingLine, const messages::RobotLocation& loc, double wz0);

    // Helper method
    void addLine(LocLineID id, float r, float t, float ep0, float ep1);

    // Map
    std::map<LocLineID, vision::GeoLine> lines;
    std::map<vision::LineID, std::vector<LocLineID>> visionToLocIDs;

    bool debug;
};

} // namespace localization
} // namespace man
