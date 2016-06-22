#include "RobotFeetDetector.hpp"

static int saliencyHeight(const Fovea &saliency) {
    return saliency.bb.height();
}

std::vector<BBox> RobotFeetDetector::complete(
        VisionFrame &frame,
        const Fovea &saliency,
        const std::vector<Obstruction> &fieldEdgeObscures) {

    const static int MAX_GAP_BETWEEN_SECTION = 5;

    std::vector<BBox> obstructions;

    for (unsigned int i = 0; i < fieldEdgeObscures.size(); ++i) {
        const Obstruction &obscure = fieldEdgeObscures[i];
        const Point start = obscure.start;
        const Point end = obscure.end;

        const int start_row = MAX(start.y(), end.y());

        int current_row = start_row;
        int bottom_feet_row = current_row;
        int last_robot_row = current_row;

        while (current_row < saliencyHeight(saliency)) {
            bool hasNonBodyPartPixel = false;

            int totalWidth = 0;
            int totalGreen = 0;
            int totalRobotColours = 0;
            int totalBackground = 0;
            for (int col = start.x(); col <= end.x(); ++col) {
                const Colour &c = saliency.colour(col, current_row);
                if (c != cBODY_PART) {
                    hasNonBodyPartPixel = true;
                    if (c == cFIELD_GREEN) {
                        ++totalGreen;
                    }
                    else if (c == cROBOT_BLUE || c == cROBOT_RED || c == cWHITE) {
                        ++totalRobotColours;
                    }

                    else if (c == cBACKGROUND) {
                        ++totalBackground;
                    }
                    ++totalWidth;
                }
            }


            //Gotten to a point where the shoulder is now obscuring the whole
            //screen.
            if (!hasNonBodyPartPixel) {
                break;
            }

            double percentageGreen = (double) totalGreen / (double) totalWidth;
            double percentageRobotColours = (double) totalRobotColours / (double) totalWidth;
            double percentageBackground = (double) totalBackground / (double) totalWidth;
            const static double PERCENTAGE_ROBOT_THRESHOLD = 0.2F;
            const static double PERCENTAGE_GREEN_THRESHOLD = 0.7F;
            const static double PERCENTAGE_BACKGROUND_THRESHOLD = 0.8F;

            //Most likely shadows on the field. shouldn't extend down this.
            if (percentageBackground > PERCENTAGE_BACKGROUND_THRESHOLD) {
                //break;
            }

            if (percentageRobotColours > PERCENTAGE_ROBOT_THRESHOLD) {
                last_robot_row = current_row;
            }


            if (percentageGreen < PERCENTAGE_GREEN_THRESHOLD) {
                bottom_feet_row = current_row;
            }


            ++current_row;

            if (current_row - bottom_feet_row == MAX_GAP_BETWEEN_SECTION) {
                break;
            }
        }

        if (last_robot_row < bottom_feet_row) {
            bottom_feet_row = last_robot_row;
        }

        //If I didn't actually move the feet down from the horizontal then
        //it is unlikely to be a robot.
        if (bottom_feet_row == start_row) {
            continue;
        }

        Point topBox(start.x(), MIN(start.y(), end.y()));
        Point bottomBox(end.x(), bottom_feet_row);

        obstructions.push_back(BBox(topBox, bottomBox));

    }

    return obstructions;
}

