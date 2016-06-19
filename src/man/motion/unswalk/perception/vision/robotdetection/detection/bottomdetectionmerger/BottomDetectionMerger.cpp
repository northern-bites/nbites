#include "BottomDetectionMerger.hpp"

#include "perception/vision/robotdetection/analysis/robotmerger/RobotMerger.hpp"



/**
 * Tests whether two ranges of X values are within each other/overlap at some point.
 */
bool horizontallyWithin(int aLeftX, int aRightX, int bLeftX, int bRightX) {
    return ((bLeftX <= aLeftX && aLeftX <= bRightX) ||
            (bLeftX <= aRightX && aRightX <= bRightX) ||
            (aLeftX <= bLeftX && bRightX <= aRightX));
}

bool BottomDetectionMerger::horizontallyOverlap(PossibleRobot &topRobot, PossibleRobot &botRobot) {
    const int BOT_RESOLUTION_MULTIPLIER = TOP_SALIENCY_COLS / BOT_SALIENCY_COLS;

    const int botLeftX = BOT_RESOLUTION_MULTIPLIER * botRobot.region.a.x();
    const int botRightX = BOT_RESOLUTION_MULTIPLIER * botRobot.region.b.x();

    const int topLeftX = topRobot.region.a.x();
    const int topRightX = topRobot.region.b.x();

    return ((botLeftX <= topLeftX && topLeftX <= botRightX) ||
            (botLeftX <= topRightX && topRightX <= botRightX) ||
            (topLeftX <= botLeftX && botRightX <= topRightX) ||
            (botLeftX <= topLeftX && topRightX <= botRightX));
}

void mergeDetectedRobots(VisionFrame &frame, const Fovea &topSaliency,
            const Fovea &botSaliency,
            std::vector<PossibleRobot> &topPossibleRobots,
            std::vector<PossibleRobot> &botPossibleRobots) {


    const int BOT_RESOLUTION_MULTIPLIER = TOP_SALIENCY_COLS / BOT_SALIENCY_COLS;

    for (unsigned int topIndex = 0; topIndex < topPossibleRobots.size(); ++topIndex) {
        PossibleRobot &topRobot = topPossibleRobots[topIndex];
        const bool topHasLeft = (topIndex > 0);
        const bool topHasRight = (topIndex + 1 < topPossibleRobots.size());

        for (unsigned int botIndex = 0; botIndex < botPossibleRobots.size(); ++botIndex) {
            PossibleRobot &botRobot = botPossibleRobots[botIndex];
            const bool botHasLeft = (botIndex > 0);
            const bool botHasRight = (botIndex + 1 < botPossibleRobots.size());
            //if they overlap expand either one to the max.
            if (BottomDetectionMerger::horizontallyOverlap(topRobot, botRobot)) {

                int leftX = MIN(topRobot.region.a.x(), botRobot.region.a.x() * BOT_RESOLUTION_MULTIPLIER);
                int rightX = MAX(topRobot.region.b.x(), botRobot.region.b.x() * BOT_RESOLUTION_MULTIPLIER);


                //if this overlaps right or left.
                if (topHasLeft && (leftX < topPossibleRobots[topIndex - 1].region.b.x())) {
                    topRobot.region.a.x() = topPossibleRobots[topIndex - 1].region.b.x();
                } else {
                    topRobot.region.a.x() = leftX;
                }
                if (topHasRight && (rightX > topPossibleRobots[topIndex + 1].region.a.x())) {
                    topRobot.region.b.x() = topPossibleRobots[topIndex + 1].region.a.x();
                } else {
                    topRobot.region.b.x() = rightX;
                }

                if (topRobot.feet.distance() < botRobot.feet.distance()) {
                    topRobot.feet = botRobot.feet;
                }

                leftX /= BOT_RESOLUTION_MULTIPLIER;
                rightX /= BOT_RESOLUTION_MULTIPLIER;

                //if this overlaps right or left.
                if (botHasLeft && (leftX < botPossibleRobots[botIndex - 1].region.b.x())) {
                    botRobot.region.a.x() = botPossibleRobots[botIndex - 1].region.b.x();
                } else {
                    botRobot.region.a.x() = leftX;
                }
                if (botHasRight && (rightX > botPossibleRobots[botIndex + 1].region.a.x())) {
                    botRobot.region.b.x() = botPossibleRobots[botIndex + 1].region.a.x();
                } else {
                    botRobot.region.b.x() = rightX;
                }
            }
        }
    }

    //Merge them together if they are very close
    RobotMerger merger(false);
    topPossibleRobots = merger.mergeTouchingPossibleRobots(frame, topSaliency, topPossibleRobots);
    botPossibleRobots = merger.mergeTouchingPossibleRobots(frame, botSaliency, botPossibleRobots);

}


/**
 * Looks for any bot and top robots who are aligned. If they are it updates the
 * RRCoord feet for the top robot to be at the bottom and merges/updates the bbox
 * of the top robot.
 */
std::vector<PossibleRobot> BottomDetectionMerger::complete(VisionFrame &frame,
        const Fovea &topSaliency,
            const Fovea &botSaliency,
            std::vector<PossibleRobot> &topPossibleRobots,
            std::vector<PossibleRobot> &botPossibleRobots) {

    mergeDetectedRobots(frame, topSaliency, botSaliency, topPossibleRobots, botPossibleRobots);

    return topPossibleRobots;
}
