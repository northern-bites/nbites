#include "LineSystem.h"

#include <limits>

namespace man {
namespace localization {

// TODO add side goalbox lines
// TODO line classification
// TODO endpoint detection and corners
// TODO rename Vision.pb.h to VisionField.pb.h
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
                                    const Particle& particle)
{
    vision::GeoLine globalLine = fromRelRobotToGlobal(observation, particle);
    int bestLine = -1;

    double bestScore = std::numeric_limits<double>::max();
    for (int i = 0; i < lines.size(); i++) {
        double curScore = lines[i].error(globalLine);
        if (curScore < bestScore) {
            bestScore = curScore;
            bestLine = i;
        }
    }

    if (bestScore >= 1)
        std::cout << "NO MATCHING LINE FOUND, " << bestScore << std::endl;
    else
        std::cout << "BEST LINE, " << bestLine << std::endl;

    return bestScore;
}

void LineSystem::addLine(float r, float t, float ep0, float ep1)
{
    vision::GeoLine line;
    line.set(r, t, ep0, ep1);
    lines.push_back(line);
}

vision::GeoLine LineSystem::fromRelRobotToGlobal(const messages::FieldLine& relRobotLine,
                                                 const Particle& particle) const
{
    const messages::HoughLine& relRobotInner = relRobotLine.inner();
    const messages::RobotLocation& loc = particle.getLocation();

    vision::GeoLine globalLine;
    globalLine.set(relRobotInner.r(), relRobotInner.t(), relRobotInner.ep0(), relRobotInner.ep1());
    globalLine.translateRotate(loc.x(), loc.y(), loc.h() - (M_PI / 2));

    return globalLine;
}

} // namespace localization
} // namespace man
