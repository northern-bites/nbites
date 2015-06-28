#include "LandmarkSystem.h"

#include <limits>
#include <boost/math/distributions/normal.hpp>

namespace man {
namespace localization {

LandmarkSystem::LandmarkSystem() 
    : corners(), ballInSet()
{
    // Construct map
    // Init corner map
    corners[vision::CornerID::Concave] = std::vector<Landmark>();
    corners[vision::CornerID::Convex] = std::vector<Landmark>();
    corners[vision::CornerID::T] = std::vector<Landmark>();

    // Add corners to map
    addCorner(vision::CornerID::Concave, LandmarkID::OurRightConcave, GREEN_PAD_X, GREEN_PAD_Y);
    addCorner(vision::CornerID::Concave, LandmarkID::OurLeftConcave, GREEN_PAD_X, GREEN_PAD_Y + FIELD_WHITE_HEIGHT);
    addCorner(vision::CornerID::Convex, LandmarkID::OurRightConvex, GREEN_PAD_X + GOALBOX_DEPTH, BLUE_GOALBOX_BOTTOM_Y);
    addCorner(vision::CornerID::Convex, LandmarkID::OurLeftConvex, GREEN_PAD_X + GOALBOX_DEPTH, BLUE_GOALBOX_TOP_Y);
    addCorner(vision::CornerID::T, LandmarkID::OurRightT, GREEN_PAD_X, BLUE_GOALBOX_BOTTOM_Y);
    addCorner(vision::CornerID::T, LandmarkID::OurLeftT, GREEN_PAD_X, BLUE_GOALBOX_TOP_Y);
    addCorner(vision::CornerID::T, LandmarkID::MidRightT, CENTER_FIELD_X, GREEN_PAD_Y);
    addCorner(vision::CornerID::T, LandmarkID::MidLeftT, CENTER_FIELD_X, GREEN_PAD_Y + FIELD_WHITE_HEIGHT);
    addCorner(vision::CornerID::Concave, LandmarkID::TheirRightConcave, GREEN_PAD_X + FIELD_WHITE_WIDTH, GREEN_PAD_Y);
    addCorner(vision::CornerID::Concave, LandmarkID::TheirLeftConcave, GREEN_PAD_X + FIELD_WHITE_WIDTH, GREEN_PAD_Y + FIELD_WHITE_HEIGHT);
    addCorner(vision::CornerID::Convex, LandmarkID::TheirRightConvex, GREEN_PAD_X + FIELD_WHITE_WIDTH - GOALBOX_DEPTH, BLUE_GOALBOX_BOTTOM_Y);
    addCorner(vision::CornerID::Convex, LandmarkID::TheirLeftConvex, GREEN_PAD_X + FIELD_WHITE_WIDTH - GOALBOX_DEPTH, BLUE_GOALBOX_TOP_Y);
    addCorner(vision::CornerID::T, LandmarkID::TheirRightT, GREEN_PAD_X + FIELD_WHITE_WIDTH, BLUE_GOALBOX_BOTTOM_Y);
    addCorner(vision::CornerID::T, LandmarkID::TheirLeftT, GREEN_PAD_X + FIELD_WHITE_WIDTH, BLUE_GOALBOX_TOP_Y);

    // Add ball in set to map
    double rBall, tBall;
    vision::cartesianToPolar(CENTER_FIELD_X, CENTER_FIELD_Y, rBall, tBall);
    ballInSet = std::make_tuple(LandmarkID::BallInSet, rBall, tBall);
}

Landmark LandmarkSystem::matchCorner(const messages::Corner& observation, 
                                     const messages::RobotLocation& loc)
{
    Landmark correspondingLandmark;
    double bestScore = std::numeric_limits<double>::min();

    // Turn observation into RobotLocation so scoreObservation can operate on it
    messages::RobotLocation obsvAsRobotLocation;
    obsvAsRobotLocation.set_x(observation.x());
    obsvAsRobotLocation.set_y(observation.y());

    // Loop through all corners with right id from vision and take the corner
    // that best corresponds to the observation
    vision::CornerID visionID = static_cast<vision::CornerID>(observation.id());
    const std::vector<Landmark>& possibleCorners = corners[visionID];
    for (int i = 0; i < possibleCorners.size(); i++) {
        Landmark possibleCorner = possibleCorners[i];
        double curScore = LandmarkSystem::scoreObservation(obsvAsRobotLocation, possibleCorner, loc);

        if (curScore > bestScore) {
            correspondingLandmark = possibleCorner;
            bestScore = curScore;
        }
    }

    return correspondingLandmark;
}

double LandmarkSystem::scoreCorner(const messages::Corner& observation, 
                                   const messages::RobotLocation& loc)
{
    // Turn observation into RobotLocation so scoreObservation can operate on it
    messages::RobotLocation obsvAsRobotLocation;
    obsvAsRobotLocation.set_x(observation.x());
    obsvAsRobotLocation.set_y(observation.y());

    // Find correspondence and calculate probability of match
    Landmark correspondingLandmark = matchCorner(observation, loc);
    return LandmarkSystem::scoreObservation(obsvAsRobotLocation, correspondingLandmark, loc);
}

double LandmarkSystem::scoreBallInSet(const messages::FilteredBall& observation, 
                                      const messages::RobotLocation& loc)
{
    // Polar to cartesian
    double xBall, yBall;
    vision::cartesianToPolar(observation.distance(), observation.bearing(), xBall, yBall);

    // Turn observation into RobotLocation so scoreObservation can operate on it
    messages::RobotLocation obsvAsRobotLocation;
    obsvAsRobotLocation.set_x(xBall);
    obsvAsRobotLocation.set_y(yBall);

    // Calculate probability of match
    return LandmarkSystem::scoreObservation(obsvAsRobotLocation, ballInSet, loc);
}

messages::RobotLocation LandmarkSystem::relRobotToAbsolute(const messages::RobotLocation& observation, const messages::RobotLocation& loc)
{

    // Translation rotation to absolute coordinate system
    double transX, transY;
    man::vision::translateRotate(observation.x(), observation.y(), loc.x(), loc.y(), loc.h(), transX, transY);

    // Return transformed robot location
    messages::RobotLocation transformed;
    transformed.set_x(transX);
    transformed.set_y(transY);

    return transformed;
}

double LandmarkSystem::scoreObservation(const messages::RobotLocation& observation, 
                                        const Landmark& correspondingLandmark,
                                        const messages::RobotLocation& loc)
{
    // Map to absolute coordinates
    messages::RobotLocation globalObsv = LandmarkSystem::relRobotToAbsolute(observation, loc);

    // Cartesian to polar
    double rObsv, tObsv;
    vision::cartesianToPolar(globalObsv.x(), globalObsv.y(), rObsv, tObsv);

    // Find difference in r and t
    double rDiff = fabs(fabs(std::get<1>(correspondingLandmark)) - fabs(rObsv));
    double tDiff = vision::diffRadians(std::get<2>(correspondingLandmark), tObsv);
 
    // Evaluate gaussian to get probability of observation from location loc
    // TODO params
    boost::math::normal_distribution<> rGaussian(0, 100);
    boost::math::normal_distribution<> tGaussian(0, 10*TO_RAD);

    double rProb = pdf(rGaussian, rDiff);
    double tProb = pdf(tGaussian, tDiff);

    // Make the conditional independence assumption
    return rProb * tProb;
}

void LandmarkSystem::addCorner(vision::CornerID type, LandmarkID id, double x, double y)
{
    // Cartesian to polar
    double r, t;
    vision::cartesianToPolar(x, y, r, t);

    // Add corner to map
    Landmark corner = std::make_tuple(id, r, t);
    corners[type].push_back(corner);
}

} // namespace localization
} // namespace man
