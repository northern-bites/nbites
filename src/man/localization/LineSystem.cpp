#include "LineSystem.h"

#include <limits>

namespace man {
namespace localization {

LineSystem::LineSystem() 
{
    // Add endlines
    addLine(GREEN_PAD_X, 0, GREEN_PAD_Y, GREEN_PAD_Y + FIELD_WHITE_HEIGHT); 
    addLine(GREEN_PAD_X + FIELD_WHITE_WIDTH, 0, GREEN_PAD_Y, GREEN_PAD_Y + FIELD_WHITE_HEIGHT); 

    // Add the midline
    addLine(CENTER_FIELD_X, 0, GREEN_PAD_Y, GREEN_PAD_Y + FIELD_WHITE_HEIGHT); 

    // Add top goalbox lines
    addLine(GREEN_PAD_X + GOALBOX_DEPTH , 0, BLUE_GOALBOX_BOTTOM_Y, BLUE_GOALBOX_TOP_Y);
    addLine(GREEN_PAD_X + GOALBOX_DEPTH , 0, YELLOW_GOALBOX_BOTTOM_Y, YELLOW_GOALBOX_TOP_Y);

    // Add sidelines
    addLine(GREEN_PAD_Y, M_PI / 2, GREEN_PAD_X, GREEN_PAD_X + FIELD_WHITE_WIDTH);
    addLine(GREEN_PAD_Y + FIELD_WHITE_HEIGHT, M_PI / 2, GREEN_PAD_X, GREEN_PAD_X + FIELD_WHITE_WIDTH);
}

LineSystem::~LineSystem() {}

double LineSystem::scoreObservation(const messages::FieldLine& observation,
                                    const messages::RobotLocation& loc)
{
    vision::GeoLine globalLine = LineSystem::relRobotToAbsolute(observation, loc);

    double bestScore = std::numeric_limits<double>::max();
    for (int i = 0; i < lines.size(); i++) {
        double curScore = lines[i].error(globalLine);
        if (curScore < bestScore)
            bestScore = curScore;
    }

    double r = observation.inner().r();
    return (1 / r) * bestScore;
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

bool LineSystem::shouldUse(const messages::FieldLine& observation)
{
    const messages::HoughLine& inner = observation.inner();
    bool longEnough = inner.ep1() - inner.ep0() > 60;
    return longEnough;
}

void LineSystem::addLine(float r, float t, float ep0, float ep1)
{
    vision::GeoLine line;
    line.set(r, t, ep0, ep1);
    lines.push_back(line);
}

} // namespace localization
} // namespace man
