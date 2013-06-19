#include "LineSystem.h"

namespace man {
namespace localization {

LineSystem::LineSystem() {
    //Note: this is hardcoded for the SPL league layout 2013 and may need to be updated
    // Add the top and bottom lines
    addLine(FIELD_WHITE_LEFT_SIDELINE_X,  FIELD_WHITE_BOTTOM_SIDELINE_Y,
            FIELD_WHITE_RIGHT_SIDELINE_X, FIELD_WHITE_BOTTOM_SIDELINE_Y);
    addLine(FIELD_WHITE_LEFT_SIDELINE_X,  FIELD_WHITE_TOP_SIDELINE_Y,
            FIELD_WHITE_RIGHT_SIDELINE_X, FIELD_WHITE_TOP_SIDELINE_Y);

    // Add the left and right sidelines
    addLine(FIELD_WHITE_LEFT_SIDELINE_X,  FIELD_WHITE_BOTTOM_SIDELINE_Y,
            FIELD_WHITE_LEFT_SIDELINE_X,  FIELD_WHITE_TOP_SIDELINE_Y);
    addLine(FIELD_WHITE_RIGHT_SIDELINE_X,  FIELD_WHITE_BOTTOM_SIDELINE_Y,
            FIELD_WHITE_RIGHT_SIDELINE_X,  FIELD_WHITE_TOP_SIDELINE_Y);

    // Add the middle line
    addLine(CENTER_FIELD_X, FIELD_WHITE_BOTTOM_SIDELINE_Y,
            CENTER_FIELD_X, FIELD_WHITE_TOP_SIDELINE_Y);

    // Add the two goalbox lines
    addLine(BLUE_GOALBOX_RIGHT_X,  BLUE_GOALBOX_BOTTOM_Y,
            BLUE_GOALBOX_RIGHT_X,  BLUE_GOALBOX_TOP_Y);
    addLine(YELLOW_GOALBOX_LEFT_X, YELLOW_GOALBOX_BOTTOM_Y,
            YELLOW_GOALBOX_LEFT_X, YELLOW_GOALBOX_TOP_Y);
}

LineSystem::~LineSystem() {}

void LineSystem::addLine(float startX, float startY, float endX, float endY) {
    Point start(startX, startY);
    Point end  (endX  , endY  );

    lines.push_back( Line(start, end) );
}

float LineSystem::scoreObservation(Line globalObsv) {
    //For each line score the observation and return the best of them all
    float bestScore = 10000000.f;
    LineIt iter;
    for(iter = lines.begin(); iter != lines.end(); iter++) {
        float curScore = (*iter).getError(globalObsv).error;
        bestScore = ((curScore < bestScore) ? curScore : bestScore);
    }

    return bestScore;
}

LineErrorMatch LineSystem::scoreAndMatchObservation(Line globalObsv) {
    //For each line score the observation and return the best of them all
    LineErrorMatch bestMatch;
    bestMatch.error = 1000000.f;
    LineIt iter;
    for(iter = lines.begin(); iter != lines.end(); iter++) {
        LineErrorMatch curMatch = (*iter).getError(globalObsv);
        if (bestMatch.error > curMatch.error)
            bestMatch = curMatch;
    }

    return bestMatch;
}

} // namespace localization
} // namespace man
