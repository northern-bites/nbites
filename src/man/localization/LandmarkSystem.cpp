#include "LandmarkSystem.h"

#include <limits>
#include <boost/math/distributions/normal.hpp>

namespace man {
namespace localization {

LandmarkSystem::LandmarkSystem() 
    : corners(), ballInSet(), debug(false)
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
    ballInSet = std::make_tuple(LandmarkID::BallInSet, CENTER_FIELD_X, CENTER_FIELD_Y);
}

Landmark LandmarkSystem::matchCorner(const messages::Corner& observation, 
                                     const messages::RobotLocation& loc)
{
    Landmark correspondingLandmark;
    double closestDist = std::numeric_limits<double>::max();

    // Transform observation to absolute coordinate system
    messages::RobotLocation obsvAsRobotLocation;
    obsvAsRobotLocation.set_x(observation.x());
    obsvAsRobotLocation.set_y(observation.y());
    messages::RobotLocation obsvAbs = LandmarkSystem::relRobotToAbsolute(obsvAsRobotLocation, loc);

    // Loop through all corners with right id from vision and take the corner
    // that best corresponds to the observation
    // NOTE correspondence is found by minimizing euclidean distance
    vision::CornerID visionID = static_cast<vision::CornerID>(observation.id());
    const std::vector<Landmark>& possibleCorners = corners[visionID];
    for (int i = 0; i < possibleCorners.size(); i++) {
        Landmark possibleCorner = possibleCorners[i];
        double dist = vision::dist(obsvAbs.x(), obsvAbs.y(),
                                   std::get<1>(possibleCorner), std::get<2>(possibleCorner));

        if (closestDist > dist) {
            correspondingLandmark = possibleCorner;
            closestDist = dist;
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
    return scoreObservation(obsvAsRobotLocation, correspondingLandmark, loc, true);
    // NOTE for corners, only bearing is used to score particles
}

double LandmarkSystem::scoreBallInSet(const messages::FilteredBall& observation, 
                                      const messages::RobotLocation& loc)
{
    // Polar to cartesian
    double xBall, yBall;
    vision::polarToCartesian(observation.distance(), observation.bearing(), xBall, yBall);

    // Turn observation into RobotLocation so scoreObservation can operate on it
    messages::RobotLocation obsvAsRobotLocation;
    obsvAsRobotLocation.set_x(xBall);
    obsvAsRobotLocation.set_y(yBall);

    // Calculate probability of match
    return scoreObservation(obsvAsRobotLocation, ballInSet, loc);
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
                                        const messages::RobotLocation& loc,
                                        bool onlyBearing)
{
    // Observation, cartesian to polar
    double rObsv, tObsv;
    vision::cartesianToPolar(observation.x(), observation.y(), rObsv, tObsv);

    // Landmark in map, absolute to robot relative
    double xMapRel, yMapRel;
    vision::inverseTranslateRotate(std::get<1>(correspondingLandmark), 
                                   std::get<2>(correspondingLandmark),
                                   loc.x(), loc.y(), loc.h(), xMapRel, yMapRel);

    // Landmark in map, cartesian to polar
    double rMapRel, tMapRel;
    vision::cartesianToPolar(xMapRel, yMapRel, rMapRel, tMapRel);

    // Find difference in r and t
    double rDiff = fabs(rMapRel - rObsv);
    double tDiff = vision::diffRadians(tMapRel, tObsv);
 
    // Evaluate gaussian to get probability of observation from location loc
    // TODO params
    boost::math::normal_distribution<> rGaussian(0, 100);
    boost::math::normal_distribution<> tGaussian(0, 10*TO_RAD);

    double rProb = pdf(rGaussian, rDiff);
    double tProb = pdf(tGaussian, tDiff);

    if (debug) {
        std::cout << "In scoreObservation:" << std::endl;
        std::cout << observation.x() << "," << observation.y() << std::endl;
        std::cout << rObsv << "," << tObsv << std::endl;
        std::cout << std::get<1>(correspondingLandmark) << "," << std::get<2>(correspondingLandmark) << std::endl;
        std::cout << xMapRel << "," << yMapRel << std::endl;
        std::cout << rMapRel << "," << tMapRel << std::endl;
        std::cout << rDiff << "," << tDiff << std::endl;
        std::cout << rProb << "/" << tProb << std::endl;
        std::cout << (rProb * tProb) << std::endl;
    }

    // Make the conditional independence assumption
    return rProb * tProb;
}

void LandmarkSystem::addCorner(vision::CornerID type, LandmarkID id, double x, double y)
{
    Landmark corner = std::make_tuple(id, x, y);
    corners[type].push_back(corner);
}

} // namespace localization
} // namespace man
