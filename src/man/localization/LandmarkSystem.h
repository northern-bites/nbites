/**
 * @brief A class responsible for maintaing knowedge of landmarks (corners, ball)
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

enum class LandmarkID {
    OurRightConcave = 0,
    OurLeftConcave,
    OurRightBox,       // Concave or convex depending on where standing
    OurLeftBox,        // Concave or convex depending on where standing
    OurRightT,
    OurLeftT,
    MidLeftT,
    MidRightT,
    TheirRightConcave,
    TheirLeftConcave,
    TheirRightBox,     // Concave or convex depending on where standing
    TheirLeftBox,      // Concave or convex depending on where standing
    TheirRightT,
    TheirLeftT,
    BallInSet,
    CenterCircle
};

// Stores the LandmarkID and (x, y) cartesian coordinate representation of the location
// of the landmark in absolute coordinates
// NOTE x is the second argument in the tuple, y is the third
typedef std::tuple<LandmarkID, double, double> Landmark;

class LandmarkSystem {
public:
    // Constructor
    LandmarkSystem();

    // Destructor
    ~LandmarkSystem() {}

    // Find corresponding corner in map
    // @param observation, the corner observation from the vision system in robot relative coords
    // @param loc, pose hypothesis used to map the observation to absolute coords
    // @returns the landmark 
    Landmark matchCorner(const messages::Corner& observation, 
                         const messages::RobotLocation& loc);

    // Find corresponding corner in map and return probability of correspondence
    // @param observation, the corner observation from the vision system in robot relative coords
    // @param loc, pose hypothesis used to map the observation to absolute coords
    // @returns the probability of correspondence
    double scoreCorner(const messages::Corner& observation, 
                       const messages::RobotLocation& loc);

    // Return probability of correspondence with center circle
    // @param observation, the circle observation from the vision system in robot relative coords
    // @param loc, pose hypothesis used to map the observation to absolute coords
    // @returns the probability of correspondence
    double scoreCircle(const messages::CenterCircle& observation, 
                       const messages::RobotLocation& loc);

    // Return probability of correspondence with ball in set
    // @param observation, the ball observation from the vision system in robot relative coords
    // @param loc, pose hypothesis used to map the observation to absolute coords
    // @returns the probability of correspondence
    double scoreBallInSet(const messages::FilteredBall& observation, 
                          const messages::RobotLocation& loc);

    // Set debug mode, enables print lines
    // @param debug_, true if debug mode
    void setDebug(bool debug_) { debug = debug_; }

    // Convert observation from relative robot coords to absolute coords
    // @param observation, the observation in robot relative coords
    // @param loc, pose hypothesis used to map the observation to absolute coords
    // @returns the observation in absolute coords
    static messages::RobotLocation relRobotToAbsolute(const messages::RobotLocation& observation, 
                                                      const messages::RobotLocation& loc);

private:
    // Return probability of correspondence between some observation and landmark
    // @param observation, the observation from the vision system in robot relative coords
    // @param correspondingLandmark, the corresponding landmark to score observation against
    // @param loc, pose hypothesis used to map the observation to absolute coords
    // @param onlyBearing, if true, only scores particle based on bearing 
    //                     between landmark in map and obsvervation
    // @returns the probability of correspondence
    // @note matchCorner, scoreCorner, and scoreBallInSet all call on this 
    //       method to do math involved in landmark scoring
    double scoreObservation(const messages::RobotLocation& observation, 
                            const Landmark& correspondingLandmark,
                            const messages::RobotLocation& loc,
                            bool onlyBearing = false);

    // Helper method
    void addCorner(vision::CornerID type, LandmarkID id, double x, double y);

    // Map
    std::map<vision::CornerID, std::vector<Landmark>> corners;
    Landmark circle;
    Landmark ballInSet;

    bool debug;
};

} // namespace localization
} // namespace man
