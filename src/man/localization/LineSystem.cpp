#include "LineSystem.h"

#include <limits>

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

LineSystem::~LineSystem() {}

LocLineID LineSystem::matchObservation(const messages::FieldLine& observation, 
                                       const messages::RobotLocation& loc)
{
    vision::GeoLine globalLine = LineSystem::relRobotToAbsolute(observation, loc);

    LocLineID id = LocLineID::NotMatched;
    double bestScore = std::numeric_limits<double>::min();

    vision::LineID visionID = static_cast<vision::LineID>(observation.id());
    const std::vector<LocLineID>& possibleLineIDs = visionToLocIDs[visionID];
    for (int i = 0; i < possibleLineIDs.size(); i++) {
        LocLineID possibleID = possibleLineIDs[i];
        double curScore = lines[possibleID].error(globalLine, debug);

        if (debug)
            std::cout << "Match, " << static_cast<int>(possibleID) << "," << curScore << "/" << bestScore << std::endl; 

        if (curScore > bestScore) {
            id = possibleID;
            bestScore = curScore;
        }
    }

    if (id == LocLineID::TheirMidline || id == LocLineID::OurMidline) {
        if (loc.x() < CENTER_FIELD_X)
            return LocLineID::OurMidline;
        return LocLineID::TheirMidline;
    }

    return id;
}

// TODO rename to prob
double LineSystem::scoreObservation(const messages::FieldLine& observation,
                                    const messages::RobotLocation& loc)
{
    vision::GeoLine globalLine = LineSystem::relRobotToAbsolute(observation, loc);

    double errorBetweenObservationAndModel;
    LocLineID id = matchObservation(observation, loc);

    if (id == LocLineID::NotMatched)
        errorBetweenObservationAndModel = 0;
    else
        errorBetweenObservationAndModel = lines[id].error(globalLine, debug);

    if (debug) { 
        std::cout << "In scoreObservation," << std::endl;
        std::cout << static_cast<int>(id) << std::endl;
        std::cout << errorBetweenObservationAndModel << std::endl;
    }

    return errorBetweenObservationAndModel;
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

vision::GeoLine LineSystem::relRobotToAbsolute(const messages::FieldLine& observation,
                                               const messages::RobotLocation& loc)
{
    const messages::HoughLine& inner = observation.inner();

    vision::GeoLine globalLine;
    globalLine.set(inner.r(), inner.t(), inner.ep0(), inner.ep1());
    globalLine.translateRotate(0, 0, -(M_PI / 2));
    globalLine.translateRotate(loc.x(), loc.y(), loc.h());

    return globalLine;
}

// TODO parameters
bool LineSystem::shouldUse(const messages::FieldLine& observation)
{
    const messages::HoughLine& inner = observation.inner();
    bool longEnough = inner.ep1() - inner.ep0() > 60;
    return longEnough;
}

void LineSystem::addLine(LocLineID id, float r, float t, float ep0, float ep1)
{
    vision::GeoLine line;
    line.set(r, t, ep0, ep1);
    lines[id] = line;
}

} // namespace localization
} // namespace man
