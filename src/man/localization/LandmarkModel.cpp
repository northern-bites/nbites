#include "LandmarkModel.h"

#include <limits>
#include <boost/math/distributions/normal.hpp>

namespace man {
namespace localization {

LandmarkModel::LandmarkModel(const struct ParticleFilterParams& params_) 
    : params(params_), corners(), ballInSet(), debug(false)
{
    // Construct map
    // Init corner map
    corners[vision::CornerID::Concave] = std::vector<Landmark>();
    corners[vision::CornerID::Convex] = std::vector<Landmark>();
    corners[vision::CornerID::T] = std::vector<Landmark>();

    // Add corners to map
    addCorner(vision::CornerID::Concave, LandmarkID::OurRightConcave, GREEN_PAD_X, GREEN_PAD_Y);
    addCorner(vision::CornerID::Concave, LandmarkID::OurLeftConcave, GREEN_PAD_X, GREEN_PAD_Y + FIELD_WHITE_HEIGHT);
    addCorner(vision::CornerID::Concave, LandmarkID::OurRightBox, GREEN_PAD_X + GOALBOX_DEPTH, BLUE_GOALBOX_BOTTOM_Y);
    addCorner(vision::CornerID::Concave, LandmarkID::OurLeftBox, GREEN_PAD_X + GOALBOX_DEPTH, BLUE_GOALBOX_TOP_Y);
    addCorner(vision::CornerID::Convex, LandmarkID::OurRightBox, GREEN_PAD_X + GOALBOX_DEPTH, BLUE_GOALBOX_BOTTOM_Y);
    addCorner(vision::CornerID::Convex, LandmarkID::OurLeftBox, GREEN_PAD_X + GOALBOX_DEPTH, BLUE_GOALBOX_TOP_Y);
    addCorner(vision::CornerID::T, LandmarkID::OurRightT, GREEN_PAD_X, BLUE_GOALBOX_BOTTOM_Y);
    addCorner(vision::CornerID::T, LandmarkID::OurLeftT, GREEN_PAD_X, BLUE_GOALBOX_TOP_Y);
    addCorner(vision::CornerID::T, LandmarkID::MidRightT, CENTER_FIELD_X, GREEN_PAD_Y);
    addCorner(vision::CornerID::T, LandmarkID::MidLeftT, CENTER_FIELD_X, GREEN_PAD_Y + FIELD_WHITE_HEIGHT);
    addCorner(vision::CornerID::Concave, LandmarkID::TheirRightConcave, GREEN_PAD_X + FIELD_WHITE_WIDTH, GREEN_PAD_Y);
    addCorner(vision::CornerID::Concave, LandmarkID::TheirLeftConcave, GREEN_PAD_X + FIELD_WHITE_WIDTH, GREEN_PAD_Y + FIELD_WHITE_HEIGHT);
    addCorner(vision::CornerID::Concave, LandmarkID::TheirRightBox, GREEN_PAD_X + FIELD_WHITE_WIDTH - GOALBOX_DEPTH, BLUE_GOALBOX_BOTTOM_Y);
    addCorner(vision::CornerID::Concave, LandmarkID::TheirLeftBox, GREEN_PAD_X + FIELD_WHITE_WIDTH - GOALBOX_DEPTH, BLUE_GOALBOX_TOP_Y);
    addCorner(vision::CornerID::Convex, LandmarkID::TheirRightBox, GREEN_PAD_X + FIELD_WHITE_WIDTH - GOALBOX_DEPTH, BLUE_GOALBOX_BOTTOM_Y);
    addCorner(vision::CornerID::Convex, LandmarkID::TheirLeftBox, GREEN_PAD_X + FIELD_WHITE_WIDTH - GOALBOX_DEPTH, BLUE_GOALBOX_TOP_Y);
    addCorner(vision::CornerID::T, LandmarkID::TheirRightT, GREEN_PAD_X + FIELD_WHITE_WIDTH, BLUE_GOALBOX_BOTTOM_Y);
    addCorner(vision::CornerID::T, LandmarkID::TheirLeftT, GREEN_PAD_X + FIELD_WHITE_WIDTH, BLUE_GOALBOX_TOP_Y);
    addCorner(vision::CornerID::T, LandmarkID::CenterCircleLeftT, CENTER_FIELD_X, CENTER_FIELD_Y + CENTER_CIRCLE_RADIUS);
    addCorner(vision::CornerID::T, LandmarkID::CenterCircleRightT, CENTER_FIELD_X, CENTER_FIELD_Y - CENTER_CIRCLE_RADIUS);

    // Add center circle to map
    circle = std::make_tuple(LandmarkID::CenterCircle, CENTER_FIELD_X, CENTER_FIELD_Y);

    // Add ball in set to map
    ballInSet = std::make_tuple(LandmarkID::BallInSet, CENTER_FIELD_X, CENTER_FIELD_Y);
}

Landmark LandmarkModel::matchCorner(const messages::Corner& observation, 
                                    const messages::RobotLocation& loc)
{
    Landmark correspondingLandmark;
    double closestDist = std::numeric_limits<double>::max();

    // Transform observation to absolute coordinate system
    messages::RobotLocation obsvAsRobotLocation;
    obsvAsRobotLocation.set_x(observation.x());
    obsvAsRobotLocation.set_y(observation.y());
    messages::RobotLocation obsvAbs = LandmarkModel::relRobotToAbsolute(obsvAsRobotLocation, loc);

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

double LandmarkModel::scoreCorner(const messages::Corner& observation, 
                                  const messages::RobotLocation& loc)
{
    // Turn observation into RobotLocation so scoreObservation can operate on it
    messages::RobotLocation obsvAsRobotLocation;
    obsvAsRobotLocation.set_x(observation.x());
    obsvAsRobotLocation.set_y(observation.y());

    // Find correspondence and calculate probability of match
    Landmark correspondingLandmark = matchCorner(observation, loc);
    return scoreObservation(obsvAsRobotLocation, correspondingLandmark, loc, observation.wz0());
}

double LandmarkModel::scoreCircle(const messages::CenterCircle& observation, 
                                  const messages::RobotLocation& loc)
{
    // Turn observation into RobotLocation so scoreObservation can operate on it
    messages::RobotLocation obsvAsRobotLocation;
    obsvAsRobotLocation.set_x(observation.x());
    obsvAsRobotLocation.set_y(observation.y());

    // Calculate probability of match
    return scoreObservation(obsvAsRobotLocation, circle, loc, observation.wz0());
}

double LandmarkModel::scoreBallInSet(const messages::FilteredBall& observation, 
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
    return scoreObservation(obsvAsRobotLocation, ballInSet, loc, observation.vis().wz0());
}

messages::RobotLocation LandmarkModel::relRobotToAbsolute(const messages::RobotLocation& observation, const messages::RobotLocation& loc)
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

// FUTURE WORK, currently we use the same gaussians to model error for corners
//              and center circle, since the detector are different it may be 
//              better to have different params for the two features, the tradeoff
//              is that then there are more params to optimize
double LandmarkModel::scoreObservation(const messages::RobotLocation& observation, 
                                       const Landmark& correspondingLandmark,
                                       const messages::RobotLocation& loc,
                                       double wz0)
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

    // Calculate tilt to both observation and corresponding line in map 
    // NOTE better to score error in r in angular coordinates since this 
    //      weights close observations more highly
    double observationTilt = atan(rObsv / wz0);
    double correspondingTilt = atan(rMapRel / wz0);

    // Find differences in tilt and t
    double tiltDiff = vision::diffRadians(observationTilt, correspondingTilt);
    double tDiff = vision::diffRadians(tMapRel, tObsv);
 
    // Evaluate gaussian to get probability of observation from location loc
    boost::math::normal_distribution<> tiltGaussian(0, params.landmarkTiltStdev);
    boost::math::normal_distribution<> tGaussian(0, params.landmarkBearingStdev);

    double tiltProb = pdf(tiltGaussian, tiltDiff);
    double tProb = pdf(tGaussian, tDiff);

    if (debug) {
        std::cout << "In scoreObservation:" << std::endl;
        std::cout << observation.x() << "," << observation.y() << std::endl;
        std::cout << rObsv << "," << tObsv << std::endl;
        std::cout << std::get<1>(correspondingLandmark) << "," << std::get<2>(correspondingLandmark) << std::endl;
        std::cout << xMapRel << "," << yMapRel << std::endl;
        std::cout << rMapRel << "," << tMapRel << std::endl;
        std::cout << tiltDiff << "," << tDiff << std::endl;
        std::cout << tiltProb << "/" << tProb << std::endl;
        std::cout << (tiltProb * tProb) << std::endl;
    }

    // Make the conditional independence assumption
    return tiltProb * tProb;

    // FUTURE WORK, also model uncertainity in classification, uncertainity may
    //              vary as a function of landmark id, for example, the center circle
    //              may be easier to classify than corners
}

void LandmarkModel::addCorner(vision::CornerID type, LandmarkID id, double x, double y)
{
    Landmark corner = std::make_tuple(id, x, y);
    corners[type].push_back(corner);
}

} // namespace localization
} // namespace man
