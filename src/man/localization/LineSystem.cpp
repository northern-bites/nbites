#include "LineSystem.h"

#include <algorithm>
#include <boost/math/distributions/normal.hpp>

namespace man {
namespace localization {

LineSystem::LineSystem() 
    : lines(), visionToLocIDs(), debug(false)
{
    // Part I
    // Add lines in absolute field coordinates to lines map
    // TODO document sign convention
    addLine(LocLineID::OurEndline, -GREEN_PAD_X, M_PI, GREEN_PAD_Y, (GREEN_PAD_Y + FIELD_WHITE_HEIGHT)); 
    addLine(LocLineID::TheirEndline, GREEN_PAD_X + FIELD_WHITE_WIDTH, 0, -GREEN_PAD_Y, -(GREEN_PAD_Y + FIELD_WHITE_HEIGHT)); 

    // NOTE two midlines so that reconstructions can be handled gracefully from
    //      either side of the field
    addLine(LocLineID::TheirMidline, -CENTER_FIELD_X, M_PI, GREEN_PAD_Y, (GREEN_PAD_Y + FIELD_WHITE_HEIGHT)); 
    addLine(LocLineID::OurMidline, CENTER_FIELD_X, 0, -GREEN_PAD_Y, -(GREEN_PAD_Y + FIELD_WHITE_HEIGHT)); 

    addLine(LocLineID::OurTopGoalbox, -(GREEN_PAD_X + GOALBOX_DEPTH), M_PI, BLUE_GOALBOX_BOTTOM_Y, BLUE_GOALBOX_TOP_Y);
    addLine(LocLineID::TheirTopGoalbox, GREEN_PAD_X + FIELD_WHITE_WIDTH - GOALBOX_DEPTH , 0, -YELLOW_GOALBOX_BOTTOM_Y, -YELLOW_GOALBOX_TOP_Y);

    // IMPORTANT system currently doesn't support reconstructions from sideline, so
    //           the below lines are not polarized
    addLine(LocLineID::RightSideline, GREEN_PAD_Y, M_PI / 2, GREEN_PAD_X, GREEN_PAD_X + FIELD_WHITE_WIDTH);
    addLine(LocLineID::LeftSideline, GREEN_PAD_Y + FIELD_WHITE_HEIGHT, M_PI / 2, GREEN_PAD_X, GREEN_PAD_X + FIELD_WHITE_WIDTH);

    addLine(LocLineID::OurRightGoalbox, BLUE_GOALBOX_BOTTOM_Y, M_PI / 2, GREEN_PAD_X, GREEN_PAD_X + GOALBOX_DEPTH);
    addLine(LocLineID::TheirRightGoalbox, BLUE_GOALBOX_BOTTOM_Y, M_PI / 2, YELLOW_GOALBOX_LEFT_X, YELLOW_GOALBOX_RIGHT_X);

    // NOTE right from the perspective of an observer looking in the positive x direction
    addLine(LocLineID::OurLeftGoalbox, BLUE_GOALBOX_TOP_Y, M_PI / 2, GREEN_PAD_X, GREEN_PAD_X + GOALBOX_DEPTH);
    addLine(LocLineID::TheirLeftGoalbox, BLUE_GOALBOX_TOP_Y, M_PI / 2, YELLOW_GOALBOX_LEFT_X, YELLOW_GOALBOX_RIGHT_X);

    // Part II
    // Map LineID that vision computes to LocLineID for use in solving the 
    // correspondence problem (see matchObservation)
    std::vector<LocLineID> all { 
        LocLineID::OurEndline, LocLineID::TheirEndline,
        LocLineID::OurMidline, LocLineID::TheirMidline,
        LocLineID::OurTopGoalbox, LocLineID::TheirTopGoalbox,
        LocLineID::RightSideline, LocLineID::LeftSideline,
        LocLineID::OurRightGoalbox, LocLineID::OurLeftGoalbox,
        LocLineID::TheirRightGoalbox, LocLineID::TheirLeftGoalbox
    };
    visionToLocIDs[vision::LineID::Line] = all;

    // China 2015 hack
    // We often see T corners in center circle
    // This should be made less of a hack after competition
    std::vector<LocLineID> endlineOrSideline { 
        LocLineID::OurEndline, LocLineID::TheirEndline,
        LocLineID::OurMidline, LocLineID::TheirMidline,
        LocLineID::RightSideline, LocLineID::LeftSideline
    };
    visionToLocIDs[vision::LineID::EndlineOrSideline] = endlineOrSideline;

    std::vector<LocLineID> endlineSidelineTopGoalboxOrSideGoalbox { 
        LocLineID::OurEndline, LocLineID::TheirEndline,
        LocLineID::RightSideline, LocLineID::LeftSideline,
        LocLineID::OurTopGoalbox, LocLineID::TheirTopGoalbox,
        LocLineID::OurRightGoalbox, LocLineID::OurLeftGoalbox,
        LocLineID::TheirRightGoalbox, LocLineID::TheirLeftGoalbox
    };
    visionToLocIDs[vision::LineID::EndlineSidelineTopGoalboxOrSideGoalbox] = endlineSidelineTopGoalboxOrSideGoalbox;

    std::vector<LocLineID> topGoalboxOrSideGoalbox { 
        LocLineID::OurTopGoalbox, LocLineID::TheirTopGoalbox,
        LocLineID::OurRightGoalbox, LocLineID::OurLeftGoalbox,
        LocLineID::TheirRightGoalbox, LocLineID::TheirLeftGoalbox
    };
    visionToLocIDs[vision::LineID::TopGoalboxOrSideGoalbox] = topGoalboxOrSideGoalbox;

    std::vector<LocLineID> sideGoalboxOrMidline { 
        LocLineID::OurMidline, LocLineID::TheirMidline,
        LocLineID::OurRightGoalbox, LocLineID::OurLeftGoalbox,
        LocLineID::TheirRightGoalbox, LocLineID::TheirLeftGoalbox
    };
    visionToLocIDs[vision::LineID::SideGoalboxOrMidline] = sideGoalboxOrMidline;

    std::vector<LocLineID> sideline { 
        LocLineID::RightSideline, LocLineID::LeftSideline 
    };
    visionToLocIDs[vision::LineID::Sideline] = sideline;

    std::vector<LocLineID> sideGoalbox {
        LocLineID::OurRightGoalbox, LocLineID::OurLeftGoalbox,
        LocLineID::TheirRightGoalbox, LocLineID::TheirLeftGoalbox
    };
    visionToLocIDs[vision::LineID::SideGoalbox] = sideGoalbox;

    std::vector<LocLineID> endline { 
        LocLineID::OurEndline, LocLineID::TheirEndline 
    };
    visionToLocIDs[vision::LineID::Endline] = endline;

    std::vector<LocLineID> topGoalbox { 
        LocLineID::OurTopGoalbox, LocLineID::TheirTopGoalbox 
    };
    visionToLocIDs[vision::LineID::TopGoalbox] = topGoalbox;

    std::vector<LocLineID> midline {
        LocLineID::OurMidline, LocLineID::TheirMidline 
    };
    visionToLocIDs[vision::LineID::Midline] = midline;
}

LocLineID LineSystem::matchLine(const messages::FieldLine& observation, 
                                const messages::RobotLocation& loc)
{
    LocLineID id = LocLineID::NotMatched;
    double bestScore = 0;

    // Turn observation into GeoLine so scoreObservation can operate on it
    vision::GeoLine obsvAsGeoLine;
    obsvAsGeoLine.set(observation.inner().r(), observation.inner().t(), 
                      observation.inner().ep0(), observation.inner().ep1());

    // Loop through all corners with right id from vision and take the corner
    // that best corresponds to the observation
    // NOTE correspondence is found by maximizing probability of correspondence 
    //      according to model (see scoreObservation)
    vision::LineID visionID = static_cast<vision::LineID>(observation.id());
    const std::vector<LocLineID>& possibleLineIDs = visionToLocIDs[visionID];
    for (int i = 0; i < possibleLineIDs.size(); i++) {
        LocLineID possibleID = possibleLineIDs[i];
        double curScore = scoreObservation(obsvAsGeoLine, lines[possibleID], loc, observation.wz0());

        if (curScore > bestScore) {
            id = possibleID;
            bestScore = curScore;
        }

        if (debug) {
            std::cout << "In matchLine:" << std::endl;
            std::cout << static_cast<int>(possibleID) << std::endl;
            std::cout << curScore << std::endl;
        }
    }

    // NOTE two midlines so that reconstructions can be handled gracefully from
    //      either side of the field
    if (id == LocLineID::TheirMidline || id == LocLineID::OurMidline) {
        if (loc.x() < CENTER_FIELD_X)
            return LocLineID::OurMidline;
        return LocLineID::TheirMidline;
    }

    return id;
}

double LineSystem::scoreLine(const messages::FieldLine& observation,
                             const messages::RobotLocation& loc)
{
    // Turn observation into GeoLine so scoreObservation can operate on it
    vision::GeoLine obsvAsGeoLine;
    obsvAsGeoLine.set(observation.inner().r(), observation.inner().t(), 
                      observation.inner().ep0(), observation.inner().ep1());

    // Find correspondence and calculate probability of match
    LocLineID id = matchLine(observation, loc);
    double score = scoreObservation(obsvAsGeoLine, lines[id], loc, observation.wz0());

    if (debug) {
        std::cout << "In scoreLine:" << std::endl;
        std::cout << static_cast<int>(id) << std::endl;
        std::cout << score << std::endl;
    }

    return score;
}

// NOTE method assumes that endpoints seen in observation are endpoints of line
// IMPORTANT only tested with id == OurTopGoalbox || TheirTopGoalbox
messages::RobotLocation LineSystem::reconstructFromMidpoint(LocLineID id, 
                                                            const messages::FieldLine& observation)
{
    messages::RobotLocation position;
    const messages::HoughLine& inner = observation.inner();
    const vision::GeoLine& absolute = lines[id];

    // Calculate heading in absolute coords
    position.set_h(vision::uMod(-inner.t() + absolute.t(), 2 * M_PI));

    // Calculate midpoint of line in relative coords
    double rx1, ry1, rx2, ry2, rxm, rym;
    vision::GeoLine relRobot;
    relRobot.set(inner.r(), inner.t(), inner.ep0(), inner.ep1());
    relRobot.translateRotate(0, 0, position.h());
    relRobot.endPoints(rx1, ry1, rx2, ry2);
    rxm = (rx1 + rx2) / 2;
    rym = (ry1 + ry2) / 2;

    // Calculate midpoint of line in absolute coords
    double ax1, ay1, ax2, ay2, axm, aym;
    absolute.endPoints(ax1, ay1, ax2, ay2);
    axm = (ax1 + ax2) / 2;
    aym = (ay1 + ay2) / 2;

    // Find x and y of reconstructed position
    position.set_x(axm - rxm);
    position.set_y(aym - rym);

    return position;
}

// IMPORTANT only tested with id == OurMidline
messages::RobotLocation LineSystem::reconstructWoEndpoints(LocLineID id, 
                                                           const messages::FieldLine& observation)
{
    messages::RobotLocation position;
    const messages::HoughLine& inner = observation.inner();
    const vision::GeoLine& absolute = lines[id];

    // If cannot recover x or y, leave as -1, will be handled in VisionSystem
    position.set_x(-1);
    position.set_y(-1);

    // Calculate heading in absolute coords
    position.set_h(vision::uMod(-inner.t() + absolute.t(), 2 * M_PI));

    // Calculate x or y depending on orientation of line
    if (id == LocLineID::LeftSideline || id == LocLineID::RightSideline) {
        // Sidelines -> calculate y
        if (absolute.r() > 0)
            position.set_y(absolute.r() - inner.r());
        else
            position.set_y(-absolute.r() + inner.r());
    } else {
        // All other lines -> calculate x
        if (absolute.r() > 0)
            position.set_x(absolute.r() - inner.r());
        else
            position.set_x(-absolute.r() + inner.r());
    }

    return position;
}

bool LineSystem::shouldUse(const messages::FieldLine& observation,
                           const messages::RobotLocation& loc)
{
    // China 2015 hack
    // If loc believes we could be seeing the goalbox, score short lines
    // in localization, otherwise do not, as you are probably seeing center
    // circle lines
    double heading = vision::uMod(loc.h(), 2 * M_PI);
    bool useShorts = ((loc.x() < CENTER_FIELD_X - 150 && (heading > 150*TO_RAD && heading < 210*TO_RAD)) ||
                      (loc.x() > CENTER_FIELD_X + 150 && (heading < 150*TO_RAD && heading < 210*TO_RAD)) ||
                      (loc.x() < LANDMARK_BLUE_GOAL_CROSS_X) ||
                      (loc.x() > LANDMARK_YELLOW_GOAL_CROSS_X));
    bool length = observation.inner().ep1() - observation.inner().ep0() > 60;
    return useShorts || length;
}

vision::GeoLine LineSystem::relRobotToAbsolute(const messages::FieldLine& observation,
                                               const messages::RobotLocation& loc)
{
    const messages::HoughLine& inner = observation.inner();

    vision::GeoLine globalLine;
    globalLine.set(inner.r(), inner.t(), inner.ep0(), inner.ep1());
    globalLine.translateRotate(loc.x(), loc.y(), loc.h());

    return globalLine;
}

double LineSystem::scoreObservation(const vision::GeoLine& observation,
                                    const vision::GeoLine& correspondingLine, 
                                    const messages::RobotLocation& loc,
                                    double wz0)
{
    // Normalize correspondingLine to have positive r and t between 0 and PI / 2 
    // NOTE see constructor for explanation of what negative r means in this context
    double normalizedT, normalizedEp0, normalizedEp1;
    if (correspondingLine.r() < 0) {
        normalizedT = correspondingLine.t() - M_PI;
        normalizedEp0 = -correspondingLine.ep0();
        normalizedEp1 = -correspondingLine.ep1();
    } else {
        normalizedT = correspondingLine.t();
        normalizedEp0 = correspondingLine.ep0();
        normalizedEp1 = correspondingLine.ep1();
    }
    vision::GeoLine normalizedCorrespondingLine;
    normalizedCorrespondingLine.set(fabs(correspondingLine.r()), normalizedT,
                                    normalizedEp0, normalizedEp1);
    
    // Landmark in map, absolute to robot relative
    normalizedCorrespondingLine.inverseTranslateRotate(loc.x(), loc.y(), loc.h(), debug);

    // Calculate tilt to both observation and corresponding line in map 
    // NOTE better to score error in r in angular coordinates since this 
    //      weights close observations more highly
    double observationTilt = atan(observation.r() / wz0);
    double correspondingTilt = atan(normalizedCorrespondingLine.r() / wz0);

    // Find differences in tilt and t
    double tiltDiff = vision::diffRadians(observationTilt, correspondingTilt);
    double tDiff = vision::diffRadians(observation.t(), normalizedCorrespondingLine.t());
    
    // Find endpoint error
    // Endpoints of observation should be contained between endpoints of line in map
    // NOTE endpoints not currently being used
    double ep0Error = std::max(0.0, normalizedCorrespondingLine.ep0() - observation.ep0());
    double ep1Error = std::max(0.0, observation.ep1() - normalizedCorrespondingLine.ep1());

    // Evaluate gaussian to get probability of observation from location loc
    // TODO params
    boost::math::normal_distribution<> tiltGaussian(0, 5*TO_RAD);
    boost::math::normal_distribution<> tGaussian(0, 10*TO_RAD);
    boost::math::normal_distribution<> ep0Gaussian(0, 100);
    boost::math::normal_distribution<> ep1Gaussian(0, 100);
  
    double tiltProb = pdf(tiltGaussian, tiltDiff);
    double tProb = pdf(tGaussian, tDiff);
    double ep0Prob = pdf(ep0Gaussian, ep0Error);
    double ep1Prob = pdf(ep1Gaussian, ep1Error);
  
    if (debug) {
      std::cout << "In scoreObservation:" << std::endl;
      std::cout << normalizedCorrespondingLine.r() << "," << normalizedCorrespondingLine.t() << std::endl;
      std::cout << observation.r() << "," << observation.t() << std::endl;
      std::cout << tiltDiff << "," << tDiff << std::endl;
      std::cout << ep0Error << "," << ep1Error << std::endl;
      std::cout << tiltProb << "/" << tProb << std::endl;
      std::cout << ep0Prob << "/" << ep1Prob << std::endl;
      std::cout << (tiltProb * tProb * ep0Prob * ep1Prob) << std::endl;
    }

    // Make the conditional independence assumption
    return tiltProb * tProb;
}

void LineSystem::addLine(LocLineID id, float r, float t, float ep0, float ep1)
{
    vision::GeoLine line;
    line.set(r, t, ep0, ep1);
    lines[id] = line;
}

} // namespace localization
} // namespace man
