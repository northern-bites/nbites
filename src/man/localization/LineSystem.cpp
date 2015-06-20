#include "LineSystem.h"

#include <limits>

namespace man {
namespace localization {

LineSystem::LineSystem() 
    : lines()
{
    // TODO update for reconstruct
    // Add endlines
    addLine(LocLineID::OurEndline, GREEN_PAD_X, 0, GREEN_PAD_Y, GREEN_PAD_Y + FIELD_WHITE_HEIGHT); 
    addLine(LocLineID::TheirEndline, GREEN_PAD_X + FIELD_WHITE_WIDTH, 0, GREEN_PAD_Y, GREEN_PAD_Y + FIELD_WHITE_HEIGHT); 

    // Add the midline
    addLine(LocLineID::Midline, CENTER_FIELD_X, 0, GREEN_PAD_Y, GREEN_PAD_Y + FIELD_WHITE_HEIGHT); 

    // Add top goalbox lines
    addLine(LocLineID::OurTopGoalbox, -(GREEN_PAD_X + GOALBOX_DEPTH) , M_PI, BLUE_GOALBOX_BOTTOM_Y, BLUE_GOALBOX_TOP_Y);
    addLine(LocLineID::TheirTopGoalbox, GREEN_PAD_X + FIELD_WHITE_WIDTH - GOALBOX_DEPTH , 0, YELLOW_GOALBOX_BOTTOM_Y, YELLOW_GOALBOX_TOP_Y);

    // Add sidelines
    addLine(LocLineID::RightSideline, GREEN_PAD_Y, M_PI / 2, GREEN_PAD_X, GREEN_PAD_X + FIELD_WHITE_WIDTH);
    addLine(LocLineID::LeftSideline, GREEN_PAD_Y + FIELD_WHITE_HEIGHT, M_PI / 2, GREEN_PAD_X, GREEN_PAD_X + FIELD_WHITE_WIDTH);
}

LineSystem::~LineSystem() {}

// TODO check line overlap
LocLineID LineSystem::matchObservation(const messages::FieldLine& observation, 
                                       const messages::RobotLocation& loc)
{
    vision::GeoLine globalLine = LineSystem::relRobotToAbsolute(observation, loc);

    LocLineID id = LocLineID::NotMatched;
    double bestScore = std::numeric_limits<double>::max();

    for (auto it = lines.begin(); it != lines.end(); it++) {
        double curScore = it->second.error(globalLine);
        if (curScore < bestScore) {
            id = it->first;
            bestScore = curScore;
        }
    }

    return id;
}

double LineSystem::scoreObservation(const messages::FieldLine& observation,
                                    const messages::RobotLocation& loc)
{
    vision::GeoLine globalLine = LineSystem::relRobotToAbsolute(observation, loc);

    double errorBetweenObservationAndModel;
    LocLineID id = matchObservation(observation, loc);
    if (id == LocLineID::NotMatched)
        errorBetweenObservationAndModel = 1;
    else
        errorBetweenObservationAndModel = lines[id].error(globalLine);

    double r = observation.inner().r();
    return (1 / r) * errorBetweenObservationAndModel;
}

// NOTE method assumes that endpoints seen in observation are endpoints of line
messages::RobotLocation LineSystem::reconstructPosition(LocLineID id, 
                                                        const messages::FieldLine& observation)
{
    messages::RobotLocation position;
    const messages::HoughLine& inner = observation.inner();
    const vision::GeoLine& absolute = lines[id];

    // Calculate heading in absolute coords
    position.set_h(vision::addRadians((M_PI / 2) - inner.t(), absolute.t()));

    // Calculate midpoint of line in relative coords
    double rx1, ry1, rx2, ry2, rxm, rym;
    vision::GeoLine relRobot;
    relRobot.set(inner.r(), inner.t(), inner.ep0(), inner.ep1());
    relRobot.translateRotate(0, 0, -(M_PI / 2));
    relRobot.translateRotate(0, 0, position.h());
    relRobot.endPoints(rx1, ry1, rx2, ry2);
    rxm = (rx1 + rx2) / 2;
    rym = (ry1 + ry2) / 2;
    std::cout << "rxm: " << rxm << std::endl;
    std::cout << "rym: " << rym << std::endl;

    // Calculate midpoint of line in absolute coords
    double ax1, ay1, ax2, ay2, axm, aym;
    absolute.endPoints(ax1, ay1, ax2, ay2);
    std::cout << "ax1: " << ax1 << std::endl;
    std::cout << "ay1: " << ay1 << std::endl;
    std::cout << "ax2: " << ax2 << std::endl;
    std::cout << "ay2: " << ay2 << std::endl;
    axm = (ax1 + ax2) / 2;
    aym = (ay1 + ay2) / 2;
    std::cout << "axm: " << axm << std::endl;
    std::cout << "aym: " << aym << std::endl;

    // Find x and y of reconstructed position
    position.set_x(axm - rxm);
    position.set_y(aym - rym);

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
