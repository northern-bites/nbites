#include "LineSystem.h"

#include <boost/math/distributions/normal.hpp>

namespace man {
namespace localization {

LineSystem::LineSystem() 
    : lines(), visionToLocIDs(), debug(false)
{
    // Part I
    // Add lines in absolute field coordinates to lines map
    // TODO document sign conventions
    addLine(LocLineID::OurEndline, -GREEN_PAD_X, M_PI, GREEN_PAD_Y, GREEN_PAD_Y + FIELD_WHITE_HEIGHT); 
    addLine(LocLineID::TheirEndline, GREEN_PAD_X + FIELD_WHITE_WIDTH, 0, -GREEN_PAD_Y, -(GREEN_PAD_Y + FIELD_WHITE_HEIGHT)); 

    // NOTE two midlines so that reconstructions can be handled gracefully from
    //      either side of the field
    addLine(LocLineID::TheirMidline, -CENTER_FIELD_X, M_PI, GREEN_PAD_Y, GREEN_PAD_Y + FIELD_WHITE_HEIGHT); 
    addLine(LocLineID::OurMidline, CENTER_FIELD_X, 0, -GREEN_PAD_Y, -(GREEN_PAD_Y + FIELD_WHITE_HEIGHT)); 

    addLine(LocLineID::OurTopGoalbox, -(GREEN_PAD_X + GOALBOX_DEPTH) , M_PI, BLUE_GOALBOX_BOTTOM_Y, BLUE_GOALBOX_TOP_Y);
    addLine(LocLineID::TheirTopGoalbox, GREEN_PAD_X + FIELD_WHITE_WIDTH - GOALBOX_DEPTH , 0, -YELLOW_GOALBOX_BOTTOM_Y, -YELLOW_GOALBOX_TOP_Y);

    addLine(LocLineID::RightSideline, -GREEN_PAD_Y, 3 * M_PI / 2, GREEN_PAD_X, GREEN_PAD_X + FIELD_WHITE_WIDTH);
    addLine(LocLineID::LeftSideline, GREEN_PAD_Y + FIELD_WHITE_HEIGHT, M_PI / 2, GREEN_PAD_X, GREEN_PAD_X + FIELD_WHITE_WIDTH);

    // Part II
    // Map LineID that vision computes to LocLineID for use in solving the 
    // correspondence problem (see matchObservation)
    std::vector<LocLineID> all { 
        LocLineID::OurEndline, LocLineID::TheirEndline,
        LocLineID::OurMidline, LocLineID::TheirMidline,
        LocLineID::OurTopGoalbox, LocLineID::TheirTopGoalbox,
        LocLineID::RightSideline, LocLineID::LeftSideline 
    };
    visionToLocIDs[vision::LineID::Line] = all;

    std::vector<LocLineID> endlineOrSideline { 
        LocLineID::OurEndline, LocLineID::TheirEndline,
        LocLineID::RightSideline, LocLineID::LeftSideline 
    };
    visionToLocIDs[vision::LineID::EndlineOrSideline] = endlineOrSideline;

    std::vector<LocLineID> topGoalboxOrSideGoalbox { 
        LocLineID::OurTopGoalbox, LocLineID::TheirTopGoalbox
    };
    visionToLocIDs[vision::LineID::TopGoalboxOrSideGoalbox] = topGoalboxOrSideGoalbox;

    std::vector<LocLineID> sideGoalboxOrMidline { 
        LocLineID::OurMidline, LocLineID::TheirMidline
    };
    visionToLocIDs[vision::LineID::SideGoalboxOrMidline] = sideGoalboxOrMidline;

    std::vector<LocLineID> sideline { 
        LocLineID::RightSideline, LocLineID::LeftSideline 
    };
    visionToLocIDs[vision::LineID::Sideline] = sideline;

    std::vector<LocLineID> sideGoalbox {};
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
    vision::LineID visionID = vision::LineID::Line;
    const std::vector<LocLineID>& possibleLineIDs = visionToLocIDs[visionID];
    for (int i = 0; i < possibleLineIDs.size(); i++) {
        LocLineID possibleID = possibleLineIDs[i];
        double curScore = scoreObservation(obsvAsGeoLine, lines[id], loc);

        if (curScore > bestScore) {
            id = possibleID;
            bestScore = curScore;
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
    return scoreObservation(obsvAsGeoLine, lines[id], loc);
}

// NOTE method assumes that endpoints seen in observation are endpoints of line
messages::RobotLocation LineSystem::reconstructFromMidpoint(LocLineID id, 
                                                            const messages::FieldLine& observation)
{
    messages::RobotLocation position;
    const messages::HoughLine& inner = observation.inner();
    const vision::GeoLine& absolute = lines[id];

    // Calculate heading in absolute coords
    position.set_h(vision::uMod((M_PI / 2) - inner.t() + absolute.t(), 2 * M_PI));

    // Calculate midpoint of line in relative coords
    double rx1, ry1, rx2, ry2, rxm, rym;
    vision::GeoLine relRobot;
    relRobot.set(inner.r(), inner.t(), inner.ep0(), inner.ep1());
    relRobot.translateRotate(0, 0, -(M_PI / 2));
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
    position.set_h(vision::uMod((M_PI / 2) - inner.t() + absolute.t(), 2 * M_PI));

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

bool LineSystem::shouldUse(const messages::FieldLine& observation)
{
    const messages::HoughLine& inner = observation.inner();
    bool longEnough = inner.ep1() - inner.ep0() > 60;
    return longEnough;
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

bool LineSystem::scoreObservation(const vision::GeoLine& observation, 
                                  const vision::GeoLine& correspondingLine, 
                                  const messages::RobotLocation& loc)
{
    // Normalize correspondingLine to have positive r and t between 0 and PI / 2 
    // NOTE see constructor for explanation of what negative r means in this context
    double normalizedT = (correspondingLine.r() > 0 ? correspondingLine.t() : correspondingLine.t() - M_PI);
    vision::GeoLine normalizedCorrespondingLine;
    normalizedCorrespondingLine.set(fabs(correspondingLine.r()), normalizedT, 
                                         correspondingLine.ep0(), correspondingLine.ep1());
    
    // Landmark in map, absolute to robot relative
    normalizedCorrespondingLine.inverseTranslateRotate(loc.x(), loc.y(), loc.h());

    // Find differences in r and t
    // NOTE must normalize T
    double rDiff = fabs(observation.r() - normalizedCorrespondingLine.r());
    double tDiff = vision::diffRadians(observation.t(), normalizedCorrespondingLine.t());

    // Evaluate gaussian to get probability of observation from location loc
    // TODO params
    boost::math::normal_distribution<> rGaussian(0, 100);
    boost::math::normal_distribution<> tGaussian(0, 10*TO_RAD);
  
    double rProb = pdf(rGaussian, rDiff);
    double tProb = pdf(tGaussian, tDiff);
  
    if (debug) {
      std::cout << "Scoring line:" << std::endl;
      std::cout << rDiff << "," << tDiff << std::endl;
      std::cout << rProb << "/" << tProb << std::endl;
    }

    // Make the conditional independence assumption
    return rProb * tProb;
}

void LineSystem::addLine(LocLineID id, float r, float t, float ep0, float ep1)
{
    vision::GeoLine line;
    line.set(r, t, ep0, ep1);
    lines[id] = line;
}

} // namespace localization
} // namespace man
